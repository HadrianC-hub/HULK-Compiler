#include "IRGen.hpp"
#include "IRContext.hpp"
#include "../ast/AST.hpp"
#include "../Utils/hulk_utils.hpp"
#include <stdexcept>
#include <iostream>
#include <string>
#include <cstring>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.7182818284590452354
#endif

IRGenerator::IRGenerator(Context &ctx)
    : context(ctx) {}

std::string processRawString(const std::string &raw)
{
    std::string processed;
    bool escape = false;
    size_t start = (raw.front() == '"') ? 1 : 0;
    size_t end = (raw.back() == '"') ? raw.size() - 1 : raw.size();

    for (size_t i = start; i <= end; ++i)
    {
        char c = raw[i];
        if (escape)
        {
            switch (c)
            {
            case 'n':
                processed += '\n';
                break;
            case 't':
                processed += '\t';
                break;
            case '"':
                processed += '"';
                break;
            case '\\':
                processed += '\\';
                break;
            default:
                processed += c;
            }
            escape = false;
        }
        else if (c == '\\')
        {
            escape = true;
        }
        else
        {
            processed += c;
        }
    }

    if (escape)
    {
        processed += '\\';
    }

    return processed;
}

void IRGenerator::visit(DataType &node)
{
    llvm::Value *val = nullptr;

    if (node._type == "Number")
    {
        double numVal = std::stod(node.value);
        val = llvm::ConstantFP::get(context.context, llvm::APFloat(numVal));
    }
    else if (node._type == "Boolean")
    {
        bool b = (node.value == "true");
        val = llvm::ConstantInt::get(llvm::Type::getInt1Ty(context.context), b);
    }
    else if (node._type == "String")
    {
        std::string processed = processRawString(node.value);
        val = context.builder.CreateGlobalStringPtr(processed);
    }
    else if (node._type == "Null")
    {
        val = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(context.context), 0));
    }
    else
    {
        throw std::runtime_error("Tipo no soportado: " + node._type);
    }

    context.valueStack.push_back(val);

    std::cout << "[EMITIDO] " << node._type << std::endl;
}

void IRGenerator::visit(BinaryOperation &node)
{
    node.left->accept(*this);
    llvm::Value *left = context.valueStack.back();
    context.valueStack.pop_back();
    node.right->accept(*this);
    llvm::Value *right = context.valueStack.back();
    context.valueStack.pop_back();

    llvm::Value *result = nullptr;
    llvm::IRBuilder<> &builder = context.builder;

    const std::string &op = node.op;

    if (op == "+")
    {
        result = builder.CreateFAdd(left, right, "addtmp");
    }
    else if (op == "-")
    {
        result = builder.CreateFSub(left, right, "subtmp");
    }
    else if (op == "*")
    {
        result = builder.CreateFMul(left, right, "multmp");
    }
    else if (op == "/")
    {
        result = builder.CreateFDiv(left, right, "divtmp");
    }
    else if (op == "%")
    {
        result = builder.CreateFRem(left, right, "modtmp");
    }
    else if (op == "^")
    {
        llvm::Function *powFn = context.module.getFunction("llvm.pow.f64");
        if (!powFn)
        {
            llvm::FunctionType *powType = llvm::FunctionType::get(
                llvm::Type::getDoubleTy(context.context),
                {llvm::Type::getDoubleTy(context.context), llvm::Type::getDoubleTy(context.context)},
                false);
            powFn = llvm::Function::Create(powType, llvm::Function::ExternalLinkage, "llvm.pow.f64", context.module);
        }
        result = builder.CreateCall(powFn, {left, right}, "powtmp");
    }

    else if (op == "<")
    {
        result = builder.CreateFCmpULT(left, right, "lttmp");
    }
    else if (op == "<=")
    {
        result = builder.CreateFCmpULE(left, right, "letmp");
    }
    else if (op == ">")
    {
        result = builder.CreateFCmpUGT(left, right, "gttmp");
    }
    else if (op == ">=")
    {
        result = builder.CreateFCmpUGE(left, right, "getmp");
    }
    else if (op == "==" || op == "!=")
    {
        const std::string &nodeType = node.left->type();

        if (nodeType == "Number")
        {
            result = (op == "==")
                         ? builder.CreateFCmpUEQ(left, right, "eqtmp")
                         : builder.CreateFCmpUNE(left, right, "netmp");
        }
        else if (nodeType == "Boolean")
        {
            result = (op == "==")
                         ? builder.CreateICmpEQ(left, right, "beqtmp")
                         : builder.CreateICmpNE(left, right, "bnetmp");
        }
        else if (nodeType == "String")
        {
            llvm::Function *eqFn = context.module.getFunction("hulk_str_equals");
            if (!eqFn)
            {
                llvm::FunctionType *eqType = llvm::FunctionType::get(
                    llvm::Type::getInt1Ty(context.context),
                    {llvm::PointerType::get(llvm::Type::getInt8Ty(context.context), 0),
                     llvm::PointerType::get(llvm::Type::getInt8Ty(context.context), 0)},
                    false);
                eqFn = llvm::Function::Create(
                    eqType, llvm::Function::ExternalLinkage,
                    "hulk_str_equals", context.module);
            }

            llvm::Value *eqCall = builder.CreateCall(eqFn, {left, right}, "strequal");

            result = (op == "!=")
                         ? builder.CreateNot(eqCall, "strnotequal")
                         : eqCall;
        }
        else
        {
            throw std::runtime_error("Tipo no soportado para las operaciones '==' o '!=': " + nodeType);
        }
    }

    else if (op == "&")
    {
        result = builder.CreateAnd(left, right, "andtmp");
    }
    else if (op == "|")
    {
        result = builder.CreateOr(left, right, "ortmp");
    }

    else if (op == "@" || op == "@@")
    {
        const char *funcName = (op == "@") ? "hulk_str_concat" : "hulk_str_concat_space";

        std::cout << "🔍 String concatenation with " << funcName << std::endl;

        // Convertir numeros a string
        llvm::Value *leftStr = left;
        llvm::Value *rightStr = right;

        auto convertToString = [&](llvm::Value *val) -> llvm::Value *
        {
            if (val->getType()->isDoubleTy())
            {
                // Get the sprintf function
                llvm::Function *sprintfFn = context.module.getFunction("sprintf");
                if (!sprintfFn)
                {
                    llvm::FunctionType *sprintfType = llvm::FunctionType::get(
                        llvm::Type::getInt32Ty(context.context),
                        {llvm::PointerType::get(llvm::Type::getInt8Ty(context.context), 0),
                         llvm::PointerType::get(llvm::Type::getInt8Ty(context.context), 0)},
                        true);
                    sprintfFn = llvm::Function::Create(
                        sprintfType,
                        llvm::Function::ExternalLinkage,
                        "sprintf",
                        context.module);
                }

                llvm::Value *buffer = context.builder.CreateAlloca(
                    llvm::Type::getInt8Ty(context.context),
                    llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.context), 32));

                // Formatear string para double
                llvm::Value *formatStr = context.builder.CreateGlobalStringPtr("%g");

                std::vector<llvm::Value *> args = {buffer, formatStr, val};
                context.builder.CreateCall(sprintfFn, args);

                return buffer;
            }
            return val;
        };

        // Convertir operandos si son numeros
        leftStr = convertToString(left);
        rightStr = convertToString(right);

        llvm::Function *concatFn = context.module.getFunction(funcName);
        if (!concatFn)
        {
            llvm::FunctionType *concatType = llvm::FunctionType::get(
                llvm::PointerType::get(llvm::Type::getInt8Ty(context.context), 0),
                {llvm::PointerType::get(llvm::Type::getInt8Ty(context.context), 0),
                 llvm::PointerType::get(llvm::Type::getInt8Ty(context.context), 0)},
                false);
            concatFn = llvm::Function::Create(concatType, llvm::Function::ExternalLinkage, funcName, context.module);
        }

        result = context.builder.CreateCall(concatFn, {leftStr, rightStr}, "concat");
    }

    else
    {
        throw std::runtime_error("Operador binario no soportado: " + op);
    }

    context.valueStack.push_back(result);
    std::cout << "[EMITIDO]" << op << "operador binario.\n";
}

void IRGenerator::visit(UnaryOperation &node)
{

    node.operand->accept(*this);
    llvm::Value *operand = context.valueStack.back();
    context.valueStack.pop_back();

    llvm::Value *result = nullptr;
    llvm::IRBuilder<> &builder = context.builder;

    const std::string &op = node.op;

    if (op == "-")
    {
        llvm::Value *zero = llvm::ConstantFP::get(context.context, llvm::APFloat(0.0));
        result = builder.CreateFSub(zero, operand, "negtmp");
    }
    else if (op == "!")
    {
        result = builder.CreateNot(operand, "nottmp");
    }
    else
    {
        throw std::runtime_error("Operador unario no soportado: " + op);
    }

    context.valueStack.push_back(result);
    std::cout << "[EMITIDO] " << op << " operador unario.\n";
}

void IRGenerator::visit(BuiltInFunc &node)
{
    llvm::IRBuilder<> &builder = context.builder;
    std::vector<llvm::Value *> args;

    for (ASTNode *arg : node.args)
    {
        arg->accept(*this);
        args.push_back(context.valueStack.back());
        context.valueStack.pop_back();
    }

    llvm::Value *result = nullptr;
    std::string name = node.name;

    auto emit_llvm_unary_math = [&](const std::string &fnName, llvm::Value *arg)
    {
        llvm::Function *fn = context.module.getFunction(fnName);
        if (!fn)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(
                llvm::Type::getDoubleTy(context.context),
                {llvm::Type::getDoubleTy(context.context)},
                false);
            fn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, fnName, context.module);
        }
        return builder.CreateCall(fn, {arg});
    };

    if (name == "print")
    {
        result = args[0];
    }
    else if (name == "sin" || name == "cos" || name == "sqrt" || name == "exp")
    {
        std::string fnName = "llvm." + name + ".f64";
        result = emit_llvm_unary_math(fnName, args[0]);
    }
    else if (name == "min")
    {
        llvm::Function *fn = context.module.getFunction("fmin");
        if (!fn)
        {
            llvm::FunctionType *type = llvm::FunctionType::get(
                llvm::Type::getDoubleTy(context.context),
                {llvm::Type::getDoubleTy(context.context), llvm::Type::getDoubleTy(context.context)},
                false);
            fn = llvm::Function::Create(type, llvm::Function::ExternalLinkage, "fmin", context.module);
        }
        result = builder.CreateCall(fn, {args[0], args[1]}, "mincall");
    }
    else if (name == "max")
    {
        llvm::Function *fn = context.module.getFunction("fmax");
        if (!fn)
        {
            llvm::FunctionType *type = llvm::FunctionType::get(
                llvm::Type::getDoubleTy(context.context),
                {llvm::Type::getDoubleTy(context.context), llvm::Type::getDoubleTy(context.context)},
                false);
            fn = llvm::Function::Create(type, llvm::Function::ExternalLinkage, "fmax", context.module);
        }
        result = builder.CreateCall(fn, {args[0], args[1]}, "maxcall");
    }
    else if (name == "log")
    {
        llvm::Function *logFn = context.module.getFunction("hulk_log_base_checked");
        if (!logFn)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(
                llvm::Type::getDoubleTy(context.context),
                {llvm::Type::getDoubleTy(context.context), llvm::Type::getDoubleTy(context.context)},
                false);
            logFn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "hulk_log_base_checked", context.module);
        }
        result = builder.CreateCall(logFn, {args[0], args[1]}, "logcall");
    }
    else if (name == "rand")
    {
        llvm::Function *randFn = context.module.getFunction("hulk_rand");
        if (!randFn)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(
                llvm::Type::getDoubleTy(context.context), false);
            randFn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "hulk_rand", context.module);
        }
        result = builder.CreateCall(randFn, {}, "randcall");
    }
    else
    {
        throw std::runtime_error("Función Built-In no soportada: " + name);
    }

    if (result)
    {
        context.valueStack.push_back(result);
    }

    std::cout << "[EMITIDO] " << name << "' Built-In.\n";
}

void IRGenerator::visit(Block &node)
{
    if (node.expressions.empty())
    {
        throw std::runtime_error("[ERROR] El bloque debe contener al menos una expresión: (line " + std::to_string(node.line()) + ")");
    }

    std::cout << "🔍 BlockNode - Stack size before: " << context.valueStack.size() << std::endl;
    context.PushFunc();

    std::vector<ASTNode *> bodyExprs;
    for (ASTNode *expr : node.expressions)
    {
        if (auto *decl = dynamic_cast<FuncDeclaration *>(expr))
        {
            context.addFuncDecl(decl->name, decl);
            std::cout << "  📝 Registered function: " << decl->name << std::endl;
        }
        else
        {
            bodyExprs.push_back(expr);
        }
    }

    llvm::Value *lastValidResult = nullptr;

    for (size_t i = 0; i < bodyExprs.size(); ++i)
    {
        ASTNode *expr = bodyExprs[i];
        std::cout << "  🔄 Evaluating expression " << i + 1 << "/" << bodyExprs.size() << " - Stack size: " << context.valueStack.size() << std::endl;
        expr->accept(*this);

        bool isPrint = false;
        if (auto *builtin = dynamic_cast<BuiltInFunc *>(expr))
        {
            isPrint = (builtin->name == "print");
        }

        if (!context.valueStack.empty())
        {
            llvm::Value *val = context.valueStack.back();

            if (i == bodyExprs.size() - 1)
            {
                if (isPrint)
                {
                    context.valueStack.push_back(val);
                    lastValidResult = val;
                }
                else
                {
                    lastValidResult = val;
                }
            }
            else
            {
                if (!isPrint)
                {
                    context.valueStack.pop_back();
                }
                else
                {
                    lastValidResult = val;
                }
            }
        }
    }

    context.PopFunc();

    if (!lastValidResult)
    {
        throw std::runtime_error("[ERROR] El bloque no tiene valor retornable en su última expresión: (line " + std::to_string(node.line()) + ")");
    }

    std::cout << "[EMITIDO] Bloque completado - Tamaño final: " << context.valueStack.size() << "\n";
}

void IRGenerator::visit(VarFuncName &node)
{
    llvm::Value *val = context.lookupLocal(node.name);

    if (node.name == "PI" || node.name == "pi")
    {
        llvm::Value *pi = llvm::ConstantFP::get(context.context, llvm::APFloat(M_PI));
        context.valueStack.push_back(pi);
        std::cout << "Constante PI emitida\n";
        return;
    }
    else if (node.name == "E" || node.name == "e")
    {
        llvm::Value *e = llvm::ConstantFP::get(context.context, llvm::APFloat(M_E));
        context.valueStack.push_back(e);
        std::cout << "Constante E emitida\n";
        return;
    }
    else
    {
        val = context.lookupLocal(node.name);
        if (!val)
        {
            throw std::runtime_error("[ERROR] Variable indefinida '" + node.name +
                                     "' en línea " + std::to_string(node.line()));
        }
    }

    context.valueStack.push_back(val);

    std::cout << "[EMITIDO] " << node.name << " resuelto y enviado a la pila\n";
}

void IRGenerator::visit(FuncDeclaration &node)
{
    context.PushVar(false); // Crea un scope aislado para parametros de funcion

    const auto &params = *node.params;
    if (params.size() > context.valueStack.size())
    {
        throw std::runtime_error("[ERROR] No hay argumentos suficientes para la función: " + node.name + "");
    }

    for (int i = params.size() - 1; i >= 0; --i)
    {
        llvm::Value *val = context.valueStack.back();
        context.valueStack.pop_back();
        context.addLocal(params[i].name, val);
    }

    node.body->accept(*this);

    if (context.valueStack.empty())
    {
        std::cout << "[NOTA] No hay valor de retorno, devolviendo null" << std::endl;
        context.valueStack.push_back(
            llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(context.context), 0)));
    }

    context.PopVar();
}

void IRGenerator::visit(FuncCall &node)
{
    for (ASTNode *arg : node.args)
    {
        arg->accept(*this);
    }

    auto *decl = context.lookupFuncDecl(node.funcName);
    if (!decl)
    {
        throw std::runtime_error("[ERROR] Función no declarada: " + node.funcName);
    }

    decl->accept(*this);
}

void IRGenerator::visit(LetExpression &node)
{
    std::cout << "🔍 LetNode - Stack size before: " << context.valueStack.size() << std::endl;

    // 1. Initialize new variable scope with inheritance
    context.PushVar(true); // true for inheritance

    // 2. Process each declaration
    for (const LetDeclaration &decl : *node.declarations)
    {
        // Process the initializer first
        decl.initializer->accept(*this);
        llvm::Value *initValue = context.valueStack.back();
        context.valueStack.pop_back();

        // Add the variable to the current scope
        context.addLocal(decl.name, initValue);
        std::cout << "  📝 Added variable '" << decl.name << "' to scope" << std::endl;
    }

    // 3. Process the body
    std::cout << "  🔄 Processing let body - Stack size: " << context.valueStack.size() << std::endl;
    node.body->accept(*this);
    std::cout << "  📤 Body processed - Stack size: " << context.valueStack.size() << std::endl;

    // 4. Keep the body's return value on the stack
    // The value is already on the stack from the body processing

    // Clean up the scope
    context.PopVar();
    std::cout << "  ✅ LetNode completed - Final stack size: " << context.valueStack.size() << std::endl;
}