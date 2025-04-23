#ifndef AST_UTILS_HPP
#define AST_UTILS_HPP

#include <vector>
#include "../ast/AST.hpp"

bool is_valid_ast(const std::vector<ASTNode*>& nodes);
void delete_ast(std::vector<ASTNode*>& nodes);

#endif
