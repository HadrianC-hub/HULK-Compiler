#include <cstdio>
#include <iostream>
#include "../ast/AST.hpp"
#include "Context.hpp"
#include "LLVM.hpp"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"

CodeGenContext::CodeGenContext()
    : builder(context), module("hulk_module", context) {}