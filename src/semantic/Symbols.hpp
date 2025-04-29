#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <set>
#include "../ast/AST.hpp"

// Tipos básicos de HULK (predefinidos)
const std::set<std::string> BUILTIN_TYPES = {"Object", "Number", "String", "Boolean", "Null"};

// Símbolo genérico (variables, funciones, tipos)
struct Symbol
{
    std::string kind;                // "variable", "function", "type"
    std::string type;                // Tipo del símbolo (para variables) o tipo de retorno (funciones)
    bool is_const;                   // ¿Es constante? (solo aplica a variables)
    std::vector<std::string> params; // Parámetros (funciones) o tipoParams (tipos)
    ASTNode *body = nullptr;

    // Constructor completo
    Symbol(std::string kind, std::string type, bool is_const, std::vector<std::string> params, ASTNode *body = nullptr)
        : kind(std::move(kind)), type(std::move(type)), is_const(is_const), params(std::move(params)), body(body) {}

    // Constructor mínimo útil
    Symbol(std::string type, bool is_const, std::vector<std::string> params = {}, ASTNode *body = nullptr)
        : kind("variable"), type(std::move(type)), is_const(is_const), params(std::move(params)), body(body) {}

    Symbol() = default;
};



