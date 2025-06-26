#pragma once
#include "../ast/NodeVisitor.hpp"
#include "scope/Symbol.hpp"
#include <vector>
#include <string>
#include <map>

struct SemanticError {
    std::string message;
    int line;
    SemanticError(const std::string& msg, int ln) : message(msg), line(ln) {}
};

class SemanticValidation : public NodeVisitor
{
private:
    SymbolTable symbolTable;

    void checkMathFunction(const std::string &funcName, const std::vector<ASTNode *> &args, int line);

    bool isValidIdentifier(const std::string &name);

    bool conformsTo(const std::string &subtype, const std::string &supertype);

    bool checkInheritanceCycles();

    std::map<std::string, int> lineMap; // Mapa de tipo -> línea

    bool dfsInheritance(const std::string &typeName, 
                        std::map<std::string, int> &color,
                        std::vector<std::string> &path,
                        std::map<std::string, int> &lineMap); // Cambio en la firma

    std::vector<SemanticError> errors;
    std::string currentMethodContext;

public:
    void validate(const std::vector<ASTNode *> &nodes);
    std::string inferParamUsageType(const std::string &paramName, ASTNode *body);
    void collectParamUsages(ASTNode *node, const std::string &paramName, std::set<std::string> &types);
    Symbol *lookupMethodInHierarchy(const std::string &typeName, const std::string &methodName);

    // Metodos visit
    void visit(FuncDeclaration &node) override;
    void visit(BinaryOperation &node) override;
    void visit(FuncCall &node) override;
    void visit(ASTNode &node) override;
    void visit(DataType &node) override;
    void visit(Block &node) override;
    void visit(VarDeclaration &node) override;
    void visit(VarFuncName &node) override;
    void visit(LetExpression &node) override;
    void visit(Assignment &node) override;
    void visit(IfExpression &node) override;
    void visit(WhileLoop &node) override;
    void visit(ForLoop &node) override;
    void visit(TypeDeclaration &node) override;
    void visit(InitInstance &node) override;
    void visit(UnaryOperation &node) override;
    void visit(BuiltInFunc &node) override;
    void visit(AttributeDeclaration &node) override;
    void visit(MethodDeclaration &node) override;
    void visit(MethodCall &node) override;
    void visit(BaseCall &node) override;
    void visit(SelfCall &node) override;

    const std::vector<SemanticError> &getErrors() const { return errors; }

    SymbolTable &getSymbolTable();
};