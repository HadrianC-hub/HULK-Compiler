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
    llvm::LLVMContext context; // Global LLVM context (types, constants, etc.)
    llvm::IRBuilder<> builder; // Builder for creating IR instructions
    llvm::Module module;       // LLVM IR module (a single compilation unit)

    std::map<std::string, llvm::Value *> locals;
    std::vector<llvm::Value *> valueStack;

    CodeGenContext();

    void generateCode(std::vector<ASTNode *> &root);
}