#pragma once

// Nodos a visitar
class ASTNode;
class BinaryOperation;
class FuncCall;
class DataType;
class Block;
class VarDeclaration;
class VarFuncName;
class FuncDeclaration;
class LetExpression;
class Assignment;
class IfExpression;
class WhileLoop;
class ForLoop;
class TypeDeclaration;
class InitInstance;
class UnaryOperation;
class BuiltInFunc;
class AttributeDeclaration;
class MethodDeclaration;
class MethodCall;
class BaseCall;
class SelfCall;

class NodeVisitor
{
public:
    virtual ~NodeVisitor() = default;
    virtual void visit(ASTNode &node) = 0;
    virtual void visit(BinaryOperation &node) = 0;
    virtual void visit(FuncCall &node) = 0;
    virtual void visit(DataType &node) = 0;
    virtual void visit(Block &node) = 0;
    virtual void visit(VarDeclaration &node) = 0;
    virtual void visit(VarFuncName &node) = 0;
    virtual void visit(FuncDeclaration &node) = 0;
    virtual void visit(LetExpression &node) = 0;
    virtual void visit(Assignment &node) = 0;
    virtual void visit(IfExpression &node) = 0;
    virtual void visit(WhileLoop &node) = 0;
    virtual void visit(ForLoop &node) = 0;
    virtual void visit(TypeDeclaration &node) = 0;
    virtual void visit(InitInstance &node) = 0;
    virtual void visit(AttributeDeclaration &node) = 0;
    virtual void visit(MethodDeclaration &node) = 0;
    virtual void visit(MethodCall &node) = 0;
    virtual void visit(BaseCall &node) = 0;
    virtual void visit(SelfCall &node) = 0;
    virtual void visit(UnaryOperation &node) = 0;
    virtual void visit(BuiltInFunc &node) = 0;
};

