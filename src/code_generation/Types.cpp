#include "Types.hpp"

type_definition &TypeSystem::reg_type(const std::string &name, std::optional<std::string> parent)
{
    // Comprobar si el padre existe en caso que sea especificado
    if (parent && !type_exists(*parent))
    {
        throw std::runtime_error("Tipo padre '" + *parent + "' no encontrado para tipo '" + name + "'");
    }

    // Crear y almacenar definición de tipo
    auto [it, inserted] = typeTable.try_emplace(name, name, parent);
    if (!inserted)
    {
        throw std::runtime_error("Tipo '" + name + "' ya existe");
    }

    // Inicializar constructor y parámetros
    it->second.constructorParams = std::vector<std::string>();
    it->second.baseArgs = std::vector<ASTNode *>();

    return it->second;
}

void TypeSystem::add_attribute(const std::string &attrName, const std::string &typeName, ASTNode *initializer)
{
    auto it = typeTable.find(typeName);
    if (it == typeTable.end())
    {
        throw std::runtime_error("Tipo '" + typeName + "' no encontrado");
    }

    // Comprobar si el atributo ya existe
    if (it->second.attributes.find(attrName) != it->second.attributes.end())
    {
        throw std::runtime_error("Atributo '" + attrName + "' ya existe en tipo '" + typeName + "'");
    }

    it->second.attributes.emplace(attrName, type_attribute(attrName, typeName, initializer));
}

void TypeSystem::add_method(const std::string &typeName, const std::string &methodName, std::vector<Parameter> *params, ASTNode *body, const std::string &returnType)
{
    auto it = typeTable.find(typeName);
    if (it == typeTable.end())
    {
        throw std::runtime_error("Tipo '" + typeName + "' no encontrado");
    }

    // Comprobar si el método ya existe
    if (it->second.methods.find(methodName) != it->second.methods.end())
    {
        throw std::runtime_error("Metodo '" + methodName + "' ya existe en tipo '" + typeName + "'");
    }

    it->second.methods.emplace(methodName, type_method(params, body, returnType));
}

void TypeSystem::new_instance(const std::string &varName, const std::string &typeName, const std::map<std::pair<std::string, std::string>, llvm::Value *> &vars)
{
    if (!type_exists(typeName))
    {
        throw std::runtime_error("Tipo '" + typeName + "' no encontrado");
    }

    // Comprobar si la instancia ya existe
    if (instanceTable.find(varName) != instanceTable.end())
    {
        throw std::runtime_error("Instancia '" + varName + "' ya existe");
    }

    instanceTable[varName] = typeName;
    instanceVars[varName] = vars;
}

std::string TypeSystem::get_instance_type(const std::string &varName) const
{
    auto it = instanceTable.find(varName);
    return it != instanceTable.end() ? it->second : "";
}

type_method *TypeSystem::find_method(const std::string &typeName, const std::string &methodName)
{
    auto it = typeTable.find(typeName);
    if (it == typeTable.end())
        return nullptr;

    // Buscar al método en el tipo actual
    auto methodIt = it->second.methods.find(methodName);
    if (methodIt != it->second.methods.end())
    {
        return &methodIt->second;
    }

    // Si no fue encontrado y tiene un padre, buscar en el padre
    if (it->second.parentType)
    {
        return find_method(*it->second.parentType, methodName);
    }

    return nullptr;
}

type_attribute *TypeSystem::find_attribute(const std::string &typeName, const std::string &attrName)
{
    auto it = typeTable.find(typeName);
    if (it == typeTable.end())
        return nullptr;

    // Buscar un atributo en el tipo actual
    auto attrIt = it->second.attributes.find(attrName);
    if (attrIt != it->second.attributes.end())
    {
        return &attrIt->second;
    }

    // Si no fue encontrado y tiene un padre, buscar en el padre
    if (it->second.parentType)
    {
        return find_attribute(*it->second.parentType, attrName);
    }

    return nullptr;
}