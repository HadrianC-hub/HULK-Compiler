// TypeCollector.cpp
#include "TypeCollector.hpp"
#include "../ast/AST.hpp"

TypeCollector::TypeCollector(SymbolTable &symTable, std::vector<SemanticError> &err)
    : symbolTable(symTable), errors(err) {}

void TypeCollector::visit(TypeDeclaration &node)
{
    // Registrar solo la declaración básica del tipo (sin cuerpo)
    std::vector<std::string> paramNames;
    for (const auto &param : *node.constructorParams) {
        paramNames.push_back(param.name);
    }
    
    std::string parent = node.baseType.value_or("");
    
    if (!symbolTable.addType(node.name, parent, paramNames)) {
        errors.emplace_back("Tipo '" + node.name + "' ya definido", node.line());
    }
}