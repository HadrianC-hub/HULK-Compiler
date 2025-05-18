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
    void visit(BinaryOperationNode &node) override {}
    void visit(FunctionCallNode &node) override {}
    void visit(LiteralNode &node) override {}
    void visit(BlockNode &node) override {}
    void visit(VariableDeclarationNode &node) override {}
    void visit(IdentifierNode &node) override {}
    void visit(LetNode &node) override {}
    void visit(AssignmentNode &node) override {}
    void visit(IfNode &node) override {}
    void visit(WhileNode &node) override {}
    void visit(ForNode &node) override {}
    void visit(TypeDeclarationNode &node) override {}
    void visit(NewInstanceNode &node) override {}
    void visit(UnaryOpNode &node) override {}
    void visit(BuiltInFunctionNode &node) override {}
    void visit(MethodCallNode &node) override {}
    // void visit(AttributeDeclaration& node) override {}
    // void visit(MethodDeclaration& node) override {}
};