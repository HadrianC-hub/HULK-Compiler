#include <cstdio>
#include <iostream>
#include "../ast/AST.hpp"
#include "Context.hpp"
#include "LLVM.hpp"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"

CodeGenContext::CodeGenContext()
    : builder(context), module("hulk_module", context) {}

void CodeGenContext::generateCode(std::vector<ASTNode *> &root)
{

    pushFuncScope();

    std::vector<ASTNode *> exprs;
    for (ASTNode *node : root)
    {
        if (auto *fn = dynamic_cast<FunctionDeclarationNode *>(node))
        {
            addFuncDecl(fn->name, fn);
        }
        else
        {
            exprs.push_back(node);
        }
    }
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

    LLVMGenerator generator(*this);
    for (ASTNode *node : root)
    {
        node->accept(generator);
    }

    for (llvm::Value *val : valueStack)
    {
        if (val->getType()->isDoubleTy())
        { // Number
            llvm::Value *format = builder.CreateGlobalStringPtr("%g\n");
            builder.CreateCall(module.getFunction("printf"), {format, val});
        }
        else if (val->getType()->isIntegerTy(1))
        { // Boolean
            llvm::Value *str = builder.CreateSelect(
                val,
                builder.CreateGlobalStringPtr("true\n"),
                builder.CreateGlobalStringPtr("false\n"));
            builder.CreateCall(module.getFunction("puts"), {str});
        }
        else if (val->getType()->isPointerTy())
        { // String
            builder.CreateCall(module.getFunction("puts"), {val});
        }
    }

    valueStack.clear();

    builder.CreateRet(llvm::ConstantInt::get(context, llvm::APInt(32, 0)));
    llvm::verifyFunction(*mainFunc);
}

void CodeGenContext::dumpIR(const std::string &filename)
{
    std::error_code EC;
    llvm::raw_fd_ostream out(filename, EC);

    module.print(out, nullptr);
    out.flush();
}
