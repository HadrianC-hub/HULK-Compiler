# === VARIABLES GLOBALES ===

# Herramientas
CXX     := g++
BISON   := bison
FLEX    := flex
LLVM	:= llvm-config
LLC     := llc

CXXFLAGS := -std=c++17 -Wall -Wextra -g -Isrc -Isrc/ast -Wno-free-nonheap-object -finput-charset=UTF-8 -fexec-charset=UTF-8

LDFLAGS += -lfl -lstdc++

LLVM_CXXFLAGS := $(filter-out -fno-exceptions, $(shell $(LLVM) --cxxflags))
LLVM_LDFLAGS  := $(shell $(LLVM) --ldflags --libs all --system-libs)

# Directorios
BUILD_DIR := .build
SRC_DIR := src
LEXER_DIR := $(SRC_DIR)/lexer
PARSER_DIR := $(SRC_DIR)/parser
OUTPUT_DIR := Hulk

# Archivos fuentes
LEX_SRC := $(LEXER_DIR)/lexer.l
YACC_SRC = $(PARSER_DIR)/parser.y
MAIN_SRC = $(SRC_DIR)/hulk.cpp

# Archivos generados por flex y bison
PARSER_SRC := $(BUILD_DIR)/parser.tab.cpp
PARSER_HEADER := $(BUILD_DIR)/parser.tab.hpp
LEXER_SRC := $(BUILD_DIR)/lex.yy.cpp

# Objetos explicitos
MAIN_OBJ = $(BUILD_DIR)/hulk.o
LEX_OBJ = $(BUILD_DIR)/lexer/lex.yy.o
YACC_OBJ = $(BUILD_DIR)/parser/parser.tab.o

# Detectar automaticamente todos los *.cpp de src/
CPP_SRC := $(shell find $(SRC_DIR) -name "*.cpp" ! -name "hulk.cpp" ! -name "hulk_utils.cpp")
CPP_OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CPP_SRC))

# Funciones auxiliares en C
RUNTIME_SRC := $(SRC_DIR)/Utils/hulk_utils.cpp
RUNTIME_OBJ := $(BUILD_DIR)/Utils/hulk_utils.o

OBJS := $(MAIN_OBJ) $(CPP_OBJ) $(YACC_OBJ) $(LEX_OBJ) $(RUNTIME_OBJ)

EXEC := Compiler.exe
INPUT_FILE := $(word 2, $(MAKECMDGOALS))
LLVM_IR := $(OUTPUT_DIR)/Hulk-IR.ll
HULK_OBJ := $(OUTPUT_DIR)/Hulk-IR.o
CODE := $(OUTPUT_DIR)/Hulk.exe

# === TARGETS ===
all: build

build: $(BUILD_DIR) $(EXEC)
	@echo "[CHECK] Build completo. Ejecutable en $(EXEC)"

run: build $(LLVM_IR) $(CODE)

compile: run

execute:
	@if [ -f "$(CODE)" ]; then \
		echo "⚡ Ejecutable ya existe. Ejecutando..."; \
	else \
		echo "🔧 Ejecutable no encontrado. Compilando..."; \
		$(MAKE) $(CODE); \
	fi
	@echo "🚀 Ejecutando programa..."
	@./$(CODE)
	@echo "🏁 Ejecucion completada"

# Generacion del IR
$(LLVM_IR): build | $(OUTPUT_DIR)
	@./$(EXEC) $(INPUT_FILE)

# Compilar LLVM IR a objeto
$(HULK_OBJ): $(LLVM_IR)
	$(LLC) -filetype=obj $< -o $@

# Compilar a ejecutable
$(CODE): $(HULK_OBJ) $(RUNTIME_OBJ)
	$(CXX) $(HULK_OBJ) $(RUNTIME_OBJ) -o $@ $(LLVM_LDFLAGS) -lm
	@echo "🔨 Generado ejecutable: $(CODE)"

clean:
	rm -rf $(BUILD_DIR) $(EXEC) $(OUTPUT_DIR)
	@echo "🧹 Proyecto limpiado."

# === REGLAS DE COMPILACIoN ===

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/lexer:
	mkdir -p $@

$(BUILD_DIR)/parser:
	mkdir -p $@

$(OUTPUT_DIR):
	mkdir -p $@

# Generar parser
$(PARSER_SRC) $(PARSER_HEADER): $(PARSER_DIR)/parser.y | $(BUILD_DIR)/parser
	$(BISON) -d -o $(PARSER_SRC) --defines=$(PARSER_HEADER) -Wno-deprecated $<

# Generar lexer
$(LEXER_SRC): $(LEXER_DIR)/lexer.l $(PARSER_HEADER) | $(BUILD_DIR)/lexer
	$(FLEX) -o $@ $<

# Compilar archivos generados
$(LEX_OBJ): $(LEXER_SRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(YACC_OBJ): $(PARSER_SRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(RUNTIME_OBJ): $(RUNTIME_SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) -c $< -o $@

# Compilar hulk.cpp y otros .cpp
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) -c $< -o $@

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LLVM_CXXFLAGS) -o $(EXEC) $(OBJS) $(LLVM_LDFLAGS)
	@echo "[CHECK] Compilacion completa. Ejecutable en $(EXEC)"

# === META ===
.PHONY: all build run compile execute clean
