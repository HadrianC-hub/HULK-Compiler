#include "Symbol.hpp"

// Agregados tipos de hulk
SymbolTable::SymbolTable()
{
    enterScope();
    addSymbol("pi", "Number", true);
    addSymbol("e", "Number", true);
    addType("Object", "");
    addType("Number", "Object");
    addType("String", "Object");
    addType("Boolean", "Object");
    addType("Null", "");
}

// Funcion para insertar un scope
void SymbolTable::enterScope()
{
    scopes.push_back({});
}

// Funcion para salir de un scope (quita el scope de la pila)
void SymbolTable::exitScope()
{
    if (!scopes.empty())
        scopes.pop_back();
}

// Agregar un símbolo a la lista
bool SymbolTable::addSymbol(const std::string &name, const std::string &type, bool is_const, const std::vector<std::string> &params)
{
    if (scopes.empty())
        return false;   // No se puede agregar un símbolo a un scope vacío
    auto &current = scopes.back();
    if (current.find(name) != current.end())    // Busca el símbolo en el scope, si no está se agrega, si está no se agrega
        return false;   // Si la busqueda no llega al final, es porque el símbolo está en el scope. No se puede agregar
    current[name] = Symbol{"variable", type, is_const, params};
    return true;
}

// Buscar un símbolo en un scope
Symbol *SymbolTable::lookup(const std::string &name)
{
    // Buscar en scopes locales, si está se devuelve, si no está se retorna un puntero nulo.
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
    {
        auto entry = it->find(name);
        if (entry != it->end())
            return &entry->second;
    }
    return nullptr;
}

// Verifica si un símbolo está en el scope actual
bool SymbolTable::existsInCurrentScope(const std::string &name)
{
    if (scopes.empty())
        return false;
    return scopes.back().find(name) != scopes.back().end(); // Si se llega al final y no se encuentra, es pq no está
}

// Agregar función (sigue la misma lógica que AddSymbol)
bool SymbolTable::addFunction(const std::string &name, const std::string &returnType, const std::vector<std::string> &params, ASTNode *body)
{
    if (scopes.empty())
        return false;
    auto &current = scopes.back();
    if (current.find(name) != current.end())
        return false;

    current[name] = Symbol{"function", returnType, false, params, body}; 
    return true;
}

// Agregar tipo definido por el usuario a la tabla de símbolos
bool SymbolTable::addType(const std::string &name, const std::string &parentType, const std::vector<std::string> &typeParams)
{
    if (types.find(name) != types.end())
        return false;
    types[name] = TypeSymbol{name, parentType, typeParams, {}, {}};
    return true;
}

// Buscar el tipo definido por el usuario
TypeSymbol *SymbolTable::lookupType(const std::string &name)
{
    auto it = types.find(name);
    if (it == types.end())
        return nullptr;
    return &it->second;
}

// Buscar el tipo definido por el usuario (constante)
const TypeSymbol *SymbolTable::lookupType(const std::string &name) const
{
    auto it = types.find(name);
    if (it == types.end())
        return nullptr;
    return &it->second;
}

// Agregar atributo de un tipo definido por el usuario a la tabla
bool SymbolTable::addtype_attribute(const std::string &typeName, const std::string &attrName, const std::string &attrType)
{
    TypeSymbol *type = lookupType(typeName);
    if (!type)
        return false;
    if (type->attributes.find(attrName) != type->attributes.end())
        return false;
    type->attributes[attrName] = Symbol{"attribute", attrType, true, {}};
    return true;
}

// Agregar método de un tipo definido por el usuario a la tabla
bool SymbolTable::addtype_method(const std::string &typeName, const std::string &methodName, const std::string &returnType, const std::vector<std::string> &params)
{
    TypeSymbol *type = lookupType(typeName);
    if (!type)
        return false;
    if (type->methods.find(methodName) != type->methods.end())
        return false;
    type->methods[methodName] = Symbol{"method", returnType, false, params};
    return true;
}

// Obtener funciones definidas por el usuario
std::vector<Symbol> SymbolTable::getUserDefinedFunctions() const
{
    std::vector<Symbol> functions;
    if (scopes.empty())
        return functions;

    const auto &global = scopes.front();
    for (const auto &[name, sym] : global)
    {
        if (sym.body != nullptr)
        {
            functions.push_back(sym);
        }
    }
    return functions;
}

// Actualizar tabla de símbolos de tipos del usuario con nuevo tipo
bool SymbolTable::updateSymbolType(const std::string &name, const std::string &newType)
{
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
    {
        auto symIt = it->find(name);
        if (symIt != it->end())
        {
            symIt->second.type = newType;
            return true;
        }
    }
    return false;
}

// Verifica si un tipo es descendiente de otro
bool SymbolTable::isSubtype(const std::string &subtype, const std::string &supertype)
{
    if (subtype == supertype)
        return true;

    TypeSymbol *type = lookupType(subtype);
    while (type && !type->parentType.empty())
    {
        if (type->parentType == supertype)
            return true;
        type = lookupType(type->parentType);
    }
    return false;
}

// Devuelve el tipo ancestro más cercano entre dos tipos. Si no existe, es Object (supertipo máximo, todo es un object)
std::string SymbolTable::lowestCommonAncestor(const std::vector<std::string> &types)
{
    if (types.empty())
        return "Object";
    if (types.size() == 1)
        return types[0];

    std::string candidate = types[0];

    for (size_t i = 1; i < types.size(); ++i)
    {
        std::string other = types[i];

        if (isSubtype(candidate, other))
        {
        }
        else if (isSubtype(other, candidate))
        {
            candidate = other;
        }
        else
        {
            TypeSymbol *type = lookupType(candidate);
            while (type && type->name != "Object")
            {
                type = lookupType(type->parentType);
                if (type && isSubtype(other, type->name))
                {
                    candidate = type->name;
                    break;
                }
            }

            if (!type)
                return "Object";
        }
    }

    return candidate;
}

// Actualizar los parámetros de un tipo definido por el usuario.
void SymbolTable::updateTypeParams(const std::string &typeName, const std::vector<std::string> &params)
{
    auto it = types.find(typeName);
    if (it != types.end())
    {
        it->second.typeParams = params;
    }
}
