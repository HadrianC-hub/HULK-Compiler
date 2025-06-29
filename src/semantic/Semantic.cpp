#include "Semantic.hpp"
#include "../ast/AST.hpp"
#include "scope/Function.hpp"
#include "scope/TypeCollector.hpp"
#include <cctype>
#include <set>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <map>

SymbolTable &SemanticValidation::getSymbolTable()
{
    return symbolTable;
}

std::string SemanticValidation::inferParamUsageType(const std::string &paramName, ASTNode *body)
{
    std::set<std::string> usageTypes;
    collectParamUsages(body, paramName, usageTypes);

    if (usageTypes.empty())
        return "Unknown";

    // Si solo hay un tipo, ese es el tipo mas especifico
    if (usageTypes.size() == 1)
    {
        return *usageTypes.begin();
    }

    // Si hay multiples tipos, necesitamos encontrar el mas especifico comun
    std::vector<std::string> types(usageTypes.begin(), usageTypes.end());

    // Si todos los tipos son Unknown, asumimos Object
    bool allUnknown = true;
    for (const auto &type : types)
    {
        if (type != "Unknown")
        {
            allUnknown = false;
            break;
        }
    }
    if (allUnknown)
    {
        return "Object";
    }

    // Filtrar tipos Unknown
    types.erase(
        std::remove_if(types.begin(), types.end(),
                       [](const std::string &t)
                       { return t == "Unknown"; }),
        types.end());

    // Si despues de filtrar Unknown no quedan tipos, asumimos Number
    if (types.empty())
    {
        return "Object";
    }

    std::string commonType = symbolTable.lowestCommonAncestor(types);

    // Verificar si hay ambigüedad en la jerarquia de tipos
    bool hasAmbiguity = false;
    for (const auto &type : types)
    {
        if (type != commonType && !symbolTable.isSubtype(type, commonType))
        {
            hasAmbiguity = true;
            break;
        }
    }

    // Si hay ambigüedad, intentamos ser mas especificos
    if (hasAmbiguity)
    {
        return "Object"; // Si no podemos ser mas especificos, usamos Object
    }

    return commonType;
}

void SemanticValidation::collectParamUsages(ASTNode *node, const std::string &paramName, std::set<std::string> &types)
{
    if (!node)
        return;

    // Identificador que referencia el parametro
    if (auto *id = dynamic_cast<VarFuncName *>(node))
    {
        if (id->name == paramName && id->type() != "Unknown" && id->type() != "Error")
        {
            types.insert(id->type());
        }
    }

    // Binary operation - verificar restricciones de tipo
    else if (auto *bin = dynamic_cast<BinaryOperation *>(node))
    {
        collectParamUsages(bin->left, paramName, types);
        collectParamUsages(bin->right, paramName, types);

        // Verificar restricciones de tipo para operaciones especificas
        if (bin->op == "+" || bin->op == "-" || bin->op == "*" || bin->op == "/" || bin->op == "^" || bin->op == "%")
        {
            // Operaciones aritmeticas requieren Number
            auto *leftId = dynamic_cast<VarFuncName *>(bin->left);
            auto *rightId = dynamic_cast<VarFuncName *>(bin->right);
            if ((leftId && leftId->name == paramName) || (rightId && rightId->name == paramName))
            {
                types.insert("Number");
            }
        }
        else if (bin->op == ">" || bin->op == "<" || bin->op == ">=" || bin->op == "<=")
        {
            // Operaciones de comparacion requieren Number
            auto *leftId = dynamic_cast<VarFuncName *>(bin->left);
            auto *rightId = dynamic_cast<VarFuncName *>(bin->right);
            if ((leftId && leftId->name == paramName) || (rightId && rightId->name == paramName))
            {
                types.insert("Number");
            }
        }
        else if (bin->op == "&" || bin->op == "|")
        {
            // Operaciones logicas requieren Boolean
            auto *leftId = dynamic_cast<VarFuncName *>(bin->left);
            auto *rightId = dynamic_cast<VarFuncName *>(bin->right);
            if ((leftId && leftId->name == paramName) || (rightId && rightId->name == paramName))
            {
                types.insert("Boolean");
            }
        }
        else if (bin->op == "@" || bin->op == "@@")
        {
            // Operaciones de concatenacion requieren String o Number
            auto *leftId = dynamic_cast<VarFuncName *>(bin->left);
            auto *rightId = dynamic_cast<VarFuncName *>(bin->right);

            if (leftId && leftId->name == paramName)
            {
                // Si el otro operando es un literal, usamos su tipo
                if (auto *rightLit = dynamic_cast<DataType *>(bin->right))
                {
                    types.insert(rightLit->type());
                }
                else
                {
                    // Si no podemos inferir el tipo del otro operando,
                    // permitimos tanto String como Number
                    types.insert("String");
                    types.insert("Number");
                }
            }
            if (rightId && rightId->name == paramName)
            {
                // Si el otro operando es un literal, usamos su tipo
                if (auto *leftLit = dynamic_cast<DataType *>(bin->left))
                {
                    types.insert(leftLit->type());
                }
                else
                {
                    // Si no podemos inferir el tipo del otro operando,
                    // permitimos tanto String como Number
                    types.insert("String");
                    types.insert("Number");
                }
            }
        }
        else if (bin->op == "==" || bin->op == "!=")
        {
            // Operaciones de comparacion requieren tipos compatibles
            auto *leftId = dynamic_cast<VarFuncName *>(bin->left);
            auto *rightId = dynamic_cast<VarFuncName *>(bin->right);
            if (leftId && leftId->name == paramName)
            {
                types.insert(bin->right->type());
            }
            if (rightId && rightId->name == paramName)
            {
                types.insert(bin->left->type());
            }
        }
    }
    // Unary operation
    else if (auto *un = dynamic_cast<UnaryOperation *>(node))
    {
        collectParamUsages(un->operand, paramName, types);
        auto *operandId = dynamic_cast<VarFuncName *>(un->operand);

        if (operandId && operandId->name == paramName)
        {
            if (un->op == "!")
            {
                types.insert("Boolean");
            }
        }
    }

    // Function call
    else if (auto *call = dynamic_cast<FuncCall *>(node))
    {
        for (auto *arg : call->args)
            collectParamUsages(arg, paramName, types);

        // Si el parametro es uno de los argumentos, inferir del tipo esperado por la funcion
        Symbol *funcSym = symbolTable.lookup(call->funcName);
        if (funcSym && funcSym->kind == "function")
        {
            for (size_t i = 0; i < call->args.size(); ++i)
            {
                if (auto *id = dynamic_cast<VarFuncName *>(call->args[i]))
                {
                    if (id->name == paramName)
                    {
                        // Si el tipo del parametro de la funcion es Unknown, intentar inferirlo del cuerpo
                        if (funcSym->params[i] == "Unknown" && funcSym->body)
                        {
                            std::set<std::string> paramTypes;
                            collectParamUsages(funcSym->body, funcSym->params[i], paramTypes);
                            if (!paramTypes.empty())
                            {
                                types.insert(*paramTypes.begin());
                            }
                            else
                            {
                                // Si no podemos inferir del cuerpo, intentar inferir del tipo de retorno
                                if (funcSym->type != "Unknown")
                                {
                                    types.insert(funcSym->type);
                                }
                            }
                        }
                        else
                        {
                            types.insert(funcSym->params[i]);
                        }
                    }
                }
            }
        }
    }

    // Built-in function call
    else if (auto *builtin = dynamic_cast<BuiltInFunc *>(node))
    {
        if (builtin->name == "print")
        {
            // print acepta cualquier tipo, así que agregamos Object
            for (auto *arg : builtin->args)
            {
                if (auto *id = dynamic_cast<VarFuncName *>(arg))
                {
                    if (id->name == paramName)
                    {
                        types.insert("Object");
                    }
                }
            }
        }
        for (auto *arg : builtin->args)
        {
            collectParamUsages(arg, paramName, types);

            // Si este argumento es el parametro que estamos inferiendo y la funcion requiere Number, inferir Number
            if (auto *id = dynamic_cast<VarFuncName *>(arg))
            {
                if (id->name == paramName)
                {
                    static const std::set<std::string> numericBuiltins = {
                        "sin", "cos", "exp", "sqrt", "log", "min", "max"};
                    if (numericBuiltins.count(builtin->name))
                    {
                        types.insert("Number");
                    }
                }
            }
        }
    }

    // Method call
    else if (auto *method = dynamic_cast<MethodCall *>(node))
    {
        // Analizar la expresión de instancia
        collectParamUsages(method->instance, paramName, types);

        // Analizar todos los argumentos de la llamada al metodo
        for (auto *arg : method->args)
        {
            collectParamUsages(arg, paramName, types);
        }

        // // Obtener el tipo del objeto (instanceName se refiere al nombre de la instancia)
        // std::string objType;

        // // Buscamos el simbolo de la instancia para obtener su tipo
        // Symbol *instanceSym = symbolTable.lookup(method->instanceName);
        // if (!instanceSym)
        //     return; // si no se encuentra, abortamos esta rama

        // objType = instanceSym->type;
        // TypeSymbol *typeSym = symbolTable.lookupType(objType);

        // // Verificar herencia si es necesario
        // while (typeSym)
        // {
        //     auto it = typeSym->methods.find(method->methodName);
        //     if (it != typeSym->methods.end())
        //     {
        //         const Symbol &methodSym = it->second;

        //         for (size_t i = 0; i < method->args.size(); ++i)
        //         {
        //             if (auto *id = dynamic_cast<VarFuncName *>(method->args[i]))
        //             {
        //                 if (id->name == paramName && i < methodSym.params.size())
        //                 {
        //                     types.insert(methodSym.params[i]);
        //                 }
        //             }
        //         }

        //         break; // metodo encontrado, no seguimos subiendo en herencia
        //     }

        //     if (typeSym->parentType.empty())
        //         break;
        //     typeSym = symbolTable.lookupType(typeSym->parentType);
        // }
    }

    // Let expression
    else if (auto *let = dynamic_cast<LetExpression *>(node))
    {
        // Primero analizar las declaraciones
        for (auto &decl : *let->declarations)
        {
            collectParamUsages(decl.initializer, paramName, types);

            // Si el parametro es usado en el inicializador, su tipo debe ser compatible
            if (auto *id = dynamic_cast<VarFuncName *>(decl.initializer))
            {
                if (id->name == paramName)
                {
                    types.insert(decl.declaredType);
                }
            }
        }
        collectParamUsages(let->body, paramName, types);
    }

    // If expression
    else if (auto *ifn = dynamic_cast<IfExpression *>(node))
    {
        // Analizar condicion
        collectParamUsages(ifn->branches->front().condition, paramName, types);

        // Analizar todas las ramas
        for (auto &branch : *ifn->branches)
        {
            collectParamUsages(branch.body, paramName, types);
        }

        // Analizar else si existe
        if (ifn->elseBody)
        {
            collectParamUsages(ifn->elseBody, paramName, types);
        }
    }

    // While expression
    else if (auto *wh = dynamic_cast<WhileLoop *>(node))
    {
        collectParamUsages(wh->condition, paramName, types);
        collectParamUsages(wh->body, paramName, types);
    }

    // For expression
    else if (auto *forNode = dynamic_cast<ForLoop *>(node))
    {
        collectParamUsages(forNode->init_range, paramName, types);
        collectParamUsages(forNode->end_range, paramName, types);
        collectParamUsages(forNode->body, paramName, types);
    }

    // Assignment
    else if (auto *assign = dynamic_cast<Assignment *>(node))
    {
        collectParamUsages(assign->rhs, paramName, types);

        if (auto *id = dynamic_cast<VarFuncName *>(assign->rhs))
        {
            if (id->name == paramName)
            {
                std::string targetName;

                if (auto *idName = dynamic_cast<VarFuncName *>(assign->name))
                {
                    targetName = idName->name;
                }
                else if (auto *selfField = dynamic_cast<SelfCall *>(assign->name))
                {
                    targetName = selfField->varName;
                }

                if (!targetName.empty())
                {
                    Symbol *varSym = symbolTable.lookup(targetName);
                    if (varSym)
                    {
                        types.insert(varSym->type);
                    }
                }
            }
        }
    }

    // Variable declaration
    else if (auto *decl = dynamic_cast<VarDeclaration *>(node))
    {
        if (decl->initializer)
        {
            collectParamUsages(decl->initializer, paramName, types);

            // Si el parametro es usado en el inicializador, su tipo debe ser compatible
            if (auto *id = dynamic_cast<VarFuncName *>(decl->initializer))
            {
                if (id->name == paramName)
                {
                    types.insert(decl->declaredType);
                }
            }
        }
    }

    // Block
    else if (auto *block = dynamic_cast<Block *>(node))
    {
        for (auto *expr : block->expressions)
            collectParamUsages(expr, paramName, types);
    }

    // New instance
    else if (auto *inst = dynamic_cast<InitInstance *>(node))
    {
        for (auto *arg : inst->args)
            collectParamUsages(arg, paramName, types);

        // Si el parametro es uno de los argumentos, inferir del tipo esperado por el constructor
        TypeSymbol *typeSym = symbolTable.lookupType(inst->typeName);
        if (typeSym)
        {
            for (size_t i = 0; i < inst->args.size(); ++i)
            {
                if (auto *id = dynamic_cast<VarFuncName *>(inst->args[i]))
                {
                    if (id->name == paramName && i < typeSym->typeParamNames.size())
                    {
                        types.insert(typeSym->typeParamNames[i]);
                    }
                }
            }
        }
    }

    // Type declaration
    else if (auto *typeDecl = dynamic_cast<TypeDeclaration *>(node))
    {
        // Recolectar tipos desde atributos
        if (typeDecl->body && typeDecl->body->attributes)
        {
            for (const auto &attr : *typeDecl->body->attributes)
            {
                collectParamUsages(attr.initializer, paramName, types);
            }
        }

        // Recolectar tipos desde cuerpos de metodos
        if (typeDecl->body && typeDecl->body->methods)
        {
            for (const auto &method : *typeDecl->body->methods)
            {
                collectParamUsages(method.body, paramName, types);
            }
        }

        // Argumentos al padre (base constructor args)
        for (ASTNode *arg : typeDecl->baseArgs)
        {
            collectParamUsages(arg, paramName, types);
        }
    }

    // Literal: no hay nada que recorrer
    else if (dynamic_cast<DataType *>(node))
    {
        return;
    }

    // Otro nodo no manejado explicitamente
    else
    {
        // Opcional: log de depuracion
        // std::cerr << "Nodo no manejado en collectParamUsages: " << typeid(*node).name() << "\n";
    }
}

bool SemanticValidation::conformsTo(const std::string &subtype, const std::string &supertype)
{
    if (subtype == "Null")
    {
        return supertype != "Number" && supertype != "String" && supertype != "Boolean";
    }
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

    // Permitir asignar Null a cualquier tipo de objeto
    if (subtype == "Null")
    {
        return true;
    }

    // Manejar referencias a tipos no declarados aún
    if (!symbolTable.lookupType(subtype) || !symbolTable.lookupType(supertype))
    {
        return true; // Posponer validación
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

bool SemanticValidation::checkInheritanceCycles()
{
    bool hasCycle = false;
    std::cout << "=== Iniciando verificacion de ciclos de herencia ===\n";
    std::map<std::string, int> color;   // 0=blanco, 1=gris, 2=negro
    std::map<std::string, int> lineMap; // Mapa de tipo -> línea

    // Inicializar colores y recolectar líneas
    for (auto &[name, type] : symbolTable.types)
    {
        color[name] = 0;
        lineMap[name] = type.line;
        std::cout << "Tipo registrado: " << name << " (padre: " << type.parentType << ") en linea " << type.line << "\n";
    }

    for (auto &[name, type] : symbolTable.types)
    {
        if (color[name] == 0)
        {
            std::cout << "Iniciando DFS para tipo: " << name << "\n";
            std::vector<std::string> path;
            if (dfsInheritance(name, color, path, lineMap))
            {
                std::cout << "Ciclo detectado comenzando desde: " << name << "\n";
                hasCycle = true;
            }
        }
    }
    std::cout << "=== Finalizada verificacion de ciclos de herencia ===\n";
    return hasCycle;
}

bool SemanticValidation::dfsInheritance(const std::string &typeName,
                                        std::map<std::string, int> &color,
                                        std::vector<std::string> &path,
                                        std::map<std::string, int> &lineMap)
{
    std::cout << "  Visitando: " << typeName << " (color: " << color[typeName] << ")\n";

    // Si encontramos un nodo gris, está en el camino actual -> ciclo
    if (color[typeName] == 1)
    {
        // Construir la ruta del ciclo
        std::string cyclePath;
        bool inCycle = false;
        for (const auto &t : path)
        {
            if (t == typeName)
                inCycle = true;
            if (inCycle)
            {
                cyclePath += t + " -> ";
            }
        }
        cyclePath += typeName; // Completar el ciclo

        int line = lineMap[typeName];
        errors.emplace_back("Ciclo de herencia detectado: " + cyclePath, line);
        std::cerr << "[ERROR] Ciclo de herencia: " << cyclePath << " en linea " << line << "\n";
        return true;
    }

    color[typeName] = 1; // Gris (en proceso)
    path.push_back(typeName);

    TypeSymbol *type = symbolTable.lookupType(typeName);
    if (!type)
    {
        std::cerr << "  [ERROR] Tipo no encontrado en DFS: " << typeName << "\n";
        path.pop_back();
        color[typeName] = 0; // Restaurar a blanco
        errors.emplace_back("Tipo '" + typeName + "' no encontrado durante verificacion de herencia", lineMap[typeName]);
        return false;
    }

    bool cycleFound = false;
    if (!type->parentType.empty())
    {
        std::cout << "  " << typeName << " tiene padre: " << type->parentType << "\n";

        // Verificar si el padre existe
        if (!symbolTable.lookupType(type->parentType))
        {
            std::cerr << "  [ERROR] Padre no encontrado: " << type->parentType << "\n";
            errors.emplace_back("Tipo padre '" + type->parentType + "' no encontrado para '" + typeName + "'", type->line);
        }
        else
        {
            cycleFound = dfsInheritance(type->parentType, color, path, lineMap);
        }
    }

    path.pop_back();
    color[typeName] = 2; // Negro (completado)

    return cycleFound;
}

void SemanticValidation::validate(const std::vector<ASTNode *> &nodes)
{
    std::cout << "Entra en analyze." << std::endl;

    // Agregar funciones predeterminadas de Hulk
    FunctionCollector collector(symbolTable, errors);
    collector.addBuiltins();
    std::cout << "Builtins agregados." << std::endl;

    // Recolectar tipos primero
    TypeCollector tcollector(symbolTable, errors);
    for (ASTNode *node : nodes)
    {
        if (auto *typeDecl = dynamic_cast<TypeDeclaration *>(node))
        {
            typeDecl->accept(tcollector);
        }
    }

    // Verificar ciclos de herencia (agrega errores si hay)
    checkInheritanceCycles();

    // Si hay errores, saltar el resto del análisis
    if (!errors.empty()) {
        goto report_errors; // Saltar directamente a reporte de errores
    }

    // Echar a andar un recolector de funciones
    for (ASTNode *node : nodes)
    {
        if (!node)
        {
            std::cerr << "Nodo nulo en AST." << std::endl;
            continue;
        }

        std::cout << "Recolectando funciones para nodo tipo: " << typeid(*node).name() << std::endl;
        // Por cada nodo, se recolectan las funciones definidas (accept llama a visit)
        node->accept(collector); // Aceptar el colector de funciones, que es un tipo de analizador semántico
    }

    std::cout << "Fase de recoleccion completada." << std::endl;

    // Echar a andar análisis semántico
    for (ASTNode *node : nodes)
    {
        try
        {
            std::cout << "Analisis semantico de: " << typeid(*node).name() << std::endl;
            node->accept(*this); // Aceptar esta instancia de analizador semántico
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error durante analisis semantico: " << e.what() << std::endl;
        }
    }

    // Mostrar errores encontrados
    if (!errors.empty())
    {
        report_errors:
        std::cerr << "Errores semanticos encontrados:\n";
        for (const auto &e : errors)
        {
            std::cerr << "- Linea " << e.line << ": " << e.message << "\n";
        }
    }
    else
    {
        std::cout << "No se encontraron errores semanticos.\n";
    }

    // En caso de errores, se detiene la ejecución y no se lleva a IR
    if (!errors.empty())
    {
        std::cerr << "Errores semanticos presentes. Abortando ejecucion.\n";
        exit(1);
    }

    std::cout << "Analisis semantico completado." << std::endl;
}

void SemanticValidation::visit(ASTNode &node) {}

// Implementacion de visitas a nodos

void SemanticValidation::visit(UnaryOperation &node)
{
    node.operand->accept(*this);
    std::string operandType = node.operand->type();

    if (node.op == "-")
    {
        if (operandType != "Number")
        {
            errors.emplace_back("El operador '-' requiere un operando de tipo Number", node.line());
            node._type = "Error";
        }
        node._type = "Number";
    }
    else if (node.op == "!")
    {
        // Para negacion logica, si el operando es Unknown, asumimos Boolean
        if (operandType == "Unknown")
            operandType = "Boolean";

        if (operandType != "Boolean")
        {
            errors.emplace_back("El operador '!' requiere un operando de tipo Boolean", node.line());
            node._type = "Error";
        }
        else
        {
            node._type = "Boolean";
        }
    }
    else
    {
        errors.emplace_back("Operador unario desconocido: " + node.op, node.line());
        node._type = "Error";
    }
}

void SemanticValidation::visit(BuiltInFunc &node)
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
            errors.emplace_back("La funcion 'print' requiere exactamente 1 argumento", node.line());
            node._type = "Error";
            return;
        }
        node._type = node.args[0]->type(); // El tipo es el del argumento impreso
    }
    else if (fn == "sin" || fn == "cos" || fn == "exp" || fn == "sqrt")
    {
        if (node.args.size() != 1)
        {
            errors.emplace_back("Funcion " + fn + " requiere 1 argumento", node.line());
            node._type = "Error";
        }
        if (node.args[0]->type() == "Unknown")
        {
            node.args[0]->type() = "Number";
        }

        else if (node.args[0]->type() != "Number")
        {
            errors.emplace_back("El argumento de '" + fn + "' debe ser un numero", node.line());
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
            errors.emplace_back("La funcion 'log' requiere 2 argumentos", node.line());
            node._type = "Error";
        }
        if (node.args[0]->type() == "Unknown")
        {
            node.args[0]->type() = "Number";
        }
        if (node.args[1]->type() == "Unknown")
            node.args[1]->type() = "Number";

        else if (node.args[0]->type() != "Number" || node.args[1]->type() != "Number")
        {
            errors.emplace_back("Los argumentos de 'log' deben ser numericos", node.line());
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
            errors.emplace_back("La funcion 'rand' no acepta argumentos", node.line());
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
            errors.emplace_back("La funcion '" + fn + "' requiere 2 argumentos", node.line());
            node._type = "Error";
        }
        if (node.args[0]->type() == "Unknown")
        {
            node.args[0]->type() = "Number";
        }
        if (node.args[1]->type() == "Unknown")
            node.args[1]->type() = "Number";
        else if (node.args[0]->type() != "Number" || node.args[1]->type() != "Number")
        {
            errors.emplace_back("Los argumentos de '" + fn + "' deben ser numericos", node.line());
            node._type = "Error";
        }
        else
        {
            node._type = "Number";
        }
    }
    else
    {
        errors.emplace_back("Funcion builtin '" + fn + "' no reconocida", node.line());
        node._type = "Error";
    }
}

void SemanticValidation::visit(FuncDeclaration &node)
{
    std::cout << "\n=== Iniciando analisis de funcion: " << node.name << " ===\n";
    std::vector<std::string> paramTypes;
    for (const auto &param : *node.params)
        paramTypes.push_back(param.type.empty() ? "Unknown" : param.type);

    symbolTable.addFunction(node.name, "Unknown", paramTypes);

    symbolTable.enterScope();

    std::map<std::string, bool> paramSeen;

    std::cout << "Paso 1: Registrando parametros\n";
    for (const auto &param : *node.params)
    {
        if (paramSeen.count(param.name))
        {
            errors.emplace_back("Parametro duplicado '" + param.name + "'", node.line());
            node._type = "Error";
            continue;
        }

        paramSeen[param.name] = true;
        std::string paramType = param.type.empty() ? "Unknown" : param.type;
        std::cout << "  - Parametro: " << param.name << " con tipo inicial: " << paramType << "\n";
        symbolTable.addSymbol(param.name, paramType, false);
    }

    std::cout << "Paso 2: Analizando cuerpo de la funcion\n";
    node.body->accept(*this);
    std::string bodyType = node.body->type();
    std::cout << "  - Tipo del cuerpo: " << bodyType << "\n";

    std::cout << "DEBUG: Iniciando inferencia de tipos para parametros\n";
    for (auto &param : *node.params)
    {
        if (!param.type.empty())
        {
            continue;
        }

        std::cout << "  - Intentando inferir tipo para parametro: " << param.name << "\n";

        if (auto *call = dynamic_cast<FuncCall *>(node.body))
        {
            Symbol *funcSym = symbolTable.lookup(call->funcName);
            if (funcSym && funcSym->kind == "function")
            {
                for (size_t i = 0; i < call->args.size(); ++i)
                {
                    if (auto *id = dynamic_cast<VarFuncName *>(call->args[i]))
                    {
                        if (id->name == param.name)
                        {
                            std::string expectedType = funcSym->params[i];
                            if (expectedType != "Unknown")
                            {
                                param.type = expectedType;
                                symbolTable.updateSymbolType(param.name, expectedType);
                                std::cout << "    - Tipo inferido de llamada a funcion: " << expectedType << "\n";
                                continue;
                            }
                        }
                    }
                }
            }
        }

        if (auto *bin = dynamic_cast<BinaryOperation *>(node.body))
        {
            std::cout << "    - Cuerpo es una operacion binaria con operador: " << bin->op << "\n";
            if (bin->op == "+" || bin->op == "-" || bin->op == "*" || bin->op == "/")
            {
                auto *leftId = dynamic_cast<VarFuncName *>(bin->left);
                auto *rightId = dynamic_cast<VarFuncName *>(bin->right);

                std::cout << "    - Verificando operandos:\n";
                if (leftId)
                    std::cout << "      - Izquierdo: " << leftId->name << "\n";
                if (rightId)
                    std::cout << "      - Derecho: " << rightId->name << "\n";

                if ((leftId && leftId->name == param.name) ||
                    (rightId && rightId->name == param.name))
                {
                    std::cout << "    - Parametro usado en operacion aritmetica, asignando tipo Number\n";
                    param.type = "Number";
                    symbolTable.updateSymbolType(param.name, "Number");
                    continue;
                }
            }
            else if (bin->op == "@" || bin->op == "@@")
            {
                auto *leftId = dynamic_cast<VarFuncName *>(bin->left);
                auto *rightId = dynamic_cast<VarFuncName *>(bin->right);

                if ((leftId && leftId->name == param.name) ||
                    (rightId && rightId->name == param.name))
                {
                    // Si el otro operando es un literal, usar su tipo
                    if (auto *otherLit = dynamic_cast<DataType *>(leftId ? bin->right : bin->left))
                    {
                        param.type = otherLit->type();
                    }
                    else
                    {
                        // Si no podemos inferir el tipo del otro operando,
                        // permitir tanto String como Number
                        param.type = "String";
                    }
                    symbolTable.updateSymbolType(param.name, param.type);
                    std::cout << "    - Parametro usado en concatenacion, asignando tipo: " << param.type << "\n";
                    continue;
                }
            }
        }

        std::cout << "    - Usando metodo general de inferencia\n";
        std::string inferredType = inferParamUsageType(param.name, node.body);
        std::cout << "    - Tipo inferido: " << inferredType << "\n";

        if (inferredType == "Unknown")
        {
            // // Si no se pudo inferir, intentar inferir del tipo de retorno
            // if (bodyType != "Unknown" && bodyType != "Error")
            // {
            //     inferredType = bodyType;
            //     std::cout << "    - Usando tipo de retorno como tipo inferido: " << inferredType << "\n";
            // }
            // else
            // {
            //     errors.emplace_back("No se pudo inferir el tipo del parametro '" + param.name + "'", node.line());
            //     node._type = "Error";
            //     continue;
            // }
            inferredType = "Object";
        }

        param.type = inferredType;
        symbolTable.updateSymbolType(param.name, inferredType);
    }

    // DESPUÉS DE INFERIR LOS PARÁMETROS
    std::cout << "Paso 3: Re-analizando cuerpo con tipos inferidos\n";
    symbolTable.enterScope(); // Re-abrir el mismo ámbito

    // Volver a registrar parámetros con tipos actualizados
    for (const auto &param : *node.params)
    {
        symbolTable.addSymbol(param.name, param.type, false);
    }

    // Re-analizar el cuerpo
    node.body->accept(*this);
    bodyType = node.body->type();
    std::cout << "  - Tipo del cuerpo (actualizado): " << bodyType << "\n";

    symbolTable.exitScope();

    // Actualizar parámetros en el símbolo de la función (¡CRUCIAL!)
    Symbol *funcSym = symbolTable.lookup(node.name);
    if (funcSym && funcSym->kind == "function")
    {
        funcSym->params.clear();
        for (const auto &param : *node.params)
        {
            funcSym->params.push_back(param.type);
        }
        // Depuración: ver tipos actualizados
        std::cout << "Parametros actualizados de " << node.name << ": ";
        for (const auto &t : funcSym->params)
            std::cout << t << " ";
        std::cout << "\n";
    }

    std::cout << "Paso 4: Verificando tipo de retorno\n";
    if (!node.returnType.empty() && !conformsTo(bodyType, node.returnType))
    {
        errors.emplace_back("Tipo de retorno incorrecto en funcion '" + node.name + "'", node.line());
        node._type = "Error";
    }

    node._type = node.returnType.empty() ? bodyType : node.returnType;
    std::cout << "  - Tipo final de la funcion: " << node._type << "\n";

    // Symbol *funcSym = symbolTable.lookup(node.name);
    if (funcSym)
    {
        funcSym->type = node._type;
    }

    std::cout << "=== Fin del analisis de funcion ===\n\n";

    symbolTable.exitScope();
}

void SemanticValidation::visit(FuncCall &node)
{
    if (node.funcName == "base")
    {
        Symbol *self = symbolTable.lookup("self");
        if (!self)
        {
            errors.emplace_back("'base' solo puede usarse dentro de metodos", node.line());
            node._type = "Error";
            return;
        }

        TypeSymbol *typeSym = symbolTable.lookupType(self->type);
        if (!typeSym || typeSym->parentType == "Object")
        {
            errors.emplace_back("'base' no disponible para este tipo", node.line());
            node._type = "Error";
            return;
        }

        node._type = typeSym->parentType;
        return;
    }

    if (node.funcName == "print")
    {
        for (auto arg : node.args)
        {
            arg->accept(*this);
        }
        node._type = "void";
        return;
    }

    Symbol *symbol = symbolTable.lookup(node.funcName);
    if (!symbol || symbol->kind != "function")
    {
        errors.emplace_back("Funcion '" + node.funcName + "' no definida", node.line());
        node._type = "Error";
        return;
    }

    if (node.args.size() != symbol->params.size())
    {
        errors.emplace_back("Numero incorrecto de argumentos para '" + node.funcName + "'", node.line());
        node._type = "Error";
        return;
    }

    for (size_t i = 0; i < node.args.size(); ++i)
    {
        node.args[i]->accept(*this);
        std::string argType = node.args[i]->type();
        std::string expectedType = symbol->params[i];

        if (expectedType == "Unknown" && symbol->body)
        {
            std::set<std::string> paramTypes;
            collectParamUsages(symbol->body, symbol->params[i], paramTypes);
            if (!paramTypes.empty())
            {
                expectedType = *paramTypes.begin();
                symbol->params[i] = expectedType;
            }
        }

        // Si el tipo del argumento es Unknown, intentar inferirlo del tipo esperado
        if (argType == "Unknown" && expectedType != "Unknown")
        {
            argType = expectedType;
            node.args[i]->type() = expectedType;
        }

        // Si ambos tipos son Unknown, intentar inferir del uso en el cuerpo
        // if (argType == "Unknown" && expectedType == "Unknown" && symbol->body)
        // {
        //     std::set<std::string> paramTypes;
        //     collectParamUsages(symbol->body, symbol->params[i], paramTypes);
        //     if (!paramTypes.empty())
        //     {
        //         expectedType = *paramTypes.begin();
        //         symbol->params[i] = expectedType;
        //         argType = expectedType;
        //         node.args[i]->type() = expectedType;
        //     }
        // }

        // Verificar compatibilidad de tipos
        if (!conformsTo(argType, expectedType))
        {
            // Si el tipo esperado es Unknown, intentar inferir del argumento
            if (expectedType == "Unknown" && argType != "Unknown")
            {
                symbol->params[i] = argType;
            }
            else
            {
                errors.emplace_back("Tipo incorrecto para argumento " + std::to_string(i + 1) +
                                        " en '" + node.funcName + "': esperado '" + expectedType + "', obtenido '" + argType + "'",
                                    node.line());
                node._type = "Error";
                return;
            }
        }
    }

    node._type = symbol->type;

    if (node._type == "Unknown" && symbol->body)
    {
        symbol->body->accept(*this);
        node._type = symbol->body->type();
        symbol->type = node._type;
    }
}

void SemanticValidation::visit(BinaryOperation &node)
{
    node.left->accept(*this);
    node.right->accept(*this);

    std::string leftType = node.left->type();
    std::string rightType = node.right->type();

    const std::set<std::string> comparisonOps = {"==", "!=", "<", ">", "<=", ">="};
    const std::set<std::string> arithmeticOps = {"+", "-", "*", "/", "^", "%"};

    if (comparisonOps.count(node.op))
    {

        if (leftType == "Unknown")
        {
            if (rightType == "Number" || rightType == "String" || rightType == "Boolean")
            {
                leftType = rightType;
            }
        }
        if (rightType == "Unknown")
        {
            if (leftType == "Number" || leftType == "String" || leftType == "Boolean")
            {
                rightType = leftType;
            }
        }

        if (node.op == "==" || node.op == "!=")
        {
            if (!conformsTo(leftType, rightType) && !conformsTo(rightType, leftType))
            {
                errors.emplace_back("Operandos de " + node.op + " deben ser compatibles", node.line());
                node._type = "Error";
            }
            else
            {
                node._type = "Boolean";
            }
        }
        else
        {

            if (leftType == "Unknown")
                leftType = "Number";
            if (rightType == "Unknown")
                rightType = "Number";

            if (leftType != "Number" || rightType != "Number")
            {
                errors.emplace_back("Operandos de " + node.op + " deben ser numeros", node.line());
                node._type = "Error";
            }
            else
            {
                node._type = "Boolean";
            }
        }
    }
    else if (arithmeticOps.count(node.op))
    {

        if (leftType == "Unknown" || leftType == "")
            leftType = "Number";
        if (rightType == "Unknown" || rightType == "")
            rightType = "Number";

        if (leftType != "Number" || rightType != "Number")
        {
            errors.emplace_back("Operandos de " + node.op + " deben ser numeros", node.line());
            node._type = "Error";
        }
        else
        {
            node._type = "Number";
        }
    }
    else if (node.op == "&" || node.op == "|")
    {

        if (leftType == "Unknown")
            leftType = "Boolean";
        if (rightType == "Unknown")
            rightType = "Boolean";

        if (leftType != "Boolean" || rightType != "Boolean")
        {
            errors.emplace_back("Operandos de " + node.op + " deben ser booleanos", node.line());
            node._type = "Error";
        }
        else
        {
            node._type = "Boolean";
        }
    }
    else if (node.op == "@" || node.op == "@@")
    {
        if (leftType == "Unknown" || leftType == "")
        {
            if (rightType == "String" || rightType == "Number")
            {
                leftType = rightType;
            }
            else if (rightType == "Unknown" || rightType == "")
            {
                leftType = "String";
                rightType = "String";
            }
        }
        if (rightType == "Unknown" || rightType == "")
        {
            if (leftType == "String" || leftType == "Number")
            {
                rightType = leftType;
            }
        }

        if ((leftType != "String" && leftType != "Number") ||
            (rightType != "String" && rightType != "Number"))
        {
            errors.emplace_back("Operandos de @ deben ser string o number", node.line());
            node._type = "Error";
        }
        else
        {
            node._type = "String";
        }
    }
    else
    {
        errors.emplace_back("Operador binario desconocido: " + node.op, node.line());
        node._type = "Error";
    }
}

void SemanticValidation::visit(DataType &node)
{
    node._type = node._type;
}

void SemanticValidation::visit(Block &node)
{
    if (node.expressions.empty())
    {
        node._type = "Null";
        return;
    }

    symbolTable.enterScope();

    // Analizar todas las expresiones
    for (auto *expr : node.expressions)
    {
        expr->accept(*this);
    }

    // Obtener la ultima expresion
    ASTNode *lastExpr = node.expressions.back();

    // Si la ultima expresion es una declaracion de funcion o un bloque que termina con una declaracion de funcion
    if (dynamic_cast<FuncDeclaration *>(lastExpr) ||
        (dynamic_cast<Block *>(lastExpr) &&
         !dynamic_cast<Block *>(lastExpr)->expressions.empty() &&
         dynamic_cast<FuncDeclaration *>(dynamic_cast<Block *>(lastExpr)->expressions.back())))
    {
        node._type = "Null";
    }
    else
    {
        // En caso contrario, el tipo del bloque es el tipo de su ultima expresion
        node._type = lastExpr->type();
    }

    // node._type = node.expressions.back()->type();

    symbolTable.exitScope();
}

void SemanticValidation::visit(VarDeclaration &node)
{
    if (symbolTable.existsInCurrentScope(node.name))
    {
        errors.emplace_back("Variable '" + node.name + "' ya declarada", node.line());
        node._type = "Error";
        return;
    }

    if (!node.initializer && node.declaredType.empty())
    {
        errors.emplace_back("Variable '" + node.name + "' sin tipo ni inicializador", node.line());
        node._type = "Error";
        return;
    }

    if (node.initializer)
    {
        node.initializer->accept(*this);
        std::string initType = node.initializer->type();
        if (!node.declaredType.empty() && node.declaredType != initType)
        {
            errors.emplace_back("Tipo declarado no coincide con el inicializador", node.line());
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

void SemanticValidation::visit(VarFuncName &node)
{
    Symbol *symbol = symbolTable.lookup(node.name);
    if (!symbol)
    {
        errors.emplace_back("Variable '" + node.name + "' no declarada", node.line());
        node._type = "Error";
        return;
    }
    node._type = symbol->type;
}

bool SemanticValidation::isValidIdentifier(const std::string &name)
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

void SemanticValidation::visit(LetExpression &node)
{
    symbolTable.enterScope();

    for (auto &decl : *node.declarations)
    {

        if (!isValidIdentifier(decl.name))
        {
            errors.emplace_back("Nombre invalido: '" + decl.name + "'", node.line());
            node._type = "Error";
            continue;
        }

        if (symbolTable.existsInCurrentScope(decl.name))
        {
            errors.emplace_back("Variable '" + decl.name + "' ya declarada en este ambito", node.line());
            node._type = "Error";
            continue;
        }

        decl.initializer->accept(*this);
        std::string initType = decl.initializer->type();

        if (decl.declaredType.empty())
        {
            decl.declaredType = initType;
        }
        else if (!conformsTo(initType, decl.declaredType))
        {
            errors.emplace_back("Tipo declarado '" + decl.declaredType + "' no coincide con inicializador '" + initType + "'", node.line());
            node._type = "Error";
            continue;
        }

        symbolTable.addSymbol(decl.name, decl.declaredType, false);
    }

    node.body->accept(*this);
    node._type = node.body->type();

    symbolTable.exitScope();
}

void SemanticValidation::visit(Assignment &node)
{

    std::string name;
    if (auto *id = dynamic_cast<VarFuncName *>(node.name))
    {
        name = id->name;
    }
    else if (auto *self = dynamic_cast<SelfCall *>(node.name))
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

    if (symbol->type == "Unknown")
    {
        symbolTable.updateSymbolType(name, rhsType);
        node._type = rhsType;
    }
    else if (!conformsTo(rhsType, symbol->type))
    {
        errors.emplace_back("Tipo incorrecto en asignacion: esperado '" + symbol->type + "', obtenido '" + rhsType + "'", node.line());
        node._type = "Error";
    }
    else
    {
        node._type = symbol->type;
    }
    node._type = symbol->type;
}

void SemanticValidation::visit(IfExpression &node)
{
    std::vector<std::string> branchTypes;
    bool hasErrors = false;

    // Evaluar condiciones
    for (auto &branch : *node.branches)
    {
        branch.condition->accept(*this);
        if (branch.condition->type() != "Boolean")
        {
            errors.emplace_back("Condición debe ser booleana", branch.condition->line());
            hasErrors = true;
        }
    }

    // Evaluar cuerpos
    for (auto &branch : *node.branches)
    {
        branch.body->accept(*this);
        if (branch.body->type() == "Error")
            hasErrors = true;
        branchTypes.push_back(branch.body->type());
    }

    if (node.elseBody)
    {
        node.elseBody->accept(*this);
        if (node.elseBody->type() == "Error")
            hasErrors = true;
        branchTypes.push_back(node.elseBody->type());
    }

    if (hasErrors)
    {
        node._type = "Error";
        return;
    }

    // Verificar compatibilidad usando ancestro común
    std::string commonType = symbolTable.lowestCommonAncestor(branchTypes);

    // Validar que el tipo común sea válido
    if (commonType == "Error" || !symbolTable.lookupType(commonType))
    {
        errors.emplace_back("Tipos incompatibles en ramas del 'if'", node.line());
        node._type = "Error";
        return;
    }

    node._type = commonType;
    std::cout << "Tipo unificado: " << node._type << "\n";
}

void SemanticValidation::visit(WhileLoop &node)
{
    // Verificar condicion es booleana
    node.condition->accept(*this);
    std::string condType = node.condition->type();
    if (condType != "Boolean")
    {
        errors.emplace_back("Condicion del while debe ser booleana", node.line());
        node._type = "Error";
        std::cout << condType << std::endl;
    }

    // Analizar cuerpo
    node.body->accept(*this);
    node._type = node.body->type(); // Tipo del while = tipo del cuerpo
}

void SemanticValidation::visit(ForLoop &node)
{
    node.init_range->accept(*this);
    node.end_range->accept(*this);

    if (node.init_range->type() != "Number" || node.end_range->type() != "Number")
    {
        errors.emplace_back("Los limites del 'for' deben ser de tipo Number", node.line());
        node._type = "Error";
        return;
    }

    symbolTable.enterScope();
    symbolTable.addSymbol(node.varName, "Number", false);

    node.body->accept(*this);
    node._type = node.body->type();

    symbolTable.exitScope();
}

void SemanticValidation::visit(TypeDeclaration &node)
{
    std::cout << "Analizando tipo: " << node.name << "\n";

    // Verificar si el tipo ya está registrado
    TypeSymbol *typeSym = symbolTable.lookupType(node.name);
    if (!typeSym)
    {
        errors.emplace_back("Tipo '" + node.name + "' no registrado", node.line());
        return;
    }

    // Verificar herencia solo ahora (cuando todos los tipos están registrados)
    if (!typeSym->parentType.empty())
    {
        const std::set<std::string> builtinTypes = {"Number", "String", "Boolean"};
        if (builtinTypes.count(typeSym->parentType))
        {
            errors.emplace_back("No se puede heredar de tipo básico", node.line());
        }
        else if (!symbolTable.lookupType(typeSym->parentType))
        {
            errors.emplace_back("Tipo padre no encontrado: " + typeSym->parentType, node.line());
        }
    }

    std::string parent = node.baseType.value_or("Object");

    // 3. Registrar el tipo en la tabla
    std::vector<std::string> paramNames;
    for (const auto &param : *node.constructorParams)
    {
        paramNames.push_back(param.name);
    }

    // Después de registrar el tipo
    // TypeSymbol *typeSym = symbolTable.lookupType(node.name);
    if (typeSym)
    {
        // Registrar tipos de parámetros (declarados o inferidos)
        for (const auto &param : *node.constructorParams)
        {
            std::string paramType = param.type.empty() ? "Unknown" : param.type;
            typeSym->paramTypes[param.name] = paramType;
        }
    }

    // 4. Si hereda y no declara baseArgs, asumir que se pasa los parametros propios al padre
    if (node.baseType.has_value() && node.baseArgs.empty())
    {
        TypeSymbol *parentSym = symbolTable.lookupType(*node.baseType);
        if (!parentSym)
        {
            std::cerr << "[DEBUG] No se encontro el tipo base '" << *node.baseType << "'\n";
            errors.emplace_back("Tipo base '" + *node.baseType + "' no encontrado", node.line());
            return;
        }

        // Herencia sin constructor explicito => heredar del padre
        if (node.baseType.has_value() && node.constructorParams->empty() && node.baseArgs.empty())
        {
            TypeSymbol *parentSym = symbolTable.lookupType(*node.baseType);
            if (parentSym)
            {
                std::cerr << "[DEBUG] Tipo '" << node.name << "' hereda de '" << *node.baseType
                          << "' sin constructor explicito. Heredando parametros del padre...\n";

                for (const std::string &paramName : parentSym->typeParamNames)
                {
                    node.constructorParams->emplace_back(Parameter{paramName, ""});
                    node.baseArgs.push_back(new VarFuncName(paramName, node.line()));
                    std::cerr << "  + Param heredado: " << paramName << "\n";
                }
            }
            symbolTable.updateTypeParams(node.name, parentSym->typeParamNames);
        }

        if (parentSym->typeParamNames.size() != node.constructorParams->size())
        {
            std::cerr << "[DEBUG] Tipo padre '" << parentSym->name
                      << "' espera " << parentSym->typeParamNames.size()
                      << " parametros, pero se encontraron "
                      << node.constructorParams->size()
                      << " en el tipo hijo '" << node.name << "'\n";
            errors.emplace_back("Cantidad incorrecta de argumentos para constructor del padre", node.line());
            return;
        }

        for (const auto &param : *node.constructorParams)
        {
            node.baseArgs.push_back(new VarFuncName(param.name, node.line()));
        }
    }

    // 5. Analizar baseArgs (si hay)
    if (node.baseType.has_value() && node.baseArgs.empty())
    {
        TypeSymbol *parentSym = symbolTable.lookupType(*node.baseType);
        if (parentSym)
        {
            // Usar los parámetros del padre si no hay argumentos base
            for (const std::string &parentParam : parentSym->typeParamNames)
            {
                node.constructorParams->emplace_back(Parameter{parentParam, ""});
                node.baseArgs.push_back(new VarFuncName(parentParam, node.line()));
            }
        }
    }

    // 6. Analizar atributos (con inferencia de tipos)
    symbolTable.enterScope();

    // Registrar parámetros
    for (const auto &param : *node.constructorParams)
    {
        std::string paramType = param.type.empty() ? "Unknown" : param.type;
        symbolTable.addSymbol(param.name, paramType, false);
    }

    // Primera pasada: recolectar tipos de inicializadores
    for (const auto &attr : *node.body->attributes)
    {
        attr.initializer->accept(*this);
    }

    // Segunda pasada: inferir y actualizar tipos de parámetros
    for (auto &param : *node.constructorParams)
    {
        if (param.type.empty() || param.type == "Unknown")
        {
            std::string inferredType = "Object";
            bool isSelfAssignment = false;

            // Verificar si el parámetro se asigna directamente a un atributo del mismo nombre
            for (const auto &attr : *node.body->attributes)
            {
                if (attr.name == param.name)
                {
                    if (auto *var = dynamic_cast<VarFuncName *>(attr.initializer))
                    {
                        if (var->name == param.name)
                        {
                            isSelfAssignment = true;
                            break;
                        }
                    }
                }
            }

            if (isSelfAssignment)
            {
                // Si es una asignación directa, usar el tipo actual
                inferredType = node.name;
            }
            else
            {
                // Lógica de inferencia existente...
            }

            param.type = inferredType;
            symbolTable.updateSymbolType(param.name, inferredType);
        }
    }

    // Tercera pasada: re-analizar inicializadores con tipos actualizados
    for (auto &attr : *node.body->attributes)
    {
        attr.initializer->accept(*this);
        std::string attrType = attr.initializer->type();
        std::cout << "[ATTR] Atributo '" << attr.name
                  << "' con tipo: " << attrType << "\n";

        // Registrar atributo SIEMPRE (incluso si es "Unknown")
        if (!symbolTable.addtype_attribute(node.name, attr.name, attrType))
        {
            errors.emplace_back("Atributo '" + attr.name + "' ya existe", node.line());
        }
    }

    symbolTable.exitScope();

    // 7. Analizar metodos
    typeSym = symbolTable.lookupType(node.name);

    for (const auto &method : *node.body->methods)
    {
        symbolTable.enterScope();
        symbolTable.addSymbol("self", node.name, true);

        currentMethodContext = method.name;

        for (const auto &param : *method.params)
        {
            symbolTable.addSymbol(param.name, param.type, false);
        }

        method.body->accept(*this);

        // Verificacion de tipo de retorno
        if (!method.returnType.empty() &&
            !conformsTo(method.body->type(), method.returnType))
        {
            errors.emplace_back("El cuerpo del metodo '" + method.name +
                                    "' no conforma al tipo de retorno declarado",
                                method.body->line());
        }

        std::vector<std::string> paramTypes;
        for (const auto &param : *method.params)
        {
            paramTypes.push_back(param.type);
        }

        symbolTable.addtype_method(node.name, method.name, method.returnType, paramTypes);

        // - Verificacion de firma heredada si aplica
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
                        errors.emplace_back("Firma de metodo '" + method.name +
                                                "' no coincide con la del tipo padre '" + typeSym->parentType + "'",
                                            method.body->line());
                    }
                }
            }
        }

        currentMethodContext.clear();
        symbolTable.exitScope();
    }

    std::cout << "Tipo '" << node.name << "' analizado correctamente\n";
}

void SemanticValidation::visit(InitInstance &node)
{
    TypeSymbol *typeSym = symbolTable.lookupType(node.typeName);
    if (!typeSym)
    {
        std::cerr << "[ERROR] Tipo no encontrado: " << node.typeName << "\n";
        errors.emplace_back("Tipo '" + node.typeName + "' no definido", node.line());
        node._type = "Error";
        return;
    }

    std::cout << "[INST] Creando instancia de: " << node.typeName
              << " con " << node.args.size() << " argumentos\n";

    if (node.args.size() != typeSym->typeParamNames.size())
    {
        errors.emplace_back("Cantidad incorrecta de argumentos para el constructor de '" + node.typeName + "'", node.line());
        node._type = "Error";
        return;
    }

    for (size_t i = 0; i < node.args.size(); ++i)
    {
        node.args[i]->accept(*this);
        std::string argType = node.args[i]->type();

        // Permitir Null como argumento para cualquier parámetro
        if (argType == "Null")
        {
            continue; // Saltar verificación de tipo
        }

        if (i < typeSym->typeParamNames.size())
        {
            std::string paramName = typeSym->typeParamNames[i];
            std::string expectedType = typeSym->paramTypes[paramName];

            if (!conformsTo(argType, expectedType))
            {
                errors.emplace_back("Tipo incorrecto para argumento " + std::to_string(i + 1) +
                                        " en constructor de '" + node.typeName + "': esperado '" + expectedType +
                                        "', obtenido '" + argType + "'",
                                    node.line());
            }

            // Actualizar tipo del parámetro si era "Unknown"
            if (typeSym->paramTypes[paramName] == "Unknown")
            {
                typeSym->paramTypes[paramName] = argType;
                // Actualizar atributo correspondiente
                if (typeSym->attributes.find(paramName) != typeSym->attributes.end())
                {
                    typeSym->attributes[paramName].type = argType;
                }
            }
        }
    }

    node._type = node.typeName;
}

void SemanticValidation::visit(MethodCall &node)
{
    // 1. Analizar la expresión de instancia para obtener su tipo
    node.instance->accept(*this);
    std::string instanceType = node.instance->type();

    if (instanceType == "Null")
    {
        // Permitir llamadas en Null (no genera error)
        node._type = "Null";
        return;
    }

    // 2. Obtener el símbolo del tipo de la instancia
    TypeSymbol *typeSym = symbolTable.lookupType(instanceType);
    if (!typeSym)
    {
        errors.emplace_back("Tipo '" + instanceType + "' no definido", node.line());
        node._type = "Error";
        return;
    }

    // Symbol *instSym = symbolTable.lookup(node.instanceName);
    // if (!instSym)
    // {
    //     errors.emplace_back("Variable '" + node.instanceName + "' no declarada", node.line());
    //     node._type = "Error";
    //     return;
    // }

    // TypeSymbol *typeSym = symbolTable.lookupType(instSym->type);
    // if (!typeSym)
    // {
    //     errors.emplace_back("Tipo '" + instSym->type + "' no definido", node.line());
    //     node._type = "Error";
    //     return;
    // }

    if (node.isMethod)
    {
        // Busqueda jerarquica del metodo en la cadena de herencia
        const Symbol *method = nullptr;
        // Buscar en toda la jerarquía
        TypeSymbol *currentType = typeSym;
        while (currentType)
        {
            auto methodIt = currentType->methods.find(node.methodName);
            if (methodIt != currentType->methods.end())
            {
                method = &methodIt->second;
                break;
            }
            currentType = symbolTable.lookupType(currentType->parentType);
        }

        if (!method)
        {
            errors.emplace_back("Metodo '" + node.methodName + "' no existe en tipo '" + typeSym->name + "'", node.line());
            node._type = "Error";
            return;
        }

        // Validar cantidad de argumentos
        if (node.args.size() != method->params.size())
        {
            errors.emplace_back("Cantidad incorrecta de argumentos en metodo '" + node.methodName + "'", node.line());
            node._type = "Error";
            return;
        }

        // Verificar tipos de argumentos
        for (size_t i = 0; i < node.args.size(); ++i)
        {
            node.args[i]->accept(*this);
            if (!conformsTo(node.args[i]->type(), method->params[i]))
            {
                errors.emplace_back("Tipo incorrecto para argumento " + std::to_string(i + 1) +
                                        " en llamada a '" + node.methodName + "'",
                                    node.line());
            }
        }

        node._type = method->type;
    }

    else
    {
        if (instanceType == "Null")
        {
            node._type = "Null";
            return;
        }

        // Buscar atributo en jerarquía
        Symbol *attr = nullptr;
        while (typeSym)
        {
            auto it = typeSym->attributes.find(node.methodName);
            if (it != typeSym->attributes.end())
            {
                attr = &it->second;
                break;
            }
            if (typeSym->parentType.empty())
                break;
            typeSym = symbolTable.lookupType(typeSym->parentType);
        }

        if (!attr)
        {
            // Permitir acceso a atributos cuando el valor es Null
            if (typeSym->name == "Null")
            {
                node._type = "Null";
                return;
            }

            errors.emplace_back("Atributo '" + node.methodName + "' no existe en tipo '" + instanceType + "'", node.line());
            node._type = "Error";
            return;
        }

        node._type = attr->type;
        std::cout << "[ATTR] Acceso a atributo '" << node.methodName
                  << "' en tipo: " << typeSym->name
                  << " -> Tipo: " << node._type << "\n";
    }
}

void SemanticValidation::visit(AttributeDeclaration &node)
{
    node.initializer->accept(*this);
}

void SemanticValidation::visit(MethodDeclaration &node)
{
}

void SemanticValidation::visit(BaseCall &node)
{
    Symbol *self = symbolTable.lookup("self");
    if (!self)
    {
        errors.emplace_back("'base' solo puede usarse dentro de metodos", node.line());
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

    // Necesitamos saber en que metodo estamos
    std::string currentMethodName = currentMethodContext;
    if (currentMethodName.empty())
    {
        errors.emplace_back("'base' solo puede usarse dentro de un metodo con nombre", node.line());
        node._type = "Error";
        return;
    }

    // Buscar en el padre el metodo con el mismo nombre
    TypeSymbol *parentSym = symbolTable.lookupType(typeSym->parentType);
    if (!parentSym)
    {
        errors.emplace_back("Tipo padre '" + typeSym->parentType + "' no encontrado", node.line());
        node._type = "Error";
        return;
    }

    auto it = parentSym->methods.find(currentMethodName);
    if (it == parentSym->methods.end())
    {
        errors.emplace_back("Metodo '" + currentMethodName + "' no existe en el padre '" + parentSym->name + "'", node.line());
        node._type = "Error";
        return;
    }

    node._type = it->second.type;
}

void SemanticValidation::visit(SelfCall &node)
{
    Symbol *self = symbolTable.lookup("self");
    if (!self)
    {
        errors.emplace_back("'self' solo puede usarse dentro de metodos", node.line());
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

// Buscar un método específico de un tipo en su familia (empezando por él)
Symbol *SemanticValidation::lookupMethodInHierarchy(const std::string &typeName, const std::string &methodName)
{
    std::cout << "[DEBUG] Buscando metodo '" << methodName << "' en jerarquia de tipo '" << typeName << "'\n";

    TypeSymbol *typeSym = symbolTable.lookupType(typeName);
    while (typeSym)
    {
        std::cout << "  [DEBUG] Buscando en tipo: " << typeSym->name << "\n";
        auto it = typeSym->methods.find(methodName);
        if (it != typeSym->methods.end())
        {
            std::cout << "    [ENCONTRADO] Metodo '" << methodName << "' en tipo '" << typeSym->name << "'\n";
            return &it->second;
        }
        if (typeSym->parentType.empty())
            break;
        typeSym = symbolTable.lookupType(typeSym->parentType);
    }

    std::cout << "    [NO ENCONTRADO] Metodo '" << methodName << "' no existe en jerarquia\n";
    return nullptr;
}
