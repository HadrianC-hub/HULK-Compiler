#pragma once

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "Types.hpp"

#include <string>
#include <map>
#include <vector>

class ASTNode;
class FuncDeclaration;

class Context
{
public:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    llvm::Module module;

    std::map<std::string, llvm::Value *> locals;
    std::vector<llvm::Value *> valueStack;

    TypeSystem typeSystem;

    Context();

    void Generate(std::vector<ASTNode *> &root);
    void WriteDownCode(const std::string &filename = "hulk.ll");

    std::vector<std::map<std::string, llvm::Value *>> localScopes;
    std::vector<std::map<std::string, FuncDeclaration *>> functionScopes;

    void PushVar(bool inherit = true)
    {
        if (inherit && !localScopes.empty())
        {
            // Crear nuevo scope (herencia)
            localScopes.push_back(localScopes.back());
        }
        else
        {
            // Create scope vacio (no herencia - funciones)
            localScopes.emplace_back();
        }
    }
    void PopVar() { localScopes.pop_back(); }
    void addLocal(const std::string &name, llvm::Value *val)
    {
        if (!localScopes.empty()) {
            // Agregar o actualizar variables en el scope actual 
            // (sobreescribe a cualquier variable con el mismo nombre en scope de padres)
            localScopes.back()[name] = val;
        }
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

    void PushFunc()
    {
        if (functionScopes.empty())
        {
            functionScopes.emplace_back();
        }
        else
        {
            functionScopes.push_back(functionScopes.back());
        }
    }
    void PopFunc() { functionScopes.pop_back(); }
    void addFuncDecl(const std::string &name, FuncDeclaration *decl)
    {
        if (!functionScopes.empty())
            functionScopes.back()[name] = decl;
    }
    FuncDeclaration *lookupFuncDecl(const std::string &name) const
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
