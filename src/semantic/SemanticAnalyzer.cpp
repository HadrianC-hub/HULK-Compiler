#include "SemanticAnalyzer.hpp"
#include "../ast/AST.hpp"
#include "../utils/F_Collector.hpp"
#include <cctype>
#include <set>
#include <iostream>

SymbolTable &SemanticAnalyzer::getSymbolTable()
{
    return symbolTable;
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
    {
        return true;
    }

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
        node._type = node.args[0]->type(); // El tipo es el del argumento impreso
    }
    else if (fn == "sin" || fn == "cos" || fn == "exp" || fn == "sqrt")
    {
        if (node.args.size() != 1)
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
    else
    {
        errors.emplace_back("[SEMANTIC ERROR] Función builtin '" + fn + "' no reconocida", node.line());
        node._type = "Error";
    }
}

void SemanticAnalyzer::visit(FunctionDeclarationNode &node)
{
    symbolTable.enterScope();

    // Verificar que los parámetros no estén duplicados
    std::unordered_map<std::string, bool> seen;
    for (const auto &param : *node.params)
    {
        if (seen.count(param.name))
        {
            errors.emplace_back("[SEMANTIC ERROR] Parámetro duplicado '" + param.name + "'", node.line());
            node._type = "Error";
        }
        else
        {
            seen[param.name] = true;
            symbolTable.addSymbol(param.name, param.type, false);
        }
    }

    // Si no es inline, analizamos el cuerpo y verificamos tipos
    if (!node.isInline)
    {
        node.body->accept(*this);
        std::string bodyType = node.body->type();

        // Verificar tipo de retorno si está especificado
        if (!node.returnType.empty())
        {
            if (!conformsTo(bodyType, node.returnType))
            {
                errors.emplace_back("[SEMANTIC ERROR] Tipo de retorno incorrecto en función '" + node.name + "'", node.line());
                node._type = "Error";
            }
        }
        else
        {
            node.returnType = bodyType; // Inferencia si no se especificó
        }
    }

    // Inferencia de tipos de parámetros si no fueron anotados
    for (auto &param : *node.params)
    {
        if (param.type.empty())
        {
            Symbol *s = symbolTable.lookup(param.name);
            std::cout << "tipo de parámetro." << std::endl;
            if (s && !s->type.empty())
            {
                param.type = s->type;
            }
            else
            {
                // Si es inline, no disparamos error: se infiere en tiempo de invocación
                if (!node.isInline)
                {
                    errors.emplace_back("[SEMANTIC ERROR] No se pudo inferir tipo para el parámetro '" + param.name + "'", node.line());
                    node._type = "Error";
                }
            }
        }
    }

    symbolTable.exitScope();

    // No se agrega aquí a la tabla, lo hace FunctionCollector y guarda el cuerpo
}

void SemanticAnalyzer::visit(FunctionCallNode &node)
{
    if (node.funcName == "base")
    {
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

    // Funciones built-in como 'print'
    if (node.funcName == "print")
    {
        for (auto arg : node.args)
        {
            arg->accept(*this);
        }
        node._type = "void";
        return;
    }

    // Funciones definidas por el usuario
    Symbol *symbol = symbolTable.lookup(node.funcName);
    if (!symbol || symbol->kind != "function")
    {
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

void SemanticAnalyzer::visit(AssignmentNode &node)
{
    node.lhs->accept(*this);
    std::string lhsType = node.lhs->type();

    node.rhs->accept(*this);
    std::string rhsType = node.rhs->type();

    if (!conformsTo(rhsType, lhsType))
    {
        errors.emplace_back("[SEMANTIC ERROR] Tipo incorrecto en asignación: esperado '" + lhsType + "', obtenido '" + rhsType + "'", node.line());
        node._type = "Error";
        return;
    }

    node._type = lhsType;
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
        errors.emplace_back("[SEMANTIC ERROR] Condición del while debe ser booleana", node.line());
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
    if (symbolTable.lookupType(node.name))
    {
        errors.emplace_back("[SEMANTIC ERROR] Tipo '" + node.name + "' ya declarado", node.line());
        return;
    }

    const std::set<std::string> builtinTypes = {"Number", "String", "Boolean"};
    if (node.baseType.has_value() && builtinTypes.count(*node.baseType))
    {
        errors.emplace_back("[SEMANTIC ERROR] No se puede heredar de tipo básico '" + *node.baseType + "'", node.line());
        return;
    }

    std::string parent = node.baseType.value_or("Object");
    std::vector<std::string> paramNames;
    for (const auto &param : *node.constructorParams)
    {
        paramNames.push_back(param.name);
    }

    if (!symbolTable.addType(node.name, parent, paramNames))
    {
        errors.emplace_back("[SEMANTIC ERROR] Tipo '" + node.name + "' ya fue registrado", node.line());
        return;
    }

    if (node.baseType.has_value())
    {
        symbolTable.enterScope();
        for (const auto &param : *node.constructorParams)
        {
            symbolTable.addSymbol(param.name, "Unknown", false);
        }
        for (ASTNode *arg : node.baseArgs)
        {
            arg->accept(*this);
        }

        TypeSymbol *parentSym = symbolTable.lookupType(parent);
        if (parentSym && parentSym->typeParams.size() != node.baseArgs.size())
        {
            errors.emplace_back("[SEMANTIC ERROR] Cantidad incorrecta de argumentos para constructor del padre", node.line());
        }

        symbolTable.exitScope();
    }

    symbolTable.enterScope();
    for (const auto &param : *node.constructorParams)
    {
        symbolTable.addSymbol(param.name, "Unknown", false);
    }

    for (const auto &attr : *node.attributes)
    {
        attr.initializer->accept(*this);
        std::string inferredType = attr.initializer->type();

        if (inferredType == "Error")
        {
            errors.emplace_back("[SEMANTIC ERROR] No se pudo inferir el tipo del atributo '" + attr.name + "'", node.line());
        }
        else
        {
            symbolTable.addTypeAttribute(node.name, attr.name, inferredType);
        }
    }
    symbolTable.exitScope();

    TypeSymbol *typeSym = symbolTable.lookupType(node.name);
    for (const auto &method : *node.methods)
    {
        symbolTable.enterScope();
        symbolTable.addSymbol("self", node.name, true);
        for (const auto &param : *method.params)
        {
            symbolTable.addSymbol(param.name, param.type, false);
        }

        method.body->accept(*this);

        if (!method.returnType.empty() &&
            !conformsTo(method.body->type(), method.returnType))
        {
            errors.emplace_back("[SEMANTIC ERROR] El cuerpo del método '" + method.name + "' no conforma al tipo de retorno declarado", node.line());
        }

        std::vector<std::string> paramTypes;
        for (const auto &param : *method.params)
        {
            paramTypes.push_back(param.type);
        }

        symbolTable.addTypeMethod(node.name, method.name, method.returnType, paramTypes);

        if (!typeSym->parentType.empty())
        {
            TypeSymbol *parentSym = symbolTable.lookupType(typeSym->parentType);
            if (parentSym)
            {
                auto it = parentSym->methods.find(method.name);
                if (it != parentSym->methods.end())
                {
                    const Symbol &inherited = it->second;
                    if (inherited.params != paramTypes || inherited.type != method.returnType)
                    {
                        errors.emplace_back("[SEMANTIC ERROR] Firma de método '" + method.name +
                                                "' no coincide con la del padre",
                                            node.line());
                    }
                }
            }
        }

        symbolTable.exitScope();
    }
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
    node.object->accept(*this);
    std::string objectType = node.object->type();

    TypeSymbol *typeSym = symbolTable.lookupType(objectType);
    if (!typeSym)
    {
        errors.emplace_back("[SEMANTIC ERROR] Tipo '" + objectType + "' no definido para llamada a método", node.line());
        node._type = "Error";
        return;
    }

    auto it = typeSym->methods.find(node.methodName);
    if (it == typeSym->methods.end())
    {
        errors.emplace_back("[SEMANTIC ERROR] Método '" + node.methodName + "' no existe en tipo '" + objectType + "'", node.line());
        node._type = "Error";
        return;
    }

    Symbol &method = it->second;
    if (node.args.size() != method.params.size())
    {
        errors.emplace_back("[SEMANTIC ERROR] Cantidad incorrecta de argumentos en llamada a método '" + node.methodName + "'", node.line());
        node._type = "Error";
        return;
    }

    for (size_t i = 0; i < node.args.size(); ++i)
    {
        node.args[i]->accept(*this);
        if (node.args[i]->type() != method.params[i])
        {
            errors.emplace_back("[SEMANTIC ERROR] Tipo de argumento " + std::to_string(i + 1) + " incorrecto en llamada a método '" + node.methodName + "'", node.line());
        }
    }

    node._type = method.type;
}