#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include "../ast/AST.hpp"
#include <llvm/IR/Value.h>

// Representa un espacio de memoria reservado para una variable o metodo
struct placeholder
{
    std::string name;
    std::string type; // "var" o "method"

    placeholder(std::string n, std::string t) : name(std::move(n)), type(std::move(t)) {}
};

// Representa al metodo de un tipo definido por el usuario, con parámetros y cuerpo
struct type_method
{
    std::vector<Parameter> *params;
    ASTNode *body;
    std::string returnType;

    type_method(std::vector<Parameter> *p, ASTNode *b, std::string ret = "")
        : params(p), body(b), returnType(std::move(ret)) {}
};

// Representa los atributos de un tipo definido por el usuario, con su inicializador
struct type_attribute
{
    std::string AttrName;
    std::string TypeName;
    ASTNode *initializer;

    type_attribute(std::string attrName, std::string attrType, ASTNode *init)
        : AttrName(std::move(attrName)), TypeName(std::move(attrType)), initializer(init) {}
};

// Representa un tipo definido por el usuario, con sus atributos e inicializador
struct type_definition
{
    std::string name;
    std::optional<std::string> parentType;
    std::map<std::string, type_attribute> attributes;
    std::map<std::string, type_method> methods;
    std::vector<std::string> constructorParams; // List of parameter names
    std::vector<ASTNode *> baseArgs;            // Arguments for base constructor

    type_definition(std::string n, std::optional<std::string> parent = std::nullopt)
        : name(std::move(n)), parentType(std::move(parent)),
          constructorParams(),
          baseArgs() {}
};

// Maneja el sistema de tipos, incluyendo definiciones e instancias
class TypeSystem
{
private:
    // Mapa de nombres de tipos a definiciones
    std::map<std::string, type_definition> typeTable;

    // Mapa de instancias de tipos a sus tipos
    std::map<std::string, std::string> instanceTable;

    // Mapa de instancias de tipos a sus valores de atributos con información de tipo
    std::map<std::string, std::map<std::pair<std::string, std::string>, llvm::Value *>> instanceVars;

    // Tipo actual siendo procesado (para llamadas a self y base)
    std::string currentType;

    // Pila para rastrear variables siendo procesadas con sus tipos
    std::vector<placeholder> placeholderStack;

    // Pila para rastrear la instancia actual de variable con su información de tipo
    std::vector<std::map<std::pair<std::string, std::string>, llvm::Value *> *> currentInstanceVarsStack;

public:

    // Nuevo mapa para rastrear valores de instancias
    std::map<llvm::Value*, std::string> valueToTypeMap;

    // Mapa para rastrear nombres de instancias por valor
    std::map<llvm::Value*, std::string> valueToInstanceNameMap;

    bool instance_exists(const std::string& name) const {
        return instanceTable.find(name) != instanceTable.end();
    }

    // Registrar tipo por valor LLVM
    void register_value_type(llvm::Value* val, const std::string& typeName) {
        valueToTypeMap[val] = typeName;
    }

    // Registra la nueva definición de un tipo
    type_definition &reg_type(const std::string &name, std::optional<std::string> parent = std::nullopt);

    // Obtiene la definición de un tipo por su nombre
    type_definition &get_type_definition(const std::string &typeName)
    {
        auto it = typeTable.find(typeName);
        if (it == typeTable.end())
        {
            throw std::runtime_error("Type '" + typeName + "' not found");
        }
        return it->second;
    }

    // Obtiene los parámetros del constructor de un tipo
    const std::vector<std::string> &get_constructor_params(const std::string &typeName)
    {
        return get_type_definition(typeName).constructorParams;
    }

    // Obtiene los argumentos base para un tipo
    const std::vector<ASTNode *> &get_base_args(const std::string &typeName)
    {
        return get_type_definition(typeName).baseArgs;
    }

    // Obtiene los atributos para un tipo
    const std::map<std::string, type_attribute> &get_attributes(const std::string &typeName)
    {
        return get_type_definition(typeName).attributes;
    }

    // Obtiene el nombre del tipo padre para un tipo
    std::optional<std::string> get_parent_type(const std::string &typeName)
    {
        return get_type_definition(typeName).parentType;
    }

    // Agrega un atributo a un tipo
    void add_attribute(const std::string &attrName, const std::string &typeName, ASTNode *initializer);

    // Agrega un método a un tipo
    void add_method(const std::string &typeName, const std::string &methodName, std::vector<Parameter> *params, ASTNode *body, const std::string &returnType = "");

    // Crea una nueva instancia del tipo
    void new_instance(const std::string &varName, const std::string &typeName, const std::map<std::pair<std::string, std::string>, llvm::Value *> &vars = {});

    // Obtiene el nombre del tipo de una instancia
    std::string get_instance_type(const std::string &varName) const;

    // Obtiene los valores de los atributos de una instancia
    const std::map<std::pair<std::string, std::string>, llvm::Value *> &get_instance_vars(const std::string &instanceName) const
    {
        auto it = instanceVars.find(instanceName);
        if (it == instanceVars.end())
        {
            throw std::runtime_error("Instance '" + instanceName + "' not found");
        }
        return it->second;
    }

    // Asigna un valor a un atributo de una instancia
    void set_instance_var(const std::string &instanceName, const std::string &attrName, const std::string &attrType, llvm::Value *value)
    {
        instanceVars[instanceName][{attrName, attrType}] = value;
    }

    // Obtiene el valor de un atributo de una instancia
    llvm::Value *get_instance_var(const std::string &instanceName, const std::string &attrName, const std::string &attrType) const
    {
        auto it = instanceVars.find(instanceName);
        if (it == instanceVars.end())
            return nullptr;

        auto attrIt = it->second.find({attrName, attrType});
        return attrIt != it->second.end() ? attrIt->second : nullptr;
    }

    // Define al tipo actual siendo procesado
    void set_current_type(const std::string &typeName) { currentType = typeName; }

    // Obtiene al tipo actual siendo procesado
    std::string get_current_type() const { return currentType; }

    // Encuentra un método en un tipo o en sus padres
    type_method *find_method(const std::string &typeName, const std::string &methodName);

    // Encuentra un atributo en un tipo o en sus padres
    type_attribute *find_attribute(const std::string &typeName, const std::string &attrName);

    // Verifica que un tipo exista
    bool type_exists(const std::string &typeName) const
    {
        return typeTable.find(typeName) != typeTable.end();
    }

    // Envía una variable junto con su tipo a un espacio reservado en la pila (placeholder)
    void push_placeholder(const std::string &varName, const std::string &varType)
    {
        placeholderStack.push_back(placeholder(varName, varType));
    }

    // Extrae de la pila una variable junto con su tipo de la pila
    placeholder pop_placeholder()
    {
        if (placeholderStack.empty())
        {
            return placeholder("", "");
        }
        placeholder entry = placeholderStack.back();
        placeholderStack.pop_back();
        return entry;
    }

    // Obtiene la variable actual junto con su tipo que están siendo procesados
    placeholder get_current_placeholder() const
    {
        return placeholderStack.empty() ? placeholder("", "") : placeholderStack.back();
    }

    // Envía un nuevo mapa de variables de instancia a la pila
    void push_current_instance_vars(std::map<std::pair<std::string, std::string>, llvm::Value *> *vars)
    {
        currentInstanceVarsStack.push_back(vars);
    }

    // Extrae de la pila un mapa de variables de instancia
    void pop_current_instance_vars()
    {
        if (!currentInstanceVarsStack.empty())
        {
            currentInstanceVarsStack.pop_back();
        }
    }

    // Establece un valor en el mapa de variables de instancia actual
    void set_current_instance_var(const std::string &varName, const std::string &varType, llvm::Value *value)
    {
        if (!currentInstanceVarsStack.empty())
        {
            (*currentInstanceVarsStack.back())[{varName, varType}] = value;
        }
    }

    // Obtiene un valor en el mapa de variables de instancia actual
    llvm::Value *get_current_instance_var(const std::string &varName, const std::string &varType) const
    {
        if (currentInstanceVarsStack.empty())
            return nullptr;
        auto &currentVars = *currentInstanceVarsStack.back();
        auto it = currentVars.find({varName, varType});
        return it != currentVars.end() ? it->second : nullptr;
    }

    // Obtiene todos los nombres de instancias
    std::vector<std::string> get_all_instances_names() const
    {
        std::vector<std::string> names;
        for (const auto &[name, _] : instanceTable)
        {
            names.push_back(name);
        }
        return names;
    }

    // Comprueba si la pila de variables de instancias está vacía (true)
    bool is_instance_vars_stack_empty() const
    {
        return currentInstanceVarsStack.empty();
    }

    std::map<std::pair<std::string, std::string>, llvm::Value *> *get_instance_vars_mutable(const std::string &instanceName)
    {
        auto it = instanceVars.find(instanceName);
        return it != instanceVars.end() ? &it->second : nullptr;
    }
};