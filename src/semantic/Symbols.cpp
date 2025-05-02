#include "Symbols.hpp"

SymbolTable::SymbolTable() {
    enterScope(); // Scope global
    // Registrar constantes matemáticas
    addSymbol("pi", "Number", true);
    addSymbol("e", "Number", true);
    addType("Object", "");
    addType("Number", "Object");
    addType("String", "Object");
    addType("Boolean", "Object");
}

void SymbolTable::enterScope() {
    scopes.push_back({});
}


void SymbolTable::exitScope() {
    if (!scopes.empty()) scopes.pop_back();
}

bool SymbolTable::addSymbol(const std::string& name, const std::string& type, bool is_const, const std::vector<std::string>& params) {
    if (scopes.empty()) return false;
    auto& current = scopes.back();
    if (current.find(name) != current.end()) return false;
    current[name] = Symbol{"variable", type, is_const, params};
    return true;
}

Symbol* SymbolTable::lookup(const std::string& name) {
    // Buscar en ámbitos locales
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto entry = it->find(name);
        if (entry != it->end()) return &entry->second;
    }
    return nullptr;
}


bool SymbolTable::existsInCurrentScope(const std::string& name) {
    if (scopes.empty()) return false;
    return scopes.back().find(name) != scopes.back().end();
}


bool SymbolTable::addFunction(
    const std::string& name,
    const std::string& returnType,
    const std::vector<std::string>& params,
    ASTNode* body
) {
    if (scopes.empty()) return false;
    auto& current = scopes.back();
    if (current.find(name) != current.end()) return false;

    current[name] = Symbol{"function", returnType, false, params, body};
    return true;
}

bool SymbolTable::addType(
    const std::string& name,
    const std::string& parentType,
    const std::vector<std::string>& typeParams
) {
    if (types.find(name) != types.end()) return false;
    types[name] = TypeSymbol{
        name,                  // std::string name
        parentType,            // std::string parentType
        typeParams,            // std::vector<std::string> typeParams
        {},                    // Atributos (inicialmente vacío)
        {}                     // Métodos (inicialmente vacío)
    };
    return true;
}



