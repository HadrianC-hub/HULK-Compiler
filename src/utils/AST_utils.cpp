#include "ast_utils.hpp"
#include "../ast/AST.hpp"  // O ajusta esta ruta segun la ubicacion real de tu definicion de ASTNode

bool is_valid_ast(const std::vector<ASTNode*>& nodes) {
    if (nodes.empty()) {
        std::cerr << "AST vacio: ningun nodo generado" << std::endl;
        return false;
    }

    for (auto node : nodes) {
        if (!node) {
            std::cerr << "AST contiene nodos nulos" << std::endl;
            return false;
        }
    }
    return true;
}

void delete_ast(std::vector<ASTNode*>& nodes) {
    for (auto node : nodes) {
        delete node;
    }
    nodes.clear();
}
