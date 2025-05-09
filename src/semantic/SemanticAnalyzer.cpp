#include "SemanticAnalyzer.hpp"

void SemanticAnalyzer::analyze(const std::vector<ASTNode*>& nodes) {
    for (ASTNode* node : nodes) {
        node->accept(*this);
    }
}

void SemanticAnalyzer::visit(LiteralNode& node) {
    // Implementación básica
    node._type = node._type; // Mantiene el tipo del literal
}

void SemanticAnalyzer::visit(IdentifierNode& node) {
    Symbol* symbol = symbolTable.lookup(node.name);
    if (!symbol) {
        errors.emplace_back("Variable '" + node.name + "' no declarada", node.line());
        node._type = "Error";
    } else {
        node._type = symbol->type;
    }
}

SymbolTable& SemanticAnalyzer::getSymbolTable() {
    return symbolTable;
}