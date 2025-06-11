#include "SemanticAnalyzer.hpp"
#include "../ast/AST.hpp"
#include "../utils/F_Collector.hpp"
#include <cctype>
#include <set>
#include <iostream>
#include <unordered_set>
#include <vector>


SymbolTable &SemanticAnalyzer::getSymbolTable()
{
    return symbolTable;
}

std::string SemanticAnalyzer::inferParamUsageType(const std::string& paramName, ASTNode* body) {
    std::set<std::string> usageTypes;

    collectParamUsages(body, paramName, usageTypes);

    if (usageTypes.empty()) return "Unknown";

    return symbolTable.lowestCommonAncestor(std::vector<std::string>(
        usageTypes.begin(), usageTypes.end()
    ));
}

void SemanticAnalyzer::collectParamUsages(ASTNode* node, const std::string& paramName, std::set<std::string>& types) {
    if (!node) return;

    // Identificador que referencia el parámetro
    if (auto* id = dynamic_cast<IdentifierNode*>(node)) {
        if (id->name == paramName && id->type() != "Unknown" && id->type() != "Error") {
            types.insert(id->type());
        }
    }

    // Binary operation
    else if (auto* bin = dynamic_cast<BinaryOperationNode*>(node)) {
        collectParamUsages(bin->left, paramName,  types);
        collectParamUsages(bin->right, paramName,  types);
    }

    // Unary operation
    else if (auto* un = dynamic_cast<UnaryOpNode*>(node)) {
        collectParamUsages(un->operand, paramName,  types);
    }

    // Function call
    else if (auto* call = dynamic_cast<FunctionCallNode*>(node)) {
        for (auto* arg : call->args)
            collectParamUsages(arg, paramName,  types);
    }

    // Built-in function call
    else if (auto* builtin = dynamic_cast<BuiltInFunctionNode*>(node)) {
        for (auto* arg : builtin->args)
            collectParamUsages(arg, paramName,  types);
    }

    // Function declaration
    else if (auto* func = dynamic_cast<FunctionDeclarationNode*>(node)) {
        collectParamUsages(func->body, paramName,  types);
    }

    // Method call
    // else if (auto* method = dynamic_cast<MethodCallNode*>(node)) {
    //     collectParamUsages(method->object,paramName,  types);
    //     for (auto* arg : method->args)
    //         collectParamUsages(arg, paramName,  types);
    // }

    // Let expression
    else if (auto* let = dynamic_cast<LetNode*>(node)) {
        for (auto& decl : *let->declarations) {
            collectParamUsages(decl.initializer, paramName,  types);
        }
        collectParamUsages( let->body, paramName, types);
    }

    // If expression
    else if (auto* ifn = dynamic_cast<IfNode*>(node)) {
        for (auto& branch : *ifn->branches) {
            collectParamUsages( branch.condition, paramName, types);
            collectParamUsages(branch.body, paramName,  types);
        }
        if (ifn->elseBody)
            collectParamUsages(ifn->elseBody ,paramName,  types);
    }

    // While
    else if (auto* wh = dynamic_cast<WhileNode*>(node)) {
        collectParamUsages(wh->condition, paramName,  types);
        collectParamUsages(wh->body, paramName,  types);
    }

    // For
    else if (auto* forNode = dynamic_cast<ForNode*>(node)) {
        collectParamUsages(forNode->init_range, paramName,  types);
        collectParamUsages(forNode->end_range, paramName,  types);
        collectParamUsages(forNode->body, paramName,  types);
    }

    // Assignment
    else if (auto* assign = dynamic_cast<AssignmentNode*>(node)) {
        collectParamUsages(assign->rhs, paramName,  types);
    }

    // Variable declaration
    else if (auto* decl = dynamic_cast<VariableDeclarationNode*>(node)) {
        collectParamUsages(decl->initializer, paramName,  types);
    }

    // Block
    else if (auto* block = dynamic_cast<BlockNode*>(node)) {
        for (auto* expr : block->expressions)
            collectParamUsages(expr, paramName, types);
    }

    // New instance
    else if (auto* inst = dynamic_cast<NewInstanceNode*>(node)) {
        for (auto* arg : inst->args)
            collectParamUsages(arg, paramName,  types);
    }

    // Type declaration
    // else if (auto* typeDecl = dynamic_cast<TypeDeclarationNode*>(node)) {
    //     for (const auto& attr : *typeDecl->attributes)
    //         collectParamUsages(attr.initializer, paramName,  types);
    //     for (const auto& method : *typeDecl->methods)
    //         collectParamUsages(method.body, paramName,  types);
    //     for (ASTNode* arg : typeDecl->baseArgs)
    //         collectParamUsages(arg, paramName,  types);
    // }

    // Literal: no hay nada que recorrer
    else if (dynamic_cast<LiteralNode*>(node)) {
        return;
    }

    // Otro nodo no manejado explícitamente
    else {
        // Opcional: log de depuración
        // std::cerr << "Nodo no manejado en collectParamUsages: " << typeid(*node).name() << "\n";
    }
}


bool SemanticAnalyzer::conformsTo(const std::string &subtype, const std::string &supertype)
{
    if (subtype == "Error" || supertype == "Error")
        return false;
    if (subtype == supertype)
        return true;
    if (supertype == "Object")
        return true;
    if (supertype == "")
        return true;

    TypeSymbol *sub = symbolTable.lookupType(subtype);
    while (sub && !sub->parentType.empty())
    {
        if (sub->parentType == supertype)
            return true;
        sub = symbolTable.lookupType(sub->parentType);
    }

    return false;
}

void SemanticAnalyzer::analyze(const std::vector<ASTNode *> &nodes)
{
    std::cout << "[SEMANTIC CHECK] Entra en analyze." << std::endl;

    FunctionCollector collector(symbolTable, errors);
    collector.addBuiltins();

    std::cout << "[SEMANTIC CHECK] Builtins agregados." << std::endl;

    for (ASTNode *node : nodes)
    {
        if (!node)
        {
            std::cerr << "[SEMANTIC ERROR] Nodo nulo en AST." << std::endl;
            continue;
        }

        std::cout << "[SEMANTIC CHECK] Recolectando funciones para nodo tipo: " << typeid(*node).name() << std::endl;
        node->accept(collector);
    }

    std::cout << "[SEMANTIC CHECK] Fase de recolección completada." << std::endl;

    for (ASTNode *node : nodes)
    {
        try
        {
            std::cout << "Análisis semántico de: " << typeid(*node).name() << std::endl;
            node->accept(*this);
        }
        catch (const std::exception &e)
        {
            std::cerr << "[SEMANTIC ERROR] Error durante análisis semántico: " << e.what() << std::endl;
        }
    }

    if (!errors.empty())
    {
        std::cerr << "[SEMANTIC ERROR] Errores semánticos encontrados:\n";
        for (const auto &e : errors)
        {
            std::cerr << "- Línea " << e.line << ": " << e.message << "\n";
        }
    }
    else
    {
        std::cout << "[SEMANTIC CHECK] No se encontraron errores semánticos.\n";
    }

    if (!errors.empty())
    {
        std::cerr << "[SEMANTIC ERROR] Errores semánticos presentes. Abortando ejecución.\n";
        exit(1);
    }

    std::cout << "[SEMANTIC CHECK] Análisis semántico completado." << std::endl;
}

void SemanticAnalyzer::visit(ASTNode &node) {}

void SemanticAnalyzer::visit(UnaryOpNode &node)
{
    node.operand->accept(*this); // Analiza operand
    std::string operandType = node.operand->type();

    if (node.op == "-")
    {
        if (operandType != "Number")
        {
            errors.emplace_back("[SEMANTIC ERROR] El operador '-' requiere un operando de tipo Number", node.line());
            node._type = "Error";
        }
        node._type = "Number";
    }
    else if (node.op == "!")
    {
        if (operandType != "Boolean")
        {
            errors.emplace_back("[SEMANTIC ERROR] El operador '!' requiere un operando de tipo Boolean", node.line());
            node._type = "Error";
        }
        node._type = "Boolean";
    }
    else
    {
        errors.emplace_back("[SEMANTIC ERROR] Operador unario desconocido: " + node.op, node.line());
        node._type = "Error";
        ;
    }
}

void SemanticAnalyzer::visit(BuiltInFunctionNode &node)
{
    for (ASTNode *arg : node.args)
    {
        arg->accept(*this);
    }

    const std::string &fn = node.name;
    size_t arity = node.args.size();

    for (ASTNode *arg : node.args)
    {
        arg->accept(*this);
    }    

    if (fn == "print")
    {
        if (arity != 1)
        {
            errors.emplace_back("[SEMANTIC ERROR] La función 'print' requiere exactamente 1 argumento", node.line());
            node._type = "Error";
            return;
        }
        node._type = node.args[0]->type();
    }
    else if (fn == "sin" || fn == "cos" || fn == "exp" || fn == "sqrt")
    {
        if (arity != 1)
        {
            errors.emplace_back("[SEMANTIC ERROR] Función " + fn + " requiere 1 argumento", node.line());
            node._type = "Error";
        }
        else if (node.args[0]->type() != "Number")
        {
            errors.emplace_back("[SEMANTIC ERROR] El argumento de '" + fn + "' debe ser un número", node.line());
            node._type = "Error";
        }
        else
        {
            node._type = "Number";
        }
    }
    else if (fn == "log")
    {
        if (arity != 2)
        {
            errors.emplace_back("[SEMANTIC ERROR] La función 'log' requiere 2 argumentos", node.line());
            node._type = "Error";
        }
        else if (node.args[0]->type() != "Number" || node.args[1]->type() != "Number")
        {
            errors.emplace_back("[SEMANTIC ERROR] Los argumentos de 'log' deben ser numéricos", node.line());
            node._type = "Error";
        }
        else
        {
            node._type = "Number";
        }
    }
    else if (fn == "rand")
    {
        if (arity != 0)
        {
            errors.emplace_back("[SEMANTIC ERROR] La función 'rand' no acepta argumentos", node.line());
            node._type = "Error";
        }
        else
        {
            node._type = "Number";
        }
    }
    else if (fn == "min" || fn == "max")
    {
        if (arity != 2)
        {
            errors.emplace_back("[SEMANTIC ERROR] La función '" + fn + "' requiere 2 argumentos", node.line());
            node._type = "Error";
        }
        else if (node.args[0]->type() != "Number" || node.args[1]->type() != "Number")
        {
            errors.emplace_back("[SEMANTIC ERROR] Los argumentos de '" + fn + "' deben ser numéricos", node.line());
            node._type = "Error";
        }
        else
        {
            node._type = "Number";
        }
    }
    else if (fn == "range")
    {
        if (arity != 2)
        {
            errors.emplace_back("[SEMANTIC ERROR] La función 'range' requiere exactamente 2 argumentos", node.line());
            node._type = "Error";
        }
        else if (node.args[0]->type() != "Number" || node.args[1]->type() != "Number")
        {
            errors.emplace_back("[SEMANTIC ERROR] Los argumentos de 'range' deben ser numéricos", node.line());
            node._type = "Error";
        }
        else
        {
            node._type = "Iterator";
        }
    }
    else
    {
        errors.emplace_back("[SEMANTIC ERROR] Función builtin '" + fn + "' no reconocida", node.line());
        node._type = "Error";
    }
}

void SemanticAnalyzer::visit(FunctionDeclarationNode &node)
{
    symbolTable.enterScope();

    std::unordered_map<std::string, bool> paramSeen;

    // Paso 1: insertar todos los parámetros con tipo Unknown (si no están anotados)
    for (const auto &param : *node.params)
    {
        if (paramSeen.count(param.name))
        {
            errors.emplace_back("[SEMANTIC ERROR] Parámetro duplicado '" + param.name + "'", node.line());
            node._type = "Error";
            continue;
        }

        paramSeen[param.name] = true;
        std::string paramType = param.type.empty() ? "Unknown" : param.type;
        symbolTable.addSymbol(param.name, paramType, false);
    }

    // Paso 2: analizar el cuerpo con símbolos disponibles
    node.body->accept(*this);
    std::string bodyType = node.body->type();

    for (auto &param : *node.params)
    {
        if (param.type.empty())
        {
            std::string inferred = inferParamUsageType(param.name, node.body);
            if (inferred == "Unknown" || inferred.empty())
            {
                errors.emplace_back("[SEMANTIC ERROR] No se pudo inferir el tipo del parámetro '" + param.name + "'", node.line());
                node._type = "Error";
            }
            else
            {
                param.type = inferred;
                symbolTable.updateSymbolType(param.name, inferred);
            }
        }
    }

    // Paso 3: Inferencia firme para parámetros sin tipo explícito
    for (auto &param : *node.params)
    {
        if (!param.type.empty())
            continue;

        std::set<std::string> observedTypes;
        collectParamUsages(node.body, param.name, observedTypes);

        if (observedTypes.empty())
        {
            errors.emplace_back("[SEMANTIC ERROR] No se pudo inferir tipo para parámetro '" + param.name + "'", node.line());
            node._type = "Error";
        }
        else if (observedTypes.size() == 1)
        {
            param.type = *observedTypes.begin();
        }
        else
        {
            std::set<std::string> usageTypes;

            std::vector<std::string> observedVec(usageTypes.begin(), usageTypes.end());
            std::string common = symbolTable.lowestCommonAncestor(observedVec);
            if (common == "Object")
            {
                errors.emplace_back("[SEMANTIC ERROR] Inferencia ambigua para parámetro '" + param.name + "'", node.line());
                node._type = "Error";
            }
            else
            {
                param.type = common;
            }
        }

        // Actualizar en la tabla de símbolos con tipo inferido
        symbolTable.lookup(param.name)->type = param.type;
    }

    // Paso 4: Verificación de tipo de retorno
    if (!node.returnType.empty() && node.returnType != bodyType)
    {
        errors.emplace_back("[SEMANTIC ERROR] Tipo de retorno incorrecto en función '" + node.name + "'", node.line());
        node._type = "Error";
    }

    node._type = node.returnType.empty() ? bodyType : node.returnType;
    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(FunctionCallNode &node)
{
    if (node.funcName == "base")
    {
        std::cout << "[DEBUG] Buscando función: " << node.funcName << std::endl;
        Symbol *self = symbolTable.lookup("self");
        if (!self)
        {
            errors.emplace_back("[SEMANTIC ERROR] 'base' solo puede usarse dentro de métodos", node.line());
            node._type = "Error";
            return;
        }

        TypeSymbol *typeSym = symbolTable.lookupType(self->type);
        if (!typeSym || typeSym->parentType == "Object")
        {
            errors.emplace_back("[SEMANTIC ERROR] 'base' no disponible para este tipo", node.line());
            node._type = "Error";
            return;
        }

        node._type = typeSym->parentType;
        return;
    }

    // Función builtin: print(...)
    if (node.funcName == "print")
    {
        for (auto arg : node.args)
        {
            arg->accept(*this);
        }
        node._type = "void";
        return;
    }

    // Función builtin: range(...)
    if (node.funcName == "range")
    {
        for (auto arg : node.args)
        {
            arg->accept(*this);
        }

        if (node.args.size() != 2)
        {
            errors.emplace_back("[SEMANTIC ERROR] 'range' espera 2 argumentos", node.line());
            node._type = "Error";
        }
        else
        {
            std::string t1 = node.args[0]->type();
            std::string t2 = node.args[1]->type();
            if (t1 != "Number" || t2 != "Number")
            {
                errors.emplace_back("[SEMANTIC ERROR] Argumentos de 'range' deben ser de tipo 'Number'", node.line());
                node._type = "Error";
            }
            else
            {
                node._type = "Iterator";
            }
        }
        return;
    }

    // Funciones definidas por el usuario o builtins restantes
    Symbol *symbol = symbolTable.lookup(node.funcName);
    if (!symbol || symbol->kind != "function")
    {
        std::cerr << "[DEBUG] No se encontró la función '" << node.funcName << "' en la tabla de símbolos.\n";
        errors.emplace_back("[SEMANTIC ERROR] Función '" + node.funcName + "' no definida", node.line());
        node._type = "Error";
        return;
    }

    if (node.args.size() != symbol->params.size())
    {
        errors.emplace_back("[SEMANTIC ERROR] Número incorrecto de argumentos para '" + node.funcName + "'", node.line());
        node._type = "Error";
        return;
    }

    for (size_t i = 0; i < node.args.size(); ++i)
    {
        node.args[i]->accept(*this);
        std::string argType = node.args[i]->type();
        std::string expectedType = symbol->params[i];

        if (!conformsTo(argType, expectedType))
        {
            errors.emplace_back("[SEMANTIC ERROR] Tipo incorrecto para argumento " + std::to_string(i + 1) +
                                    " en '" + node.funcName + "': esperado '" + expectedType + "', obtenido '" + argType + "'",
                                node.line());
            node._type = "Error";
        }
    }

    node._type = symbol->type;
}

void SemanticAnalyzer::visit(BinaryOperationNode &node)
{

    std::cout << "Visitando BinaryOperationNode con operador: " << node.op << "\n";

    if (auto *leftBin = dynamic_cast<BinaryOperationNode *>(node.left))
    {
        std::cout << "  Left es otro BinaryOperationNode con operador: " << leftBin->op << "\n";
    }
    else
    {
        std::cout << "  Left no es BinaryOperationNode\n";
    }

    if (auto *rightBin = dynamic_cast<BinaryOperationNode *>(node.right))
    {
        std::cout << "  Right es otro BinaryOperationNode con operador: " << rightBin->op << "\n";
    }
    else
    {
        std::cout << "  Right no es BinaryOperationNode\n";
    }

    node.left->accept(*this);
    node.right->accept(*this);

    std::string leftType = node.left->type();
    std::string rightType = node.right->type();

    std::cout << ">>> Visitando operador binario: " << node.op << std::endl;
    std::cout << "    Left node: " << node.left << std::endl;
    std::cout << "    Right node: " << node.right << std::endl;
    std::cout << "    Left type: " << leftType << std::endl;
    std::cout << "    Right type: " << rightType << std::endl;

    const std::set<std::string> comparisonOps = {"==", "!=", "<", ">", "<=", ">="};

    if (comparisonOps.count(node.op))
    {
        if (node.op == "==" || node.op == "!=")
        {
            if (!conformsTo(leftType, rightType) && !conformsTo(rightType, leftType))
            {
                errors.emplace_back("[SEMANTIC ERROR] Operandos de " + node.op + " deben ser compatibles", node.line());
                node._type = "Error";
            }
            else
            {
                node._type = "Boolean";
            }
        }
        else if (leftType != "Number" || rightType != "Number")
        {
            errors.emplace_back("[SEMANTIC ERROR] Operandos de " + node.op + " deben ser números", node.line());
            node._type = "Error";
        }
        else
        {
            node._type = "Boolean";
        }
    }
    else if (node.op == "&" || node.op == "|")
    {
        if (leftType != "Boolean" || rightType != "Boolean")
        {
            errors.emplace_back("[SEMANTIC ERROR] Operandos de " + node.op + " deben ser booleanos", node.line());
            node._type = "Error";
        }
        else
        {
            node._type = "Boolean";
        }
    }
    else if (node.op == "@" || node.op == "@@")
    {
        if ((leftType != "String" && leftType != "Number") ||
            (rightType != "String" && rightType != "Number"))
        {
            errors.emplace_back("[SEMANTIC ERROR] Operandos de @ deben ser string o number", node.line());
            node._type = "Error";
        }
        else
        {
            node._type = "String";
        }
    }
    else
    {
        if (leftType != "Number" || rightType != "Number")
        {
            errors.emplace_back("[SEMANTIC ERROR] Operandos de " + node.op + " deben ser números", node.line());
            node._type = "Error";
            std::cout << leftType << std::endl;
            std::cout << rightType << std::endl;
        }
        else
        {
            node._type = "Number";
        }
    }
}

void SemanticAnalyzer::visit(LiteralNode &node)
{
    node._type = node._type;
}

void SemanticAnalyzer::visit(BlockNode &node)
{
    symbolTable.enterScope();

    std::string lastType = "Null"; // Por defecto
    for (auto expr : node.expressions)
    {
        expr->accept(*this);
        lastType = expr->type(); // Actualiza con el tipo de la última expresión
    }
    symbolTable.exitScope();
    node._type = lastType;
}

void SemanticAnalyzer::visit(VariableDeclarationNode &node)
{
    if (symbolTable.existsInCurrentScope(node.name))
    {
        errors.emplace_back("[SEMANTIC ERROR] Variable '" + node.name + "' ya declarada", node.line());
        node._type = "Error";
        return;
    }

    if (!node.initializer && node.declaredType.empty())
    {
        errors.emplace_back("[SEMANTIC ERROR] Variable '" + node.name + "' sin tipo ni inicializador", node.line());
        node._type = "Error";
        return;
    }

    if (node.initializer)
    {
        node.initializer->accept(*this);
        std::string initType = node.initializer->type();
        if (!node.declaredType.empty() && node.declaredType != initType)
        {
            errors.emplace_back("[SEMANTIC ERROR] Tipo declarado no coincide con el inicializador", node.line());
            node._type = "Error";
        }
        node._type = !node.declaredType.empty() ? node.declaredType : initType;
    }
    else
    {
        node._type = node.declaredType;
    }

    symbolTable.addSymbol(node.name, node._type, !node.isMutable);
}

void SemanticAnalyzer::visit(IdentifierNode &node)
{
    Symbol *symbol = symbolTable.lookup(node.name);
    if (!symbol)
    {
        errors.emplace_back("[SEMANTIC ERROR] Variable '" + node.name + "' no declarada", node.line());
        node._type = "Error";
        return;
    }
    node._type = symbol->type;
}

bool SemanticAnalyzer::isValidIdentifier(const std::string &name)
{
    if (name.empty())
        return false;
    if (!std::isalpha(name[0]))
        return false;
    for (char c : name)
    {
        if (!std::isalnum(c) && c != '_')
            return false;
    }
    return true;
}

void SemanticAnalyzer::visit(LetNode &node)
{
    symbolTable.enterScope();

    for (auto &decl : *node.declarations)
    {
        if (!isValidIdentifier(decl.name))
        {
            errors.emplace_back("[SEMANTIC ERROR] Nombre inválido: '" + decl.name + "'", node.line());
            node._type = "Error";
        }

        if (!decl.initializer && decl.declaredType.empty())
        {
            errors.emplace_back("[SEMANTIC ERROR] Variable '" + decl.name + "' sin tipo ni inicializador", node.line());
            node._type = "Error";
        }

        // Verificar duplicados en el mismo let
        if (symbolTable.existsInCurrentScope(decl.name))
        {
            errors.emplace_back("[SEMANTIC ERROR] Variable '" + decl.name + "' ya declarada en este ámbito", node.line());
            node._type = "Error";
            continue;
        }

        // Analizar inicializador
        decl.initializer->accept(*this);
        std::string initType = decl.initializer->type();

        // Validar tipo declarado vs inferido
        if (!decl.declaredType.empty() && !conformsTo(initType, decl.declaredType))
        {
            errors.emplace_back("[SEMANTIC ERROR] Tipo declarado '" + decl.declaredType + "' no coincide con inicializador '" + initType + "'", node.line());
            node._type = "Error";
        }

        // Registrar en tabla de símbolos (no constante)
        symbolTable.addSymbol(decl.name, decl.declaredType.empty() ? initType : decl.declaredType, false);
    }

    // Analizar cuerpo
    node.body->accept(*this);
    node._type = node.body->type();

    symbolTable.exitScope();
}

// void SemanticAnalyzer::visit(AssignmentNode &node)
// {
//     node.lhs->accept(*this);
//     std::string lhsType = node.lhs->type();

//     node.rhs->accept(*this);
//     std::string rhsType = node.rhs->type();

//     if (!conformsTo(rhsType, lhsType))
//     {
//         errors.emplace_back("[SEMANTIC ERROR] Tipo incorrecto en asignación: esperado '" + lhsType + "', obtenido '" + rhsType + "'", node.line());
//         node._type = "Error";
//         return;
//     }

//     node._type = lhsType;
// }

void SemanticAnalyzer::visit(AssignmentNode &node)
{

    std::string name;
    if (auto *id = dynamic_cast<IdentifierNode *>(node.name))
    {
        name = id->name;
    }
    else if (auto *self = dynamic_cast<SelfCallNode *>(node.name))
    {
        name = self->varName;
    }

    Symbol *symbol = symbolTable.lookup(name);

    if (name == "self")
    {
        errors.emplace_back("No se puede reasignar 'self'", node.line());
        node._type = "Error";
        return;
    }

    if (!symbol)
    {
        errors.emplace_back("Variable '" + name + "' no declarada", node.line());
        node._type = "Error";
        return;
    }

    if (symbol->is_const)
    {
        errors.emplace_back("No se puede reasignar la constante '" + name + "'", node.line());
        node._type = "Error";
    }

    node.rhs->accept(*this);
    std::string rhsType = node.rhs->type();

    if (!conformsTo(rhsType, symbol->type))
    {
        errors.emplace_back("Tipo incorrecto en asignación: esperado '" + symbol->type + "', obtenido '" + rhsType + "'", node.line());
        node._type = "Error";
    }

    node._type = symbol->type;
}

void SemanticAnalyzer::visit(IfNode &node)
{

    std::vector<std::string> branchTypes;
    bool hasErrors = false;

    for (auto &branch : *node.branches)
    {
        branch.condition->accept(*this);
        std::string condType = branch.condition->type();
        if (condType != "Boolean")
        {
            errors.emplace_back("[SEMANTIC ERROR] Condición debe ser booleana", branch.condition->line());
            // node._type = "Error";
            hasErrors = true;
        }
        // Analizar cuerpo de la rama
        branch.body->accept(*this);
        branchTypes.push_back(branch.body->type());
    }

    if (node.elseBody)
    {
        node.elseBody->accept(*this);
        branchTypes.push_back(node.elseBody->type());
    }

    if (hasErrors)
    {
        node._type = "Error";
        return;
    }

    const std::string &commonType = branchTypes.front();
    for (const auto &t : branchTypes)
    {
        if (t != commonType)
        {
            errors.emplace_back("[SEMANTIC ERROR] Tipos incompatibles en ramas del 'if'", node.line());
            node._type = "Error";
            std::cout << t << std::endl;
            std::cout << commonType << std::endl;
            return;
        }
    }

    node._type = commonType;
}

void SemanticAnalyzer::visit(WhileNode &node)
{
    // Verificar condición es booleana
    node.condition->accept(*this);
    std::string condType = node.condition->type();
    if (condType != "Boolean")
    {
        errors.emplace_back("[SEMANTIC ERROR] La condición del while debe ser booleana", node.line());
        node._type = "Error";
        std::cout << condType << std::endl;
    }

    // Analizar cuerpo
    node.body->accept(*this);
    node._type = node.body->type();
}

void SemanticAnalyzer::visit(ForNode &node)
{
    node.init_range->accept(*this);
    node.end_range->accept(*this);

    if (node.init_range->type() != "Number" || node.end_range->type() != "Number")
    {
        errors.emplace_back("[SEMANTIC ERROR] Los límites del 'for' deben ser de tipo Number", node.line());
        node._type = "Error";
        return;
    }

    symbolTable.enterScope();
    symbolTable.addSymbol(node.varName, "Number", false);

    node.body->accept(*this);
    node._type = node.body->type();

    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(TypeDeclarationNode &node)
{
    // if (symbolTable.lookupType(node.name))
    // {
    //     errors.emplace_back("[SEMANTIC ERROR] Tipo '" + node.name + "' ya declarado", node.line());
    //     return;
    // }

    // const std::set<std::string> builtinTypes = {"Number", "String", "Boolean"};
    // if (node.baseType.has_value() && builtinTypes.count(*node.baseType))
    // {
    //     errors.emplace_back("[SEMANTIC ERROR] No se puede heredar de tipo básico '" + *node.baseType + "'", node.line());
    //     return;
    // }

    // std::string parent = node.baseType.value_or("Object");
    // std::vector<std::string> paramNames;
    // for (const auto &param : *node.constructorParams)
    // {
    //     paramNames.push_back(param.name);
    // }

    // if (!symbolTable.addType(node.name, parent, paramNames))
    // {
    //     errors.emplace_back("[SEMANTIC ERROR] Tipo '" + node.name + "' ya fue registrado", node.line());
    //     return;
    // }

    // if (node.baseType.has_value())
    // {
    //     symbolTable.enterScope();
    //     for (const auto &param : *node.constructorParams)
    //     {
    //         symbolTable.addSymbol(param.name, "Unknown", false);
    //     }
    //     for (ASTNode *arg : node.baseArgs)
    //     {
    //         arg->accept(*this);
    //     }

    //     TypeSymbol *parentSym = symbolTable.lookupType(parent);
    //     if (parentSym && parentSym->typeParams.size() != node.baseArgs.size())
    //     {
    //         errors.emplace_back("[SEMANTIC ERROR] Cantidad incorrecta de argumentos para constructor del padre", node.line());
    //     }

    //     symbolTable.exitScope();
    // }

    // symbolTable.enterScope();
    // for (const auto &param : *node.constructorParams)
    // {
    //     symbolTable.addSymbol(param.name, "Unknown", false);
    // }

    // for (const auto &attr : *node.attributes)
    // {
    //     attr.initializer->accept(*this);
    //     std::string inferredType = attr.initializer->type();

    //     if (inferredType == "Error")
    //     {
    //         errors.emplace_back("[SEMANTIC ERROR] No se pudo inferir el tipo del atributo '" + attr.name + "'", node.line());
    //     }
    //     else
    //     {
    //         symbolTable.addTypeAttribute(node.name, attr.name, inferredType);
    //     }
    // }
    // symbolTable.exitScope();

    // TypeSymbol *typeSym = symbolTable.lookupType(node.name);
    // for (const auto &method : *node.methods)
    // {
    //     symbolTable.enterScope();
    //     symbolTable.addSymbol("self", node.name, true);
    //     for (const auto &param : *method.params)
    //     {
    //         symbolTable.addSymbol(param.name, param.type, false);
    //     }

    //     method.body->accept(*this);

    //     if (!method.returnType.empty() &&
    //         !conformsTo(method.body->type(), method.returnType))
    //     {
    //         errors.emplace_back("[SEMANTIC ERROR] El cuerpo del método '" + method.name + "' no conforma al tipo de retorno declarado", node.line());
    //     }

    //     std::vector<std::string> paramTypes;
    //     for (const auto &param : *method.params)
    //     {
    //         paramTypes.push_back(param.type);
    //     }

    //     symbolTable.addTypeMethod(node.name, method.name, method.returnType, paramTypes);

    //     if (!typeSym->parentType.empty())
    //     {
    //         TypeSymbol *parentSym = symbolTable.lookupType(typeSym->parentType);
    //         if (parentSym)
    //         {
    //             auto it = parentSym->methods.find(method.name);
    //             if (it != parentSym->methods.end())
    //             {
    //                 const Symbol &inherited = it->second;
    //                 if (inherited.params != paramTypes || inherited.type != method.returnType)
    //                 {
    //                     errors.emplace_back("[SEMANTIC ERROR] Firma de método '" + method.name +
    //                                             "' no coincide con la del padre",
    //                                         node.line());
    //                 }
    //             }
    //         }
    //     }

    //     symbolTable.exitScope();
    // }
}

void SemanticAnalyzer::visit(NewInstanceNode &node)
{
    TypeSymbol *typeSym = symbolTable.lookupType(node.typeName);
    if (!typeSym)
    {
        errors.emplace_back("[SEMANTIC ERROR] Tipo '" + node.typeName + "' no definido", node.line());
        node._type = "Error";
        return;
    }

    if (node.args.size() != typeSym->typeParams.size())
    {
        errors.emplace_back("[SEMANTIC ERROR] Cantidad incorrecta de argumentos para el constructor de '" + node.typeName + "'", node.line());
        node._type = "Error";
        return;
    }

    for (ASTNode *arg : node.args)
    {
        arg->accept(*this);
        // Aquí podrías validar tipos si se almacenan los tipos de parámetros
    }

    node._type = node.typeName;
}

void SemanticAnalyzer::visit(MethodCallNode &node)
{
    // node.object->accept(*this);
    // std::string objectType = node.object->type();

    // TypeSymbol *typeSym = symbolTable.lookupType(objectType);
    // if (!typeSym)
    // {
    //     errors.emplace_back("[SEMANTIC ERROR] Tipo '" + objectType + "' no definido para llamada a método", node.line());
    //     node._type = "Error";
    //     return;
    // }

    // auto it = typeSym->methods.find(node.methodName);
    // if (it == typeSym->methods.end())
    // {
    //     errors.emplace_back("[SEMANTIC ERROR] Método '" + node.methodName + "' no existe en tipo '" + objectType + "'", node.line());
    //     node._type = "Error";
    //     return;
    // }

    // Symbol &method = it->second;
    // if (node.args.size() != method.params.size())
    // {
    //     errors.emplace_back("[SEMANTIC ERROR] Cantidad incorrecta de argumentos en llamada a método '" + node.methodName + "'", node.line());
    //     node._type = "Error";
    //     return;
    // }

    // for (size_t i = 0; i < node.args.size(); ++i)
    // {
    //     node.args[i]->accept(*this);
    //     if (node.args[i]->type() != method.params[i])
    //     {
    //         errors.emplace_back("[SEMANTIC ERROR] Tipo de argumento " + std::to_string(i + 1) + " incorrecto en llamada a método '" + node.methodName + "'", node.line());
    //     }
    // }

    // node._type = method.type;
}

void SemanticAnalyzer::visit(AttributeDeclaration &node)
{
    node.initializer->accept(*this);
}

void SemanticAnalyzer::visit(MethodDeclaration &node)
{
    // Implementar
}

void SemanticAnalyzer::visit(BaseCallNode &node)
{
    for (ASTNode *arg : node.args)
    {
        arg->accept(*this);
    }

    Symbol *self = symbolTable.lookup("self");
    if (!self)
    {
        errors.emplace_back("'base' solo puede usarse dentro de métodos", node.line());
        node._type = "Error";
        return;
    }

    TypeSymbol *typeSym = symbolTable.lookupType(self->type);
    if (!typeSym || typeSym->parentType.empty())
    {
        errors.emplace_back("'base' no disponible para este tipo", node.line());
        node._type = "Error";
        return;
    }

    node._type = typeSym->parentType;
}

void SemanticAnalyzer::visit(SelfCallNode &node)
{
    Symbol *self = symbolTable.lookup("self");
    if (!self)
    {
        errors.emplace_back("'self' solo puede usarse dentro de métodos", node.line());
        node._type = "Error";
        return;
    }

    TypeSymbol *typeSym = symbolTable.lookupType(self->type);
    if (!typeSym)
    {
        errors.emplace_back("Tipo de 'self' no encontrado", node.line());
        node._type = "Error";
        return;
    }

    auto it = typeSym->attributes.find(node.varName);
    if (it == typeSym->attributes.end())
    {
        errors.emplace_back("Atributo '" + node.varName + "' no existe en tipo '" + self->type + "'", node.line());
        node._type = "Error";
        return;
    }

    node._type = it->second.type;
}