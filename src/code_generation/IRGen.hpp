#pragma once

#include "../ast/NodeVisitor.hpp"
#include "llvm/IR/Value.h"

class CodeGenContext;

// This class implements the visitor pattern for generating LLVM IR
class LLVMGenerator : public ASTVisitor {
private:
    CodeGenContext& context;

public:
    LLVMGenerator(CodeGenContext& ctx);

    // Implemented nodes
    void visit(class LiteralNode& node) override;
    void visit(class BinaryOpNode& node) override;
    void visit(class UnaryOpNode& node) override;
    void visit(class BuiltInFunctionNode& node) override;
    void visit(class BlockNode& node) override;
    void visit(class IdentifierNode& node) override;
    void visit(class FunctionDeclarationNode& node) override;
    void visit(class FunctionCallNode& node) override;

    // Stubs for other nodes
    void visit(ASTNode&) override {}
    void visit(VariableDeclarationNode&) override {}
    void visit(LetNode&) override {}
    void visit(AssignmentNode&) override {}
    void visit(IfNode&) override {}
    void visit(WhileNode&) override {}
    void visit(ForNode&) override {}
    void visit(TypeDeclarationNode&) override {}
    void visit(NewInstanceNode&) override {}
    void visit(AttributeDeclaration& node) override {}
    void visit(MethodDeclaration& node) override {}
    void visit(MethodCallNode& node) override {}
    void visit(BaseCallNode&) override {}
    void visit(SelfCallNode&) override {}
};
