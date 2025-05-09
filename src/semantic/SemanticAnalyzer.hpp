#pragma once
#include "../ast/AST_Node_Visitor.hpp"
#include "Symbols.hpp"
#include "../utils/SemanticError.hpp"
#include <vector>

class SemanticAnalyzer : public ASTVisitor {
private:
    SymbolTable symbolTable;
    std::vector<SemanticError> errors;

public:
    void analyze(const std::vector<ASTNode*>& nodes);
    SymbolTable& getSymbolTable();
    
    void visit(ASTNode& node) override {}
    void visit(LiteralNode& node) override;
    void visit(IdentifierNode& node) override;
    void visit(VariableDeclarationNode& node) override;

    
    const std::vector<SemanticError>& getErrors() const { return errors; }
};