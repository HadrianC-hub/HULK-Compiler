#include <cstdio>
#include <iostream>
#include "../ast/AST.hpp"
#include "IRContext.hpp"
#include "IRGen.hpp"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"

Context::Context()
    : builder(context), module("hulk_module", context) {}

void Context::Generate(std::vector<ASTNode *> &root)
{

    // Separate type declarations, function declarations, and expressions
    std::vector<ASTNode *> typeDecls;
    std::vector<ASTNode *> funcDecls;
    std::vector<ASTNode *> exprs;

    for (ASTNode *node : root)
    {
        if (auto *typeDecl = dynamic_cast<TypeDeclaration *>(node))
        {
            typeDecls.push_back(node);
        }
        else if (auto *fn = dynamic_cast<FuncDeclaration *>(node))
        {
            funcDecls.push_back(node);
        }
        else
        {
            exprs.push_back(node);
        }
    }

    // Process type declarations first
    PushFunc(); // Global function registry
    IRGenerator generator(*this);
    for (ASTNode *node : typeDecls)
    {
        node->accept(generator);
    }

    // Then process function declarations
    for (ASTNode *node : funcDecls)
    {
        if (auto *fn = dynamic_cast<FuncDeclaration *>(node))
        {
            addFuncDecl(fn->name, fn);
        }
    }

    // Finally process expressions
    root = std::move(exprs);

    llvm::FunctionType *printfType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(context),
        {llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0)},
        true);
    llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", &module);

    llvm::FunctionType *putsType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(context),
        {llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0)},
        false);
    llvm::Function::Create(putsType, llvm::Function::ExternalLinkage, "puts", &module);

    llvm::FunctionType *mainType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(context), false);
    llvm::Function *mainFunc = llvm::Function::Create(
        mainType, llvm::Function::ExternalLinkage, "main", module);
    llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", mainFunc);
    builder.SetInsertPoint(entry);

    // Generate code for all remaining nodes
    for (ASTNode *node : root)
    {
        node->accept(generator);
    }

    for (llvm::Value *val : valueStack)
    {
        if (val->getType()->isDoubleTy())
        {
            llvm::Value *format = builder.CreateGlobalStringPtr("%g\n");
            builder.CreateCall(module.getFunction("printf"), {format, val});
        }
        else if (val->getType()->isIntegerTy(1))
        {
            llvm::Value *str = builder.CreateSelect(
                val,
                builder.CreateGlobalStringPtr("true\n"),
                builder.CreateGlobalStringPtr("false\n"));
            builder.CreateCall(module.getFunction("puts"), {str});
        }
        else if (val->getType()->isPointerTy())
        {
            builder.CreateCall(module.getFunction("puts"), {val});
        }
    }

    valueStack.clear();

    builder.CreateRet(llvm::ConstantInt::get(context, llvm::APInt(32, 0)));
    llvm::verifyFunction(*mainFunc);
}

void Context::WriteDownCode(const std::string &filename)
{
    std::error_code EC;
    llvm::raw_fd_ostream out(filename, EC);

    if (EC)
    {
        std::cerr << "[ERROR-IRCODE]: " << EC.message() << std::endl;
        return;
    }

    module.print(out, nullptr);
    out.flush();
    std::cout << "[IRCODE] Representacion intermedia: " << filename << std::endl;
}
