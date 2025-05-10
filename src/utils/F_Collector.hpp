#pragma once
#include "../ast/AST_Node_Visitor.hpp"
#include "../semantic/Symbols.hpp"
#include "../utils/SemanticError.hpp"

class FunctionCollector : public ASTVisitor
{
private:
    SymbolTable &symbolTable;
    std::vector<SemanticError> &errors;

public:
    FunctionCollector(SymbolTable &symTable, std::vector<SemanticError> &err);
    virtual ~FunctionCollector() = default;

    void addBuiltins();

    void visit(ASTNode &node) override {}
    void visit(FunctionDeclarationNode &node) override;
};