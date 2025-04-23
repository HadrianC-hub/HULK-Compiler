#include "AST_utils.hpp"
#include <iostream>

// Verificar que el vector no esté vacío y que todos los nodos sean válidos
bool is_valid_ast(const std::vector<ASTNode *> &nodes)
{
    // Nodo vacío
    if (nodes.empty())
    {
        std::cerr << "AST vacío: ningún nodo generado" << std::endl;
        return false;
    }

    // Nodos nulos en el AST
    for (auto node : nodes)
    {
        if (!node)
        {
            std::cerr << "AST contiene nodos nulos" << std::endl;
            return false;
        }
    }
    return true;
}

