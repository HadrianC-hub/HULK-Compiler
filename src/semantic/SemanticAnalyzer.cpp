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

void SemanticAnalyzer::visit(BinaryOperationNode& node) {
    node.left->accept(*this);
    node.right->accept(*this);
    
    std::string leftType = node.left->type();
    std::string rightType = node.right->type();
    
    if (node.op == "+" || node.op == "-" || node.op == "*" || node.op == "/") {
        if (leftType != "Number" || rightType != "Number") {
            errors.emplace_back("Operandos deben ser números", node.line());
            node._type = "Error";
        } else {
            node._type = "Number";
        }
    } 
    else if (node.op == "==" || node.op == "!=") {
        node._type = "Boolean";
    }
    else {
        errors.emplace_back("Operador no soportado", node.line());
        node._type = "Error";
    }
}

void SemanticAnalyzer::visit(IfNode& node) {
    for (auto& branch : *node.branches) {
        branch.condition->accept(*this);
        if (branch.condition->type() != "Boolean") {
            errors.emplace_back("La condición debe ser booleana", branch.condition->line());
        }
        branch.body->accept(*this);
    }

    if (node.elseBody) {
        node.elseBody->accept(*this);
    }

    // Versión inicial sin chequeo de tipos consistentes en ramas
    node._type = "Void"; 
}

void SemanticAnalyzer::visit(WhileNode& node) {
    node.condition->accept(*this);
    if (node.condition->type() != "Boolean") {
        errors.emplace_back("La condición del while debe ser booleana", node.line());
    }

    node.body->accept(*this);
    node._type = "Void"; // Versión inicial simplificada
}

SymbolTable& SemanticAnalyzer::getSymbolTable() {
    return symbolTable;
}