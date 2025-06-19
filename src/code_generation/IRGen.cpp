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
    std::cout << "[GENERANDO]: " << node.op << " - T-Pila: " << context.valueStack.size() << std::endl;

    node.left->accept(*this);
    llvm::Value *left = context.valueStack.back();
    context.valueStack.pop_back();

    std::cout << "  - Operando izquierdo - T-Pila: " << context.valueStack.size() << std::endl;

    node.right->accept(*this);
    llvm::Value *right = context.valueStack.back();
    context.valueStack.pop_back();

    std::cout << "  - Operando derecho - T-Pila: " << context.valueStack.size() << std::endl;

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

        std::cout << "- Concatenando STRING con " << funcName << std::endl;

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
    std::cout << "  - Resultado enviado a la pila - T-Pila: " << context.valueStack.size() << std::endl;
    std::cout << "[EMITIDO] Operacion binaria '" << op << "' procesada" << std::endl;
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

    std::cout << "- Funcion Built-In: " << node.name << " - T-Pila: " << context.valueStack.size() << std::endl;

    for (ASTNode *arg : node.args)
    {
        arg->accept(*this);
        args.push_back(context.valueStack.back());
        context.valueStack.pop_back();
        std::cout << "  - Argumento evaluado - T-Pila: " << context.valueStack.size() << std::endl;
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
        std::cout << "  - Procesando funcion PRINT" << std::endl;
        result = args[0];
        std::cout << "  - Valor de PRINT obtenido - T-PILA: " << context.valueStack.size() << std::endl;
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
        std::cout << "  - Resultado enviado a la pila - T-PILA: " << context.valueStack.size() << std::endl;
    }

    std::cout << "[EMITIDO] " << name << "' Built-In.\n";
}

void IRGenerator::visit(Block &node)
{
    if (node.expressions.empty())
    {
        throw std::runtime_error("[ERROR] El bloque debe contener al menos una expresion: (line " + std::to_string(node.line()) + ")");
    }

    std::cout << "- Analizando bloque - T-PILA: " << context.valueStack.size() << std::endl;
    context.PushFunc();

    std::vector<ASTNode *> bodyExprs;
    for (ASTNode *expr : node.expressions)
    {
        if (auto *decl = dynamic_cast<FuncDeclaration *>(expr))
        {
            context.addFuncDecl(decl->name, decl);
            std::cout << "  - Funcion registrada: " << decl->name << std::endl;
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
        std::cout << "  - Evaluando expresion " << i + 1 << "/" << bodyExprs.size() << " - T-PILA: " << context.valueStack.size() << std::endl;
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
                if (!isPrint)
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
            }
        }
    }

    context.PopFunc();

    if (!lastValidResult)
    {
        std::cout << "[ADVERTENCIA] Bloque sin valor final: se usará 0 (line " << node.line() << ")\n";
        lastValidResult = llvm::ConstantInt::get(context.builder.getInt32Ty(), 0);
    }

    std::cout << "[EMITIDO] Bloque completado - T-PILA: " << context.valueStack.size() << "\n";
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
        // Enviar el nombre y tipo de variable al reservado de memoria en pila antes de procesar su valor
        context.typeSystem.push_placeholder(decl.name, "var");
        std::cout << "  - Agregada reservacion: " << decl.name << " de tipo variable" << std::endl;

        // Procesar el inicializador
        decl.initializer->accept(*this);
        if (auto *newInstance = dynamic_cast<InitInstance *>(decl.initializer))
        {
            context.typeSystem.pop_placeholder();
            continue;
        }
        llvm::Value *initValue = context.valueStack.back();
        context.valueStack.pop_back();

        // Agregar variables al scope
        context.addLocal(decl.name, initValue);

        // Pop the placeholder after processing
        context.typeSystem.pop_placeholder();
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
    // else
    // {
    //     // Handle instance variable assignment
    //     std::string varName;

    //     // 1. Get the variable name
    //     if (auto* idNode = dynamic_cast<VarFuncName*>(node.name)) {
    //         varName = idNode->name;
    //     } else if (auto* selfNode = dynamic_cast<SelfCall*>(node.name)) {
    //         varName = selfNode->varName;
    //     } else {
    //         throw std::runtime_error("[ERROR] El miembro izquierdo de la asignación debe ser un asignador o un acceso a self el linea " +
    //                                std::to_string(node.line()));
    //     }

    //     // 2. Look for the variable in currentInstanceVars
    //     if (!context.typeSystem.is_instance_vars_stack_empty()) {
    //         // 3. Change its value for newValue
    //         context.typeSystem.set_current_instance_var(varName, "var", newValue);
    //         // llvm::Value* addr = context.typeSystem.get_current_instance_var(varName, "var");
    //         // if (!addr) {
    //         //     throw std::runtime_error("Variable de instancia '" + varName + "' no encontrada en objeto actual.");
    //         // }

    //         // // Escribe en la posición de memoria del atributo:
    //         // context.builder.CreateStore(newValue, addr);
    //         std::cout << "  [CHECK] Variable de instancia '" << varName << "' asignada - T-PILA: " << context.valueStack.size() << std::endl;
    //     } else {
    //         throw std::runtime_error("[ERROR] No se puede asignar a una variable de instancia fuera del contexto del tipo en linea " +
    //                                std::to_string(node.line()));
    //     }

    //     // Push the assigned value onto the stack
    //     context.valueStack.push_back(newValue);
    // }
    else if (auto *selfNode = dynamic_cast<SelfCall *>(node.name))
    {
        const std::string &varName = selfNode->varName;

        if (!context.typeSystem.is_instance_vars_stack_empty())
        {
            // Obtener el tipo actual y buscar el tipo real del atributo
            std::string currentType = context.typeSystem.get_current_type();
            type_attribute *attr = context.typeSystem.find_attribute(currentType, varName);

            if (!attr)
            {
                throw std::runtime_error("[ERROR] Atributo '" + varName +
                                         "' no encontrado en tipo '" + currentType +
                                         "' en línea " + std::to_string(node.line()));
            }

            // Usar el tipo real del atributo como clave
            context.typeSystem.set_current_instance_var(varName, attr->TypeName, newValue);

            context.valueStack.push_back(newValue);
            std::cout << "  [ASSIGN] Atributo '" << varName << "' (tipo: " << attr->TypeName
                      << ") actualizado" << std::endl;
        }
        else
        {
            throw std::runtime_error("[ERROR] Contexto de instancia no disponible en línea " +
                                     std::to_string(node.line()));
        }
    }
    else
    {
        throw std::runtime_error("[ERROR] Target de asignación inválido en línea " +
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

    std::cout << "[CHECK] Expresion IF completada - T-PILA: " << context.valueStack.size() << std::endl;
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
    // if (!loopBodyValues.empty())
    // {
    //     context.valueStack.push_back(loopBodyValues.back());
    // }

    // Limpiar scope
    context.PopVar();

    std::cout << "[CHECK] Ciclo while completado - T-PILA: " << context.valueStack.size() << std::endl;
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

    // // Enviar el ultimo valor a la stack si existe
    // if (!loopBodyValues.empty())
    // {
    //     context.valueStack.push_back(loopBodyValues.back());
    // }

    // Clean up loop scope
    context.PopVar();

    std::cout << "[CHECK] Ciclo for terminado - T-PILA: " << context.valueStack.size() << std::endl;
}

void IRGenerator::visit(TypeDeclaration &node)
{
    std::cout << "- Declaracion de tipo: " << node.name << std::endl;

    // Comprueba si el tipo ya está registrado
    if (context.typeSystem.type_exists(node.name))
    {
        std::cout << "  [WARNING] Tipo " << node.name << " ya registrado, saltando..." << std::endl;
        return;
    }

    // Registra el tipo
    auto &typeDef = context.typeSystem.reg_type(node.name, node.baseType);

    // Define los parametros del constructor y argumentos de la base
    typeDef.constructorParams.clear();
    if (node.constructorParams)
    {
        for (const auto &param : *node.constructorParams)
        {
            typeDef.constructorParams.push_back(param.name);
        }
    }
    typeDef.baseArgs = node.baseArgs;

    // Establecer como tipo actual para procesar atributos y metodos
    context.typeSystem.set_current_type(node.name);

    // Procesar atributos
    if (node.body->attributes)
    {
        for (const auto &attr : *node.body->attributes)
        {
            context.typeSystem.add_attribute(attr.name, node.name, attr.initializer);
            std::cout << "  - Agregado atributo: " << attr.name << " del tipo " << node.name << std::endl;
        }
    }

    // Procesar metodos
    if (node.body->methods)
    {
        for (const auto &method : *node.body->methods)
        {
            context.typeSystem.add_method(node.name, method.name, method.params, method.body, method.returnType);
            std::cout << "  - Agregado metodo: " << method.name << std::endl;
        }
    }

    std::cout << "[CHECK] Tipo " << node.name << " procesado" << std::endl;
}

void IRGenerator::visit(InitInstance &node)
{
    std::cout << "- Nueva instancia: " << node.typeName << std::endl;

    // Comprueba si estamos dentro de una declaración let
    std::string varName = context.typeSystem.get_current_placeholder().name;
    if (!varName.empty())
    {
        std::cout << "  - Usando variable reservada: " << varName << std::endl;
    }

    // Inicializando mapa de variables de instancia y estableciendo tipo actual
    std::map<std::pair<std::string, std::string>, llvm::Value *> instanceVars;
    std::string currType = node.typeName;
    context.typeSystem.set_current_type(currType);

    // Crear un nuevo scope de variables sin herencia
    context.PushVar(false);

    // Procesar argumentos del constructor
    std::vector<llvm::Value *> args;
    for (ASTNode *arg : node.args)
    {
        arg->accept(*this);
        args.push_back(context.valueStack.back());
        context.valueStack.pop_back();
    }

    // Procesar jerarquía de tipos
    while (!currType.empty())
    {
        std::cout << "  - Procesando tipo: " << currType << std::endl;

        // Obtener información de tipos
        const auto &typeConst = context.typeSystem.get_constructor_params(currType);
        const auto &fatherArgs = context.typeSystem.get_base_args(currType);
        const auto &attrType = context.typeSystem.get_attributes(currType);
        const auto &father = context.typeSystem.get_parent_type(currType);

        // Procesar parámetros del constructor
        if (!typeConst.empty())
        {
            for (size_t i = 0; i < typeConst.size() && i < args.size(); ++i)
            {
                context.addLocal(typeConst[i], args[i]);
                std::cout << "    - Agregado parametro de constructor: " << typeConst[i] << std::endl;
            }
            // Remover argumentos procesados
            args.erase(args.begin(), args.begin() + std::min(typeConst.size(), args.size()));
        }

        // Procesar parámetros del constructor padre
        if (father)
        {
            const auto &fatherConst = context.typeSystem.get_constructor_params(*father);

            // Procesar argumentos de la base primero
            if (!fatherArgs.empty())
            {
                for (size_t i = 0; i < fatherArgs.size() && i < fatherConst.size(); ++i)
                {
                    fatherArgs[i]->accept(*this);
                    context.addLocal(fatherConst[i], context.valueStack.back());
                    context.valueStack.pop_back();
                    std::cout << "    - Agregados argumentos de la base: " << fatherConst[i] << std::endl;
                }
            }

            // Procesar argumentos restantes con los restantes parametros del constructor del padre
            size_t startIdx = fatherArgs.size();
            for (size_t i = 0; i < fatherConst.size() - startIdx && i < args.size(); ++i)
            {
                context.addLocal(fatherConst[startIdx + i], args[i]);
                std::cout << "    - Agregando parametros del padre: " << fatherConst[startIdx + i] << std::endl;
            }
            // Remover argumentos procesados
            args.erase(args.begin(), args.begin() + std::min(fatherConst.size() - startIdx, args.size()));
        }

        // Procesar atributos
        for (const auto &[attrName, attr] : attrType)
        {
            if (attr.initializer)
            {
                attr.initializer->accept(*this);
                if (!context.valueStack.empty())
                {
                    // Usar el tipo real del atributo como clave
                    instanceVars[{attrName, attr.TypeName}] = context.valueStack.back();
                    context.valueStack.pop_back();
                    std::cout << "    - Agregado atributo: " << attrName << " de tipo " << attr.TypeName << std::endl;
                }
            }
        }

        // Moverse al procesamiento del padre
        if (father)
        {
            currType = *father;
            context.typeSystem.set_current_type(currType);
        }
        else
        {
            currType = "";
        }
    }

    // Limpiar scope
    context.PopVar();
    context.typeSystem.set_current_type("");

    // Crear la instancia con sus variables
    context.typeSystem.new_instance(varName, node.typeName, instanceVars);

    std::cout << "[CHECK] Instancia creada: " << varName << std::endl;
}

void IRGenerator::visit(MethodCall &node)
{
    std::cout << "- Llamada a metodo: " << node.instanceName << "." << node.methodName << std::endl;

    std::string instanceName = node.instanceName;
    std::string realInstanceName = instanceName;  // Usar el nombre real por defecto
    std::map<std::pair<std::string, std::string>, llvm::Value *> *instanceVarsMap = nullptr;

    if (instanceName == "self")
    {
        // Obtener todos los nombres de instancia y usar el último
        auto instanceNames = context.typeSystem.get_all_instances_names();
        if (!instanceNames.empty())
        {
            realInstanceName = instanceNames.back();  // Actualizar nombre real
            instanceVarsMap = context.typeSystem.get_instance_vars_mutable(realInstanceName);
            std::cout << "[SELF] Usando instancia real: " << realInstanceName << std::endl;
        }
        else
        {
            throw std::runtime_error("[ERROR] No hay instancias disponibles para 'self' en linea " + 
                                     std::to_string(node.line()));
        }
    }
    else
    {
        instanceVarsMap = context.typeSystem.get_instance_vars_mutable(instanceName);
    }

    if (!instanceVarsMap)
    {
        throw std::runtime_error("[ERROR] Variables de instancia no encontradas para '" + realInstanceName + 
                                 "' en linea " + std::to_string(node.line()));
    }

    // Apilar PUNTERO al mapa global de variables
    context.typeSystem.push_current_instance_vars(instanceVarsMap);
    
    // OBTENER EL TIPO USANDO EL NOMBRE REAL DE LA INSTANCIA
    std::string typeName = context.typeSystem.get_instance_type(realInstanceName);
    if (typeName.empty())
    {
        throw std::runtime_error("[ERROR] Instancia '" + realInstanceName + "' no encontrada en linea " + 
                                 std::to_string(node.line()));
    }

    context.typeSystem.set_current_type(typeName);

    // Crear un nuevo scope de variables sin herencia
    context.PushVar(false);

    // Establecer el nombre del metodo en la memoria reservada
    context.typeSystem.push_placeholder(node.methodName, "method");

    // Encontrar método en la jerarquía
    type_method *method = nullptr;
    std::string currType = typeName;
    while (!currType.empty() && !method)
    {
        method = context.typeSystem.find_method(currType, node.methodName);
        if (!method)
        {
            currType = context.typeSystem.get_parent_type(currType).value_or("");
        }
    }

    if (!method)
    {
        throw std::runtime_error("[ERROR] Metodo '" + node.methodName + "' no encontrado en la jerarquia de tipos empezando por '" +
                                 typeName + "' en linea " + std::to_string(node.line()));
    }

    // Procesar argumentos y parametros
    std::vector<llvm::Value *> args;
    for (ASTNode *arg : node.args)
    {
        arg->accept(*this);
        args.push_back(context.valueStack.back());
        context.valueStack.pop_back();
    }

    // Asociar parametros con valores de argumentos
    if (method->params)
    {
        for (size_t i = 0; i < method->params->size() && i < args.size(); ++i)
        {
            context.addLocal((*method->params)[i].name, args[i]);
            std::cout << "  - Vinculando parametros a valores " << (*method->params)[i].name << std::endl;
        }
    }

    // Evaluar cuerpo de metodo
    method->body->accept(*this);

    // Limpiar
    context.typeSystem.set_current_type("");
    context.typeSystem.pop_placeholder();
    context.PopVar();
    context.typeSystem.pop_current_instance_vars();

    std::cout << "[CHECK] Llamada al metodo procesada" << std::endl;
}

void IRGenerator::visit(SelfCall &node)
{
    std::cout << "- Llamada self: " << node.varName << " - T-PILA: " << context.valueStack.size() << std::endl;

    // Obtener tipo actual
    std::string currentType = context.typeSystem.get_current_type();
    if (currentType.empty())
    {
        throw std::runtime_error("[ERROR] Acceso a 'self' fuera del contexto del tipo en linea " +
                                 std::to_string(node.line()));
    }

    // Buscar el atributo en la jerarquía de tipos
    type_attribute *attr = context.typeSystem.find_attribute(currentType, node.varName);
    if (!attr)
    {
        throw std::runtime_error("[ERROR] Atributo '" + node.varName +
                                 "' no encontrado en tipo '" + currentType +
                                 "' en línea " + std::to_string(node.line()));
    }

    // Obtener el valor usando el tipo real del atributo
    llvm::Value *val = context.typeSystem.get_current_instance_var(node.varName, attr->TypeName);
    if (!val)
    {
        throw std::runtime_error("[ERROR] Valor no encontrado para atributo '" + node.varName +
                                 "' en línea " + std::to_string(node.line()));
    }

    context.valueStack.push_back(val);
    std::cout << "  - Valor self enviado a la pila - T-PILA: " << context.valueStack.size() << std::endl;
}

void IRGenerator::visit(BaseCall &node)
{
    std::cout << "- Llamada a origen - T-PILA: " << context.valueStack.size() << std::endl;

    // Obtener espacio de memoria reservado
    placeholder currentPlaceholder = context.typeSystem.get_current_placeholder();
    std::string name = currentPlaceholder.name;
    std::string elemType = currentPlaceholder.type;

    if (name.empty() || elemType.empty())
    {
        throw std::runtime_error("[ERROR] Reserva invalida para llamada a origen en linea " + std::to_string(node.line()));
    }

    // Obtener tipo actual y sus padres
    std::string currentType = context.typeSystem.get_current_type();
    if (currentType.empty())
    {
        throw std::runtime_error("[ERROR] Llamada a origen fuera del contexto del tipo en linea " + std::to_string(node.line()));
    }

    std::optional<std::string> parentType = context.typeSystem.get_parent_type(currentType);
    if (!parentType)
    {
        throw std::runtime_error("[ERROR] No se ha encontrado un padre para el tipo '" + currentType +
                                 "' en linea " + std::to_string(node.line()));
    }

    // Manejar accesos a variables
    if (elemType == "var")
    {
        llvm::Value *val = context.typeSystem.get_current_instance_var(name, *parentType);
        if (!val)
        {
            throw std::runtime_error("[ERROR] Variable indefinida '" + name +
                                     "' en tipo padre '" + *parentType +
                                     "' en linea " + std::to_string(node.line()));
        }
        context.valueStack.push_back(val);
        std::cout << "  - Valor de variable origen enviado a la pila - T-PILA: " << context.valueStack.size() << std::endl;
        std::cout << "[CHECK] Acceso a variable de origen procesado" << std::endl;
        return;
    }

    // Manejar llamada a método
    if (elemType == "method")
    {
        std::cout << "  - Procesando llamada a metodo de origen" << std::endl;
        // Actualizar tipo actual al padre
        context.typeSystem.set_current_type(*parentType);

        // Crear nuevo scope de variables sin herencia
        context.PushVar(false);

        // Reservar espacio en memoria para un metodo
        context.typeSystem.push_placeholder(name, "method");

        // Encontrar metodo en la jerarquia de tipos
        type_method *method = nullptr;
        std::string currType = *parentType;
        while (!currType.empty() && !method)
        {
            method = context.typeSystem.find_method(currType, name);
            if (!method)
            {
                currType = context.typeSystem.get_parent_type(currType).value_or("");
            }
        }

        if (!method)
        {
            throw std::runtime_error("[ERROR] Metodo '" + name + "' no encontrado en la jerarquia de tipos empezando en '" +
                                     *parentType + "' en linea " + std::to_string(node.line()));
        }

        // Procesar argumentos y parametros
        std::vector<llvm::Value *> args;
        for (ASTNode *arg : node.args)
        {
            arg->accept(*this);
            args.push_back(context.valueStack.back());
            context.valueStack.pop_back();
        }

        // Asociar valores de argumentos a parametros
        if (method->params)
        {
            for (size_t i = 0; i < method->params->size() && i < args.size(); ++i)
            {
                context.addLocal((*method->params)[i].name, args[i]);
                std::cout << "  - Parametro vinculado " << (*method->params)[i].name << std::endl;
            }
        }

        // Evaluar cuerpo
        method->body->accept(*this);

        // Limpiar
        context.typeSystem.set_current_type(currentType); // Restaurar tipo original
        context.typeSystem.pop_placeholder();
        context.PopVar();

        std::cout << "  - Resultado del metodo de origen enviado a la pila - T-PILA: " << context.valueStack.size() << std::endl;
        std::cout << "[CHECK] Procesada llamada a metodo de origen" << std::endl;
        return;
    }

    throw std::runtime_error("[ERROR] Tipo de elemento invalido '" + elemType +
                             "' para llamada a origen en linea " + std::to_string(node.line()));
}
