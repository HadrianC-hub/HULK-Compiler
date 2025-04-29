#include "Symbols.hpp"

SymbolTable::SymbolTable() {
    enterScope(); // Scope global
    // Registrar constantes matemáticas
    addSymbol("pi", "Number", true);
    addSymbol("e", "Number", true);
    addType("Object", "");
    addType("Number", "Object");
    addType("String", "Object");
    addType("Boolean", "Object");
}
















