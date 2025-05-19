#include "SemanticAnalyzer.hpp"
#include "../ast/AST.hpp"
#include "../utils/F_Collector.hpp"
#include <cctype>
#include <set>
#include <iostream>

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

void SemanticAnalyzer::visit(LiteralNode &node)
{
    node._type = node._type; // Mantiene el tipo del literal
}

void SemanticAnalyzer::visit(IdentifierNode &node)
{
    Symbol *symbol = symbolTable.lookup(node.name);
    if (!symbol)
    {
        errors.emplace_back("Variable '" + node.name + "' no declarada", node.line());
        node._type = "Error";
    }
    else
    {
        node._type = symbol->type;
    }
}

void SemanticAnalyzer::visit(VariableDeclarationNode &node)
{
    if (symbolTable.existsInCurrentScope(node.name))
    {
        errors.emplace_back("Variable '" + node.name + "' ya declarada", node.line());
        return;
    }

    if (node.initializer)
    {
        node.initializer->accept(*this);
        std::string initType = node.initializer->type();

        if (!node.declaredType.empty() && node.declaredType != initType)
        {
            errors.emplace_back("Tipo declarado no coincide con inicializador", node.line());
        }

        node._type = !node.declaredType.empty() ? node.declaredType : initType;
    }
    else
    {
        if (node.declaredType.empty())
        {
            errors.emplace_back("Variable sin tipo ni inicializador", node.line());
            return;
        }
        node._type = node.declaredType;
    }

    symbolTable.addSymbol(node.name, node._type, !node.isMutable);
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
    node.left->accept(*this);
    node.right->accept(*this);

    std::string leftType = node.left->type();
    std::string rightType = node.right->type();

    if (node.op == "+" || node.op == "-" || node.op == "*" || node.op == "/")
    {
        if (leftType != "Number" || rightType != "Number")
        {
            errors.emplace_back("Operandos deben ser números", node.line());
            node._type = "Error";
        }
        else
        {
            node._type = "Number";
        }
    }
    else if (node.op == "==" || node.op == "!=")
    {
        node._type = "Boolean";
    }
    else
    {
        errors.emplace_back("Operador no soportado", node.line());
        node._type = "Error";
    }
}

void SemanticAnalyzer::visit(IfNode &node)
{
    for (auto &branch : *node.branches)
    {
        branch.condition->accept(*this);
        if (branch.condition->type() != "Boolean")
        {
            errors.emplace_back("La condición debe ser booleana", branch.condition->line());
        }
        branch.body->accept(*this);
    }

    if (node.elseBody)
    {
        node.elseBody->accept(*this);
    }

    // Versión inicial sin chequeo de tipos consistentes en ramas
    node._type = "Void";
}

void SemanticAnalyzer::visit(WhileNode &node)
{
    node.condition->accept(*this);
    if (node.condition->type() != "Boolean")
    {
        errors.emplace_back("La condición del while debe ser booleana", node.line());
    }

    node.body->accept(*this);
    node._type = "Void"; // Versión inicial simplificada
}

bool SemanticAnalyzer::conformsTo(const std::string &subtype, const std::string &supertype)
{
    if (subtype == "Error" || supertype == "Error")
        return false;
    if (subtype == supertype)
        return true;
    if (supertype == "Object")
        return true;
    if (supertype.empty())
        return true; // Permite compatibilidad con tipos no especificados

    // Ahora recorre toda la jerarquía
    TypeSymbol *sub = symbolTable.lookupType(subtype);
    while (sub && !sub->parentType.empty())
    {
        if (sub->parentType == supertype)
            return true;
        sub = symbolTable.lookupType(sub->parentType);
    }

    return false;
}

void SemanticAnalyzer::visit(BlockNode &node)
{
    symbolTable.enterScope();

    std::string lastType = "Void";
    for (auto expr : node.expressions)
    {
        expr->accept(*this);
        lastType = expr->type();
    }

    symbolTable.exitScope();
    node._type = lastType; // Tipo del bloque es el de su última expresión
}

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

SymbolTable &SemanticAnalyzer::getSymbolTable()
{
    return symbolTable;
}