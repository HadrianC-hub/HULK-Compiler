// SymbolTable.hpp
#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <set>
#include "../../ast/AST.hpp"

const std::set<std::string> BUILTIN_TYPES = {"Object", "Number", "String", "Boolean", "Null"};

struct Symbol
{
    std::string kind;                   // Si es variable, función, etc
    std::string type;                   // Que tipo de dato representa o devuelve (String, Number...)
    bool is_const;                      // Es constante o no
    std::vector<std::string> params;    // Parámetros
    ASTNode *body = nullptr;            // Puntero al nodo que contiene el cuerpo

    Symbol(std::string kind, std::string type, bool is_const, std::vector<std::string> params, ASTNode *body = nullptr)
        : kind(std::move(kind)), type(std::move(type)), is_const(is_const), params(std::move(params)), body(body) {}

    Symbol(std::string type, bool is_const, std::vector<std::string> params = {}, ASTNode *body = nullptr)
        : kind("variable"), type(std::move(type)), is_const(is_const), params(std::move(params)), body(body) {}

    Symbol() = default;
};

struct TypeSymbol   // Usado para representar tipos declarados por el usuario (cada parametro es una tabla)
{
    std::string name;
    std::string parentType;
    std::vector<std::string> typeParams;
    std::unordered_map<std::string, Symbol> attributes;
    std::unordered_map<std::string, Symbol> methods;
};

class SymbolTable   // Tabla de símbolos que se usará durante el análisis semántico para llevar registros
{
private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;

    std::unordered_map<std::string, TypeSymbol> types;

public:
    SymbolTable();
    void enterScope();
    void exitScope();
    bool addSymbol(const std::string &name, const std::string &type, bool is_const, const std::vector<std::string> &params = {});
    Symbol *lookup(const std::string &name);
    bool existsInCurrentScope(const std::string &name);
    std::string lowestCommonAncestor(const std::vector<std::string> &types);
    bool isSubtype(const std::string &subtype, const std::string &supertype);

    bool addFunction(
        const std::string &name,
        const std::string &returnType,
        const std::vector<std::string> &params,
        ASTNode *body = nullptr);

    bool addType(
        const std::string &name,
        const std::string &parentType,
        const std::vector<std::string> &typeParams = {});
    const TypeSymbol *lookupType(const std::string &name) const;
    TypeSymbol *lookupType(const std::string &name);

    bool addTypeAttribute(const std::string &typeName, const std::string &attrName, const std::string &attrType);
    bool addTypeMethod(
        const std::string &typeName,
        const std::string &methodName,
        const std::string &returnType,
        const std::vector<std::string> &params);

    std::vector<Symbol> getUserDefinedFunctions() const;
    bool updateSymbolType(const std::string &name, const std::string &newType);
    void updateTypeParams(const std::string &typeName, const std::vector<std::string> &params);
};