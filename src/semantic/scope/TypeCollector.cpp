// TypeCollector.cpp
#include "TypeCollector.hpp"
#include "../ast/AST.hpp"
#include <iostream>

TypeCollector::TypeCollector(SymbolTable &symTable, std::vector<SemanticError> &err)
    : symbolTable(symTable), errors(err) {}

void TypeCollector::visit(TypeDeclaration &node) {
    std::vector<std::string> paramNames;
    for (const auto &param : *node.constructorParams) {
        paramNames.push_back(param.name);
    }
    
    std::string parent = node.baseType.value_or("");
    
    if (!symbolTable.addType(node.name, parent, paramNames, node.line())) {
        errors.emplace_back("Tipo '" + node.name + "' ya definido", node.line());
    } else {
        std::cout << "[COLECTOR] Tipo registrado: " << node.name 
                  << " (padre: " << (parent.empty() ? "ninguno" : parent) 
                  << ") en linea " << node.line() << "\n";
    }
}