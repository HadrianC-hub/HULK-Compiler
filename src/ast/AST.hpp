#pragma once
#include "NodeVisitor.hpp"
#include <vector>
#include <optional>
#include <string>
#include <memory>

class ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual void accept(NodeVisitor &visitor) = 0;
    virtual int line() const = 0;
    virtual std::string type() const = 0;
};

struct Parameter
{
    std::string name;
    std::string type;
};

struct LetDeclaration
{
    std::string name;
    std::string declaredType;
    ASTNode *initializer;
};

struct IfBranch
{
    ASTNode *condition;
    ASTNode *body;
};

struct TypeBody
{
    std::vector<AttributeDeclaration> *attributes;
    std::vector<MethodDeclaration> *methods;

    TypeBody(std::vector<AttributeDeclaration> *attributes, std::vector<MethodDeclaration> *methods)
        : attributes(attributes), methods(methods) {}
};

struct AttributeDeclaration
{
    std::string name;
    ASTNode *initializer;

    AttributeDeclaration(std::string name, ASTNode *init)
        : name(std::move(name)), initializer(init) {}
};

struct MethodDeclaration
{
    std::string name;
    std::vector<Parameter> *params;
    ASTNode *body;
    std::string returnType;

    MethodDeclaration(std::string name, std::vector<Parameter> *params, ASTNode *body, std::string ret = "")
        : name(std::move(name)), params(params), body(body), returnType(std::move(ret)) {}
};

// Nodos del AST

class UnaryOperation : public ASTNode
{
public:
    std::string op;
    ASTNode *operand;
    int _line;
    std::string _type;

    UnaryOperation(const std::string &op, ASTNode *operand, int line)
        : op(op), operand(operand), _line(line), _type("") {}

    void accept(NodeVisitor &visitor) override
    {
        visitor.visit(*this);
    }

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class BinaryOperation : public ASTNode
{
public:
    std::string op;
    ASTNode *left;
    ASTNode *right;
    int _line;
    std::string _type;

    BinaryOperation(std::string op, ASTNode *l, ASTNode *r, int ln)
        : op(op), left(l), right(r), _line(ln), _type("") {}

    void accept(NodeVisitor &visitor) override
    {
        visitor.visit(*this);
    }

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class BuiltInFunc : public ASTNode
{
public:
    std::string name;
    std::vector<ASTNode *> args;
    int _line;
    std::string _type;

    BuiltInFunc(const std::string &name, const std::vector<ASTNode *> &args, int line)
        : name(name), args(args), _line(line), _type("") {}

    void accept(NodeVisitor &visitor) override
    {
        visitor.visit(*this);
    }

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class FuncCall : public ASTNode
{
public:
    std::string funcName;
    std::vector<ASTNode *> args;
    int _line;
    std::string _type;

    FuncCall(std::string name, std::vector<ASTNode *> args, int ln)
        : funcName(name), args(args), _line(ln), _type("") {}

    void accept(NodeVisitor &visitor) override
    {
        visitor.visit(*this);
    }

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class DataType : public ASTNode
{
public:
    std::string value;
    std::string _type;
    int _line;

    DataType(std::string val, std::string type, int ln)
        : value(val), _type(type), _line(ln) {}

    void accept(NodeVisitor &visitor) override
    {
        visitor.visit(*this);
    }

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class Block : public ASTNode
{
public:
    std::vector<ASTNode *> expressions;
    int _line;
    std::string _type;

    Block(std::vector<ASTNode *> exprs, int ln)
        : expressions(exprs), _line(ln), _type("") {}

    void accept(NodeVisitor &visitor) override
    {
        visitor.visit(*this);
    }

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class VarDeclaration : public ASTNode
{
public:
    std::string name;
    std::string declaredType;
    ASTNode *initializer;
    bool isMutable;
    int _line;
    std::string _type;

    VarDeclaration(std::string name, std::string type, ASTNode *init, bool isMut, int ln)
        : name(name), declaredType(type), initializer(init), isMutable(isMut), _line(ln), _type("") {}

    void accept(NodeVisitor &visitor) override
    {
        visitor.visit(*this);
    }

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class VarFuncName : public ASTNode
{
public:
    std::string name;
    int _line;
    std::string _type;

    VarFuncName(std::string name, int ln)
        : name(name), _line(ln), _type("") {}

    void accept(NodeVisitor &visitor) override
    {
        visitor.visit(*this);
    }

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class FuncDeclaration : public ASTNode
{
public:
    std::string name;
    std::string returnType = "";
    std::vector<Parameter> *params;
    ASTNode *body;
    bool isInline;
    int _line;
    std::string _type;

    FuncDeclaration(std::string name,
                    std::vector<Parameter> *params, ASTNode *body,
                    bool isInline, int ln)
        : name(name), params(params),
          body(body), isInline(isInline), _line(ln), _type("") {}

    void accept(NodeVisitor &visitor) override
    {
        visitor.visit(*this);
    }

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class LetExpression : public ASTNode
{
public:
    std::vector<LetDeclaration> *declarations;
    ASTNode *body;
    int _line;
    std::string _type;

    LetExpression(std::vector<LetDeclaration> *decls, ASTNode *body, int ln)
        : declarations(decls), body(body), _line(ln), _type("") {}

    void accept(NodeVisitor &visitor) override
    {
        visitor.visit(*this);
    }

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class Assignment : public ASTNode
{
public:
    ASTNode *name;
    ASTNode *rhs;
    int _line;
    std::string _type;

    Assignment(ASTNode *name, ASTNode *rhs, int ln)
        : name(name), rhs(rhs), _line(ln), _type("") {}

    void accept(NodeVisitor &visitor) override
    {
        visitor.visit(*this);
    }

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class IfExpression : public ASTNode
{
public:
    std::vector<IfBranch> *branches;
    ASTNode *elseBody;
    int _line;
    std::string _type;

    IfExpression(std::vector<IfBranch> *branches, ASTNode *elseBody, int ln)
        : branches(branches), elseBody(elseBody), _line(ln), _type("") {}

    void accept(NodeVisitor &visitor) override
    {
        visitor.visit(*this);
    }

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class WhileLoop : public ASTNode
{
public:
    ASTNode *condition;
    ASTNode *body;
    int _line;
    std::string _type;

    WhileLoop(ASTNode *cond, ASTNode *b, int ln)
        : condition(cond), body(b), _line(ln), _type("") {}

    void accept(NodeVisitor &visitor) override
    {
        visitor.visit(*this);
    }

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class ForLoop : public ASTNode
{
public:
    std::string varName;
    ASTNode *iterable;
    ASTNode *init_range;
    ASTNode *end_range;
    ASTNode *body;
    int _line;
    std::string _type;

    ForLoop(const std::string &var, ASTNode *init, ASTNode *end, ASTNode *b, int ln)
        : varName(var), init_range(init), end_range(end), body(b), _line(ln), _type("") {}

    void accept(NodeVisitor &visitor) override
    {
        visitor.visit(*this);
    }

    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class TypeDeclaration : public ASTNode
{
public:
    std::string name;
    std::vector<Parameter> *constructorParams;
    TypeBody *body;
    std::optional<std::string> baseType;
    std::vector<ASTNode *> baseArgs;
    int _line;

    TypeDeclaration(std::string name,
                        std::vector<Parameter> *params,
                        TypeBody *body,
                        std::optional<std::string> baseType,
                        std::vector<ASTNode *> baseArgs,
                        int line)
        : name(std::move(name)), constructorParams(params),
          body(body),
          baseType(std::move(baseType)), baseArgs(std::move(baseArgs)),
          _line(line) {}

    void accept(NodeVisitor &v) override { v.visit(*this); }
    int line() const override { return _line; }
    std::string type() const override { return "Type"; }
};

class InitInstance : public ASTNode
{
public:
    std::string typeName;
    std::vector<ASTNode *> args;
    int _line;
    std::string _type;

    InitInstance(std::string typeName, std::vector<ASTNode *> args, int line)
        : typeName(std::move(typeName)), args(std::move(args)), _line(line), _type("") {}

    void accept(NodeVisitor &v) override { v.visit(*this); }
    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class MethodCall : public ASTNode
{
public:
    std::string instanceName;
    std::string methodName;
    std::vector<ASTNode *> args;
    int _line;
    std::string _type;

    MethodCall(std::string instanceName, std::string methodName, std::vector<ASTNode *> args, int line)
        : instanceName(instanceName), methodName(std::move(methodName)), args(std::move(args)), _line(line), _type("") {}

    void accept(NodeVisitor &v) override { v.visit(*this); }
    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class BaseCall : public ASTNode
{
public:
    std::vector<ASTNode *> args;
    int _line;
    std::string _type;

    BaseCall(std::vector<ASTNode *> args, int line)
        : args(std::move(args)), _line(line), _type("") {}

    void accept(NodeVisitor &v) override { v.visit(*this); }
    int line() const override { return _line; }
    std::string type() const override { return _type; }
};

class SelfCall : public ASTNode
{
public:
    std::string varName;
    int _line;
    std::string _type;

    SelfCall(const std::string &varName, int line)
        : varName(varName), _line(line), _type("") {}

    void accept(NodeVisitor &v) override { v.visit(*this); }
    int line() const override { return _line; }
    std::string type() const override { return _type; }
};