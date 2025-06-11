#pragma once
#include "../ast/AST_Node_Visitor.hpp"
#include "Symbols.hpp"
#include "../utils/SemanticError.hpp"
#include <vector>

class SemanticAnalyzer : public ASTVisitor {
private:
    SymbolTable symbolTable;
    
    void checkMathFunction(const std::string& funcName, const std::vector<ASTNode*>& args, int line);

    bool isValidIdentifier(const std::string& name);

    bool conformsTo(const std::string& subtype, const std::string& supertype);

    std::vector<SemanticError> errors;

public:
    void analyze(const std::vector<ASTNode*>& nodes);
    std::string inferParamUsageType(const std::string& paramName, ASTNode* body);
    void collectParamUsages(ASTNode* node, const std::string& paramName, std::set<std::string>& types);

    // std::string lowestCommonAncestor(const std::set<std::string>& types);
    // std::string commonAncestor(const std::string& t1, const std::string& t2);

    void visit(FunctionDeclarationNode& node) override;
    void visit(BinaryOperationNode& node) override;
    void visit(FunctionCallNode& node) override;
    void visit(ASTNode& node) override;
    void visit(LiteralNode& node) override;
    void visit(BlockNode& node) override;
    void visit(VariableDeclarationNode& node) override;
    void visit(IdentifierNode& node) override;
    void visit(LetNode& node) override;
    void visit(AssignmentNode& node) override;
    void visit(IfNode& node) override;
    void visit(WhileNode& node) override;
    void visit(ForNode& node) override;
    void visit(TypeDeclarationNode& node) override;
    void visit(NewInstanceNode& node) override;
    void visit(UnaryOpNode& node) override;
    void visit(BuiltInFunctionNode& node) override;
    void visit(AttributeDeclaration& node) override;
    void visit(MethodDeclaration& node) override;
    void visit(MethodCallNode& node) override;
    void visit(BaseCallNode& node) override;
    void visit(SelfCallNode& node) override;

    const std::vector<SemanticError>& getErrors() const { return errors; }

    SymbolTable& getSymbolTable();
};