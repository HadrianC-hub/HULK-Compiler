// TypeCollector.hpp
#pragma once
#include "../ast/NodeVisitor.hpp"
#include "Symbol.hpp"
#include "../Semantic.hpp"

class TypeCollector : public NodeVisitor
{
private:
    SymbolTable &symbolTable;
    std::vector<SemanticError> &errors;

public:
    TypeCollector(SymbolTable &symTable, std::vector<SemanticError> &err);
    virtual ~TypeCollector() = default;

    void visit(ASTNode &node) override {}
    void visit(TypeDeclaration &node) override;
    void visit(FuncDeclaration &node) override {}
    void visit(BinaryOperation &node) override {}
    void visit(FuncCall &node) override {}
    void visit(DataType &node) override {}
    void visit(Block &node) override {}
    void visit(VarDeclaration &node) override {}
    void visit(VarFuncName &node) override {}
    void visit(LetExpression &node) override {}
    void visit(Assignment &node) override {}
    void visit(IfExpression &node) override {}
    void visit(WhileLoop &node) override {}
    void visit(ForLoop &node) override {}
    void visit(InitInstance &node) override {}
    void visit(UnaryOperation &node) override {}
    void visit(BuiltInFunc &node) override {}
    void visit(AttributeDeclaration &node) override {}
    void visit(MethodDeclaration &node) override {}
    void visit(MethodCall &node) override {}
    void visit(BaseCall &node) override {}
    void visit(SelfCall &node) override {}
};