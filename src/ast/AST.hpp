#pragma once
#include <vector>
#include <optional>
#include <string>
#include <memory>

// Clase base para todos los nodos del AST
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual int line() const = 0;
    virtual std::string type() const = 0;
};


