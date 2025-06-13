#pragma once
#include "../ast/NodeVisitor.hpp"
#include "scope/Symbol.hpp"
#include <vector>
#include "SemanticError.hpp"

class SemanticAnalyzer : public ASTVisitor
{
private:
    SymbolTable symbolTable;

    void checkMathFunction(const std::string &funcName, const std::vector<ASTNode *> &args, int line);

    bool isValidIdentifier(const std::string &name);

    bool conformsTo(const std::string &subtype, const std::string &supertype);

    std::vector<SemanticError> errors;
    std::string currentMethodContext;

public:
    void analyze(const std::vector<ASTNode *> &nodes);
    std::string inferParamUsageType(const std::string &paramName, ASTNode *body);
    void collectParamUsages(ASTNode *node, const std::string &paramName, std::set<std::string> &types);
    Symbol *lookupMethodInHierarchy(const std::string &typeName, const std::string &methodName);

    // Métodos visit
    void visit(FunctionDeclarationNode &node) override;
    void visit(BinaryOpNode &node) override;
    void visit(FunctionCallNode &node) override;
    void visit(ASTNode &node) override;
    void visit(LiteralNode &node) override;
    void visit(BlockNode &node) override;
    void visit(VariableDeclarationNode &node) override;
    void visit(IdentifierNode &node) override;
    void visit(LetNode &node) override;
    void visit(AssignmentNode &node) override;
    void visit(IfNode &node) override;
    void visit(WhileNode &node) override;
    void visit(ForNode &node) override;
    void visit(TypeDeclarationNode &node) override;
    void visit(NewInstanceNode &node) override;
    void visit(UnaryOpNode &node) override;
    void visit(BuiltInFunctionNode &node) override;
    void visit(AttributeDeclaration &node) override;
    void visit(MethodDeclaration &node) override;
    void visit(MethodCallNode &node) override;
    void visit(BaseCallNode &node) override;
    void visit(SelfCallNode &node) override;

    const std::vector<SemanticError> &getErrors() const { return errors; }

    SymbolTable &getSymbolTable();
};
