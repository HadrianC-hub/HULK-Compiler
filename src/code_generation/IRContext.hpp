#pragma once

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

#include <string>
#include <map>
#include <vector>

class ASTNode;
class FunctionDeclarationNode;

class CodeGenContext
{
public:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    llvm::Module module;

    std::map<std::string, llvm::Value *> locals;
    std::vector<llvm::Value *> valueStack;

    CodeGenContext();

    void generateCode(std::vector<ASTNode *> &root);
    void dumpIR(const std::string &filename = "hulk-low-code.ll");

    std::vector<std::map<std::string, llvm::Value *>> localScopes;
    std::vector<std::map<std::string, FunctionDeclarationNode *>> functionScopes;

    void pushVarScope() { localScopes.emplace_back(); }
    void popVarScope() { localScopes.pop_back(); }
    void addLocal(const std::string &name, llvm::Value *val)
    {
        if (!localScopes.empty())
            localScopes.back()[name] = val;
    }
    llvm::Value *lookupLocal(const std::string &name) const
    {
        for (auto it = localScopes.rbegin(); it != localScopes.rend(); ++it)
        {
            auto found = it->find(name);
            if (found != it->end())
                return found->second;
        }
        return nullptr;
    }

    void pushFuncScope() { functionScopes.emplace_back(); }
    void popFuncScope() { functionScopes.pop_back(); }
    void addFuncDecl(const std::string &name, FunctionDeclarationNode *decl)
    {
        if (!functionScopes.empty())
            functionScopes.back()[name] = decl;
    }
    FunctionDeclarationNode *lookupFuncDecl(const std::string &name) const
    {
        for (auto it = functionScopes.rbegin(); it != functionScopes.rend(); ++it)
        {
            auto found = it->find(name);
            if (found != it->end())
                return found->second;
        }
        return nullptr;
    }
};
