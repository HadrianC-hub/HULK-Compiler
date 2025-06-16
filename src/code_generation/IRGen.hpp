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
    void visit(class TypeDeclaration& node) override;
    void visit(class InitInstance& node) override;
    void visit(class MethodCall& node) override;
    void visit(class SelfCall& node) override; 
    void visit(class OriginCall& node) override;

    //Faltan por implementar
    void visit(ASTNode&) override {}
    void visit(VarDeclaration&) override {}
    void visit(AttributeDeclaration& node) override {}
    void visit(MethodDeclaration& node) override {}
    
    
};
