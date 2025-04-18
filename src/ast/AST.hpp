#pragma once
#include <vector>
#include <optional>
#include <string>
#include <memory>

// Clase base para todos los nodos del AST
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual int line() const = 0;
    virtual std::string type() const = 0;
};

// Nodos específicos del AST

class BinaryOpNode : public ASTNode {
public:
    std::string op;
    ASTNode* left;
    ASTNode* right;
    int _line;
    std::string _type;

    BinaryOpNode(std::string op, ASTNode* l, ASTNode* r, int ln)
        : op(op), left(l), right(r), _line(ln), _type("") {}

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class UnaryOpNode : public ASTNode {
    public:
        std::string op;
        ASTNode* operand;
        int _line;
        std::string _type;
    
        UnaryOpNode(const std::string& op, ASTNode* operand, int line)
            : op(op), operand(operand), _line(line), _type("") {}
    
        int line() const override { return _line; }
        std::string type() const override { return _type; }
    };

class BuiltInFunctionNode : public ASTNode {
public:
    std::string name;
    std::vector<ASTNode*> args;
    int _line;
    std::string _type;

    BuiltInFunctionNode(const std::string& name, const std::vector<ASTNode*>& args, int line)
        : name(name), args(args), _line(line), _type("") {}

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

// ------------------------------------------------------

class FunctionCallNode : public ASTNode {
public:
    std::string funcName;
    std::vector<ASTNode*> args;
    int _line;
    std::string _type;

    FunctionCallNode(std::string name, std::vector<ASTNode*> args, int ln)
        : funcName(name), args(args), _line(ln), _type("") {}

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

// ------------------------------------------------------

class LiteralNode : public ASTNode {
public:
    std::string value;
    std::string _type;
    int _line;

    LiteralNode(std::string val, std::string type, int ln)
        : value(val), _type(type), _line(ln) {}

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

// ------------------------------------------------------

class BlockNode : public ASTNode {
public:
    std::vector<ASTNode*> expressions;
    int _line;
    std::string _type;

    BlockNode(std::vector<ASTNode*> exprs, int ln)
        : expressions(exprs), _line(ln), _type("") {}

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

// ------------------------------------------------------

class VariableDeclarationNode : public ASTNode {
public:
    std::string name;
    std::string declaredType;
    ASTNode* initializer;
    bool isMutable;
    int _line;
    std::string _type;

    VariableDeclarationNode(std::string name, std::string type, ASTNode* init, bool isMut, int ln)
        : name(name), declaredType(type), initializer(init), isMutable(isMut), _line(ln), _type("") {}

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

// ------------------------------------------------------

class IdentifierNode : public ASTNode {
public:
    std::string name;
    int _line;
    std::string _type;

    IdentifierNode(std::string name, int ln)
        : name(name), _line(ln), _type("") {}

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

struct Parameter {
    std::string name;
    std::string type;
};
