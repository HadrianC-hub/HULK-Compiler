#ifndef AST_UTILS_HPP
#define AST_UTILS_HPP

#include <vector>
#include <iostream>

// Declaracion anticipada para evitar incluir todo el AST
class ASTNode;

/**
 * Verifica si un AST es valido.
 * @param nodes Vector de punteros a nodos del AST.
 * @return true si todos los nodos son no nulos y el vector no esta vacio.
 */
bool is_valid_ast(const std::vector<ASTNode*>& nodes);

/**
 * Libera la memoria de todos los nodos del AST y limpia el vector.
 * @param nodes Vector de punteros a nodos del AST.
 */
void delete_ast(std::vector<ASTNode*>& nodes);

#endif // AST_UTILS_HPP
