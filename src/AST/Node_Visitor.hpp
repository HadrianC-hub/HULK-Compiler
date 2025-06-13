#pragma once

class ASTNode;
class AssignmentNode;
class AttributeDeclaration;
class BinaryOperationNode;
class BlockNode;
class BuiltInFunctionNode;
class ForNode;
class FunctionCallNode;
class FunctionDeclarationNode;
class IdentifierNode;
class IfNode;
class LetNode;
class LiteralNode;
class MethodCallNode;
class MethodDeclaration;
class NewInstanceNode;
class TypeDeclarationNode;
class UnaryOpNode;
class VariableDeclarationNode;
class WhileNode;
class BaseCallNode;
class SelfCallNode;

class ASTVisitor
{
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(ASTNode &node) = 0;
    virtual void visit(BinaryOperationNode &node) = 0;
    virtual void visit(FunctionCallNode &node) = 0;
    virtual void visit(FunctionDeclarationNode &node) = 0;
    virtual void visit(MethodCallNode &node) = 0;
    virtual void visit(LiteralNode &node) = 0;
    virtual void visit(BlockNode &node) = 0;
    virtual void visit(VariableDeclarationNode &node) = 0;
    virtual void visit(IdentifierNode &node) = 0;
    virtual void visit(LetNode &node) = 0;
    virtual void visit(AssignmentNode &node) = 0;
    virtual void visit(IfNode &node) = 0;
    virtual void visit(WhileNode &node) = 0;
    virtual void visit(ForNode &node) = 0;
    virtual void visit(TypeDeclarationNode &node) = 0;
    virtual void visit(NewInstanceNode &node) = 0;
    virtual void visit(AttributeDeclaration& node) = 0;
    virtual void visit(UnaryOpNode &node) = 0;
    virtual void visit(BuiltInFunctionNode &node) = 0;
    virtual void visit(MethodDeclaration& node) = 0;
    virtual void visit(BaseCallNode& node) = 0;
    virtual void visit(SelfCallNode& node) = 0;
};