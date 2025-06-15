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
        throw std::runtime_error("Funcion Built-In no soportada: " + name);
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
        throw std::runtime_error("[ERROR] El bloque debe contener al menos una expresion: (line " + std::to_string(node.line()) + ")");
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
        throw std::runtime_error("[ERROR] El bloque no tiene valor retornable en su ultima expresion: (line " + std::to_string(node.line()) + ")");
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
                                     "' en linea " + std::to_string(node.line()));
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
        throw std::runtime_error("[ERROR] No hay argumentos suficientes para la funcion: " + node.name + "");
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
        throw std::runtime_error("[ERROR] Funcion no declarada: " + node.funcName);
    }

    decl->accept(*this);
}

void IRGenerator::visit(LetExpression &node)
{
    // Inicializar nuevo scope de variables con herencia
    context.PushVar(true);

    // Procesar declaraciones
    for (const LetDeclaration &decl : *node.declarations)
    {
        // Push the variable name and type onto the placeholder stack before processing its value
        context.typeSystem.pushPlaceholder(decl.name, "var");
        std::cout << "  📝 Added placeholder: " << decl.name << " of type var" << std::endl;

        // Procesar el inicializador
        decl.initializer->accept(*this);
        llvm::Value *initValue = context.valueStack.back();
        context.valueStack.pop_back();

        // Agregar variables al scope
        context.addLocal(decl.name, initValue);

        // Pop the placeholder after processingAdd commentMore actions
        context.typeSystem.popPlaceholder();
    }

    // Procesar el cuerpo
    node.body->accept(*this);

    // Limpiar el scope
    context.PopVar();
}

void IRGenerator::visit(Assignment &node)
{
    // Procesar los valores a la derecha primero
    node.rhs->accept(*this);
    llvm::Value *newValue = context.valueStack.back();
    context.valueStack.pop_back();

    // Obtener nombre de variable a la derecha
    if (auto *idNode = dynamic_cast<VarFuncName *>(node.name))
    {
        const std::string &varName = idNode->name;
        bool found = false;

        // Buscar a traves de los scope (desde adentro hacia afuera)
        for (auto it = context.localScopes.rbegin(); it != context.localScopes.rend(); ++it)
        {
            auto foundVar = it->find(varName);
            if (foundVar != it->end())
            {
                // Encontrada la variable, actualizando su valor
                foundVar->second = newValue;
                found = true;
            }
            else if (found)
            {
                // Si antes la encontramos pero ahora no encontramos la variable, deja de buscar
                break;
            }
        }

        if (!found)
        {
            throw std::runtime_error("[ERROR] Variable indefinida '" + varName +
                                     "' en linea: " + std::to_string(node.line()));
        }

        // 5. Push the assigned value onto the stack
        context.valueStack.push_back(newValue);
        std::cout << "[CHECK] Variable '" << varName << "' asignada en scopes relevantes - Tamaño de pila: " << context.valueStack.size() << std::endl;
    }
    else
    {
        throw std::runtime_error("[ERROR] La parte izquerda debe ser un asignador. Linea: " +
                                 std::to_string(node.line()));
    }
}

void IRGenerator::visit(IfExpression &node)
{
    // Procesar cada rama if y elif
    for (const IfBranch &branch : *node.branches)
    {
        // Condicion de evaluacion
        branch.condition->accept(*this);
        llvm::Value *condition = context.valueStack.back();
        context.valueStack.pop_back();

        // Comparacion directa con true (1)
        if (auto *constInt = llvm::dyn_cast<llvm::ConstantInt>(condition))
        {
            if (constInt->getZExtValue() == 1)
            {
                // Condicion true, evaluar cuerpo
                branch.body->accept(*this);
                return;
            }
        }
    }

    // Si llegamos aqui, ninguna condicion fue true, evaluando cuerpo de else
    if (node.elseBody)
    {
        node.elseBody->accept(*this);
    }

    std::cout << "[CHECK] Expresion IF completada - Tamaño de pila: " << context.valueStack.size() << std::endl;
}

void IRGenerator::visit(WhileLoop &node)
{
    // Crear un nuevo scope para variables
    context.PushVar(true);

    // Crear un vector para almacenar valores del cuerpo del ciclo
    std::vector<llvm::Value *> loopBodyValues;

    int iteration = 0;
    // Comenzar ciclo
    while (true)
    {
        // Evaluar condicion
        node.condition->accept(*this);
        llvm::Value *condition = context.valueStack.back();
        context.valueStack.pop_back();

        // Comparacion directa con true (1)
        if (auto *constInt = llvm::dyn_cast<llvm::ConstantInt>(condition))
        {
            if (constInt->getZExtValue() == 1)
            {
                // Condicion es true, evaluar cuerpo
                std::cout << "Iteracion " << ++iteration << std::endl;
                node.body->accept(*this);

                // Si el cuerpo produjo un valor, almacenarlo
                if (!context.valueStack.empty())
                {
                    llvm::Value *bodyValue = context.valueStack.back();
                    context.valueStack.pop_back(); // Remover del stack global
                    loopBodyValues.push_back(bodyValue);
                }
            }
            else
            {
                // Condicion es falsa, salir del loop
                break;
            }
        }
        else
        {
            // Si no es constante, convertir en booleano y comprobar
            condition = context.builder.CreateFCmpONE(
                condition,
                llvm::ConstantFP::get(context.context, llvm::APFloat(0.0)),
                "whilecond");

            if (auto *boolVal = llvm::dyn_cast<llvm::ConstantInt>(condition))
            {
                if (boolVal->getZExtValue() == 1)
                {
                    // Condicion es true, evaluar cuerpo
                    std::cout << "Iteracion " << ++iteration << std::endl;
                    node.body->accept(*this);

                    // Si el cuerpo produjo un valor, almacenarlo
                    if (!context.valueStack.empty())
                    {
                        llvm::Value *bodyValue = context.valueStack.back();
                        context.valueStack.pop_back(); // Remover del stack global
                        loopBodyValues.push_back(bodyValue);
                    }
                }
                else
                {
                    // Condicion es falsa, salir del loop
                    break;
                }
            }
        }
    }

    std::cout << "[CHECK] Ciclo WHILE terminado despues de " << iteration << " iteraciones" << std::endl;

    // Despues del ciclo, enviar el ultimo valor del cuerpo a la stack global (si hay algun valor devuelto)
    // NOTA: ARREGLAR VALOR DEVUELTO
    if (!loopBodyValues.empty())
    {
        context.valueStack.push_back(loopBodyValues.back());
    }

    // Limpiar scope
    context.PopVar();

    std::cout << "[CHECK] Ciclo while completado - Tamaño de pila: " << context.valueStack.size() << std::endl;
}

void IRGenerator::visit(ForLoop &node)
{
    // Inicializar scope de variables con propiedad de heredar
    context.PushVar(true);

    // Evaluar expresiones de iniciar y terminar rango
    node.init_range->accept(*this);
    llvm::Value *initValue = context.valueStack.back();
    context.valueStack.pop_back();

    node.end_range->accept(*this);
    llvm::Value *endValue = context.valueStack.back();
    context.valueStack.pop_back();

    // Agregar la variable con el valor inicial al scope
    context.addLocal(node.varName, initValue);

    // Vector para almacenar valores del cuerpo del ciclo
    std::vector<llvm::Value *> loopBodyValues;

    int iteration = 0;
    // Inicializar ciclo
    while (true)
    {
        // Obtener valor actual de la variable del ciclo for
        llvm::Value *currentValue = context.lookupLocal(node.varName);

        // Comparacion de la variable con el valor final
        llvm::Value *condition = context.builder.CreateFCmpULT(
            currentValue,
            endValue,
            "forcond");

        // Caso en que se deba continuar el ciclo
        if (auto *constInt = llvm::dyn_cast<llvm::ConstantInt>(condition))
        {
            if (constInt->getZExtValue() == 1)
            {
                // Evaluar el cuerpo del ciclo para esta iteracion
                node.body->accept(*this);

                // Almacenar valor del cuerpo si existe
                if (!context.valueStack.empty())
                {
                    llvm::Value *bodyValue = context.valueStack.back();

                    // Verificar caso en que se deba imprimir
                    bool isPrint = false;
                    if (auto *builtin = dynamic_cast<BuiltInFunc *>(node.body))
                    {
                        isPrint = (builtin->name == "print");
                    }

                    if (isPrint)
                    {
                        // Para caso de imprimir, mantener el valor en la stack global y ademas meter en la stack local
                        loopBodyValues.push_back(bodyValue);
                    }
                    else
                    {
                        // Para caso de no imprimir, remover el valor de la stack global y almacenar en la stack local
                        context.valueStack.pop_back();
                        loopBodyValues.push_back(bodyValue);
                    }
                }

                // Incrementar variable de ciclo
                llvm::Value *one = llvm::ConstantFP::get(context.context, llvm::APFloat(1.0));
                llvm::Value *nextValue = context.builder.CreateFAdd(currentValue, one, "nextval");
                context.addLocal(node.varName, nextValue);
            }
            else
            {
                // Condicional falsa, salir del ciclo
                break;
            }
        }
        else
        {
            // Si no es constante, convertir a booleana y comprobar
            condition = context.builder.CreateFCmpONE(
                condition,
                llvm::ConstantFP::get(context.context, llvm::APFloat(0.0)),
                "forcond");

            if (auto *boolVal = llvm::dyn_cast<llvm::ConstantInt>(condition))
            {
                if (boolVal->getZExtValue() == 1)
                {
                    // Evaluar cuerpo para esta iteracion
                    std::cout << "Iteracion " << ++iteration << std::endl;
                    node.body->accept(*this);

                    // Almacenar valor del cuerpo si existe
                    if (!context.valueStack.empty())
                    {
                        llvm::Value *bodyValue = context.valueStack.back();

                        // Verificar caso en que se deba imprimir
                        bool isPrint = false;
                        if (auto *builtin = dynamic_cast<BuiltInFunc *>(node.body))
                        {
                            isPrint = (builtin->name == "print");
                        }

                        if (isPrint)
                        {
                            // Para caso de imprimir, mantener el valor en la stack global y ademas meter en la stack local
                            loopBodyValues.push_back(bodyValue);
                        }
                        else
                        {
                            // Para caso de no imprimir, remover el valor de la stack global y almacenar en la stack local
                            context.valueStack.pop_back();
                            loopBodyValues.push_back(bodyValue);
                        }
                    }

                    // Incrementar variable de ciclo
                    llvm::Value *one = llvm::ConstantFP::get(context.context, llvm::APFloat(1.0));
                    llvm::Value *nextValue = context.builder.CreateFAdd(currentValue, one, "nextval");
                    context.addLocal(node.varName, nextValue);
                }
                else
                {
                    // Condicional falsa, salir del ciclo
                    break;
                }
            }
        }
    }

    std::cout << "[CHECK] Ciclo FOR terminado despues de " << iteration << " iteraciones" << std::endl;

    // 7. Push the last body value to global stack if any
    if (!loopBodyValues.empty())
    {
        context.valueStack.push_back(loopBodyValues.back());
    }

    // Clean up loop scope
    context.PopVar();

    std::cout << "[CHECK] Ciclo for terminado - Tamaño de pila: " << context.valueStack.size() << std::endl;
}

void IRGenerator::visit(TypeDeclaration &node)
{
    std::cout << "🔍 TypeDeclaration: " << node.name << std::endl;

    // Register the type
    auto &typeDef = context.typeSystem.registerType(node.name, node.baseType);

    // Set constructor parameters and base argsAdd commentMore actions
    typeDef.constructorParams.clear();
    if (node.constructorParams)
    {
        for (const auto &param : *node.constructorParams)
        {
            typeDef.constructorParams.push_back(param.name);
        }
    }
    typeDef.baseArgs = node.baseArgs;

    // Set current type for processing attributes and methods
    context.typeSystem.setCurrentType(node.name);

    // Process attributes
    if (node.body->attributes)
    {
        for (const auto &attr : *node.body->attributes)
        {
            context.typeSystem.addAttribute(attr.name, node.name, attr.initializer);
            std::cout << "  📝 Added attribute: " << attr.name << std::endl;
        }
    }

    // Process methods
    if (node.body->methods)
    {
        for (const auto &method : *node.body->methods)
        {
            context.typeSystem.addMethod(node.name, method.name, method.params, method.body, method.returnType);
            std::cout << "  📝 Added method: " << method.name << std::endl;
        }
    }

    std::cout << "✅ Type " << node.name << " processed" << std::endl;
}

void IRGenerator::visit(InitInstance &node)
{
    std::cout << "🔍 NewInstance: " << node.typeName << std::endl;

    // Check if we're inside a let declaration
    std::string varName = context.typeSystem.getCurrentPlaceholder().name;
    if (!varName.empty())
    {
        std::cout << "  📝 Using placeholder variable: " << varName << std::endl;
    }

    // 1. Initialize instance variables map and set current type
    std::map<std::pair<std::string, std::string>, llvm::Value *> instanceVars;
    std::string currType = node.typeName;
    context.typeSystem.setCurrentType(currType);

    // 2. Start a new variable scope without inheritance
    context.PushVar(false);

    // Process constructor arguments first
    std::vector<llvm::Value *> args;
    for (ASTNode *arg : node.args)
    {
        arg->accept(*this);
        args.push_back(context.valueStack.back());
        context.valueStack.pop_back();
    }

    // Process type hierarchy
    while (!currType.empty())
    {
        std::cout << "  🔄 Processing type: " << currType << std::endl;

        // Get type information
        const auto &typeConst = context.typeSystem.getConstructorParams(currType);
        const auto &fatherArgs = context.typeSystem.getBaseArgs(currType);
        const auto &attrType = context.typeSystem.getAttributes(currType);
        const auto &father = context.typeSystem.getParentType(currType);

        // Process constructor parameters
        if (!typeConst.empty())
        {
            for (size_t i = 0; i < typeConst.size() && i < args.size(); ++i)
            {
                context.addLocal(typeConst[i], args[i]);
                std::cout << "    📝 Added constructor param: " << typeConst[i] << std::endl;
            }
            // Remove processed args
            args.erase(args.begin(), args.begin() + std::min(typeConst.size(), args.size()));
        }

        // Process parent constructor parameters
        if (father)
        {
            const auto &fatherConst = context.typeSystem.getConstructorParams(*father);

            // Process base args first
            if (!fatherArgs.empty())
            {
                for (size_t i = 0; i < fatherArgs.size() && i < fatherConst.size(); ++i)
                {
                    fatherArgs[i]->accept(*this);
                    context.addLocal(fatherConst[i], context.valueStack.back());
                    context.valueStack.pop_back();
                    std::cout << "    📝 Added base arg: " << fatherConst[i] << std::endl;
                }
            }

            // Process remaining args with remaining parent constructor params
            size_t startIdx = fatherArgs.size();
            for (size_t i = 0; i < fatherConst.size() - startIdx && i < args.size(); ++i)
            {
                context.addLocal(fatherConst[startIdx + i], args[i]);
                std::cout << "    📝 Added remaining parent param: " << fatherConst[startIdx + i] << std::endl;
            }
            // Remove processed args
            args.erase(args.begin(), args.begin() + std::min(fatherConst.size() - startIdx, args.size()));
        }

        // Process attributes
        for (const auto &[attrName, attr] : attrType)
        {
            if (attr.initializer)
            {
                attr.initializer->accept(*this);
                instanceVars[{attrName, attr.TypeName}] = context.valueStack.back();
                context.valueStack.pop_back();
                std::cout << "    📝 Added attribute: " << attrName << " of type " << attr.TypeName << std::endl;
            }
        }

        // Move to parent type
        if (father)
        {
            currType = *father;
            context.typeSystem.setCurrentType(currType);
        }
    }

    // Clean up
    context.PopVar();
    context.typeSystem.setCurrentType("");

    // Create the instance with its variablesAdd commentMore actions
    context.typeSystem.createInstance(varName, node.typeName, instanceVars);

    std::cout << "✅ Instance created: " << varName << std::endl;
}

void IRGenerator::visit(MethodCall &node)
{
    std::cout << "🔍 MethodCall: " << node.instanceName << "." << node.methodName << std::endl;

    // 1. Get instance type and set currentType
    std::string typeName = context.typeSystem.getInstanceType(node.instanceName);
    if (typeName.empty())
    {
        throw std::runtime_error("❌ Instance '" + node.instanceName + "' not found at line " + std::to_string(node.line()));
    }

    context.typeSystem.setCurrentType(typeName);

    // 2. Push instance variables onto the stackAdd commentMore actions
    try
    {
        context.typeSystem.pushCurrentInstanceVars(context.typeSystem.getInstanceVars(node.instanceName));
    }
    catch (const std::runtime_error &e)
    {
        throw std::runtime_error("❌ " + std::string(e.what()) + " at line " + std::to_string(node.line()));
    }

    // 3. Create new Scope without inheritance
    context.PushVar(false);

    // 4. Set method name in placeholderStack
    context.typeSystem.pushPlaceholder(node.methodName, "method");

    // 5. Find method in type hierarchy
    TypeMethod *method = nullptr;
    std::string currType = typeName;
    while (!currType.empty() && !method)
    {
        method = context.typeSystem.findMethod(currType, node.methodName);
        if (!method)
        {
            currType = context.typeSystem.getParentType(currType).value_or("");
        }
    }

    if (!method)
    {
        throw std::runtime_error("❌ Method '" + node.methodName + "' not found in type hierarchy starting from '" +
                                 typeName + "' at line " + std::to_string(node.line()));
    }

    // 6. Process method arguments and parametersAdd commentMore actions
    std::vector<llvm::Value *> args;
    for (ASTNode *arg : node.args)
    {
        arg->accept(*this);
        args.push_back(context.valueStack.back());
        context.valueStack.pop_back();
    }

    // Associate parameters with argument values
    if (method->params)
    {
        for (size_t i = 0; i < method->params->size() && i < args.size(); ++i)
        {
            context.addLocal((*method->params)[i].name, args[i]);
            std::cout << "  📝 Bound param " << (*method->params)[i].name << std::endl;
        }
    }

    // 7. Evaluate method body
    method->body->accept(*this);

    // 8. Clean up
    context.typeSystem.setCurrentType("");
    context.typeSystem.popPlaceholder();
    context.PopVar();
    context.typeSystem.popCurrentInstanceVars();

    std::cout << "✅ Method call processed" << std::endl;
}

void IRGenerator::visit(SelfCall &node)
{
    std::cout << "🔍 SelfCall: " << node.varName << std::endl;

    // Get current type
    std::string currentType = context.typeSystem.getCurrentType();
    if (currentType.empty())
    {
        throw std::runtime_error("❌ 'self' access outside of type context at line " + std::to_string(node.line()));
    }

    // Handle self variables
    llvm::Value *val = context.typeSystem.getCurrentInstanceVar(node.varName, currentType);

    if (!val)
    {
        throw std::runtime_error("❌ Undefined variable '" + node.varName +
                                 "' in type '" + currentType +
                                 "' at line " + std::to_string(node.line()));
    }

    context.valueStack.push_back(val);

    std::cout << "✅ Self access processed" << std::endl;
}