#pragma once

#include "../ast/NodeVisitor.hpp"
#include "llvm/IR/Value.h"

class Context;

class IRGenerator : public NodeVisitor {
private:
    Context& context;

public:
    IRGenerator(Context& ctx);

    void visit(class DataType& node) override;
    void visit(class BinaryOperation& node) override;
    void visit(class UnaryOperation& node) override;
    void visit(class BuiltInFunc& node) override;
    void visit(class Block& node) override;
    void visit(class VarFuncName& node) override;
    void visit(class FuncDeclaration& node) override;
    void visit(class FuncCall& node) override;
    void visit(class LetExpression& node) override;
    void visit(class Assignment& node) override;
    void visit(class IfExpression& node) override;
    void visit(class WhileLoop& node) override;
    void visit(class ForLoop& node) override;

    //Faltan por implementar
    void visit(ASTNode&) override {}
    void visit(VarDeclaration&) override {}
    void visit(TypeDeclaration&) override {}
    void visit(InitInstance&) override {}
    void visit(AttributeDeclaration& node) override {}
    void visit(MethodDeclaration& node) override {}
    void visit(MethodCall& node) override {}
    void visit(OriginCall&) override {}
    void visit(SelfCall&) override {}
};
