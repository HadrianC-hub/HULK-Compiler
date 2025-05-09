#include "SemanticAnalyzer.hpp"

void SemanticAnalyzer::analyze(const std::vector<ASTNode*>& nodes) {
    for (ASTNode* node : nodes) {
        node->accept(*this);
    }
}

void SemanticAnalyzer::visit(LiteralNode& node) {
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

void SemanticAnalyzer::visit(VariableDeclarationNode& node) {
    if (symbolTable.existsInCurrentScope(node.name)) {
        errors.emplace_back("Variable '" + node.name + "' ya declarada", node.line());
        return;
    }

    if (node.initializer) {
        node.initializer->accept(*this);
        std::string initType = node.initializer->type();
        
        if (!node.declaredType.empty() && node.declaredType != initType) {
            errors.emplace_back("Tipo declarado no coincide con inicializador", node.line());
        }
        
        node._type = !node.declaredType.empty() ? node.declaredType : initType;
    } else {
        if (node.declaredType.empty()) {
            errors.emplace_back("Variable sin tipo ni inicializador", node.line());
            return;
        }
        node._type = node.declaredType;
    }

    symbolTable.addSymbol(node.name, node._type, !node.isMutable);
}

SymbolTable& SemanticAnalyzer::getSymbolTable() {
    return symbolTable;
}