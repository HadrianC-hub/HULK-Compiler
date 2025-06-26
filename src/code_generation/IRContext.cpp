#include <cstdio>
#include <set>
#include <string>
#include <stack>
#include <iostream>
#include "../ast/AST.hpp"
#include "IRContext.hpp"
#include "IRGen.hpp"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"

Context::Context()
    : builder(context), module("hulk_module", context) {}

// Función para generar el código de la representación intermedia
void Context::Generate(std::vector<ASTNode *> &root)
{

    // Separa declaraciones de tipos, declaraciones de funciones y expresiones
    std::vector<ASTNode *> typeDecls;
    std::vector<ASTNode *> funcDecls;
    std::vector<ASTNode *> exprs;

    // Por cada nodo del AST, si es una de las 3 anteriores, se guardan
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

    // Procesando primero declaraciones de tipos
    PushFunc(); // Función de registrar globalmente

    IRGenerator generator(*this);

    buildTypeGraph(typeDecls);
    
    processTypeNodes(generator, typeDecls);

    // for (ASTNode *node : typeDecls)
    // {
    //     node->accept(generator);
    // }

    // Procesando luego declaraciones de funciones
    for (ASTNode *node : funcDecls)
    {
        if (auto *fn = dynamic_cast<FuncDeclaration *>(node))
        {
            addFuncDecl(fn->name, fn);
        }
    }

    // Finalmente procesando expresiones
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

    // Generar el código por cada nodo restante
    for (ASTNode *node : root)
    {
        node->accept(generator);
    }

    for (llvm::Value *val : valueStack)
    {
        // Escribir el código según el tipo de dato
        if (val->getType()->isDoubleTy())
        {
            llvm::Value *format = builder.CreateGlobalStringPtr("%g\n");
            builder.CreateCall(module.getFunction("printf"), {format, val});
        }
        else if (val->getType()->isIntegerTy(1)) // Booleanos
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

void Context::buildTypeGraph(std::vector<ASTNode*>& typeDecls) {
    std::set<std::string> unprocessedTypes;
    for (ASTNode* node : typeDecls) {
        if (auto* typeDecl = dynamic_cast<TypeDeclaration*>(node)) {
            unprocessedTypes.insert(typeDecl->name);
        }
    }
}

void Context::processTypeNodes(IRGenerator& generator, std::vector<ASTNode*>& typeDecls) {
    std::set<std::string> processedTypes;
    std::stack<TypeDeclaration*> typeStack;

    // Primera pasada: procesar tipos sin dependencias
    for (ASTNode* node : typeDecls) {
        if (auto* typeDecl = dynamic_cast<TypeDeclaration*>(node)) {
            if (!typeDecl->baseType || processedTypes.count(*typeDecl->baseType)) {
                typeDecl->accept(generator);
                processedTypes.insert(typeDecl->name);
            } else {
                typeStack.push(typeDecl);
            }
        }
    }

    // Procesar tipos restantes en orden de dependencias
    while (!typeStack.empty()) {
        TypeDeclaration* typeDecl = typeStack.top();
        typeStack.pop();

        if (processedTypes.count(*typeDecl->baseType)) {
            typeDecl->accept(generator);
            processedTypes.insert(typeDecl->name);
        } else {
            // Reintentar más tarde si la dependencia aún no está lista
            typeStack.push(typeDecl);
        }
    }
}

// Funcion para escribir el código de la representación intermedia
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
