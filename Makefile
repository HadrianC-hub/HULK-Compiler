# === VARIABLES GLOBALES ===

# Herramientas
CXX     := g++

CXXFLAGS := -std=c++17 -Wall -Wextra -g	-Isrc	-Isrc/ast	-Wno-free-nonheap-object
LDFLAGS += -lfl -lstdc++


# Directorios
BUILD_DIR := .build
SRC_DIR := src
MAIN_SRC = $(SRC_DIR)/hulk.cpp

# Objetos
MAIN_OBJ = $(BUILD_DIR)/hulk.o

# Detectar automáticamente todos los *.cpp de src/
CPP_SRC := $(shell find $(SRC_DIR) -name "*.cpp" ! -name "hulk.cpp")
CPP_OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CPP_SRC))

OBJS := $(MAIN_OBJ) $(CPP_OBJ) $(YACC_OBJ) $(LEX_OBJ) $(RUNTIME_OBJ)

EXEC := Compiler.exe
INPUT_FILE := $(word 2, $(MAKECMDGOALS))
LLVM_IR := $(OUTPUT_DIR)/Hulk-IR.ll
CODE := $(OUTPUT_DIR)/Hulk.exe

