#include "F_Collector.hpp"
#include "../ast/AST.hpp"
#include <iostream>

FunctionCollector::FunctionCollector(SymbolTable& symTable, std::vector<SemanticError>& err)
    : symbolTable(symTable), errors(err) {}


void FunctionCollector::addBuiltins() {
    symbolTable.addFunction("sin", "Number", {"Number"});
    symbolTable.addFunction("cos", "Number", {"Number"});
    symbolTable.addFunction("exp", "Number", {"Number"});
    symbolTable.addFunction("sqrt", "Number", {"Number"});
    symbolTable.addFunction("log", "Number", {"Number", "Number"});
    symbolTable.addFunction("min", "Number", {"Number", "Number"});
    symbolTable.addFunction("max", "Number", {"Number", "Number"});
    symbolTable.addFunction("rand", "Number", {});
    symbolTable.addFunction("print", "Null", {"Object"});
}
