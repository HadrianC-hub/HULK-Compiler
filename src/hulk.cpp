#include <cstdio>
#include <iostream>
#include <vector>
#include "ast/AST.hpp"
#include "semantic/Semantic.hpp"
#include "code_generation/IRContext.hpp"
#include "utils/ast_utils.hpp"

extern int yyparse();
extern FILE *yyin;
extern std::vector<ASTNode *> root;
extern int lexical_error;

int main(int argc, char **argv)
{
    const char *filename = "script.hulk";

    if (argc >= 2)
    {
        filename = argv[1];
    }

    FILE *input_file = fopen(filename, "r");
    if (!input_file)
    {
        perror("Error abriendo el archivo");
        return 1;
    }

    yyin = input_file;
    yyparse();
    if (lexical_error > 0)
    {
        std::cerr << "[ERROR] Fallo el analisis lexico." << std::endl;
        fclose(input_file);
        return 1;
    }
    if (yyparse() != 0)
    {
        std::cerr << "[SYNTAX ERROR] Fallo el analisis sintactico." << std::endl;
        fclose(input_file);
        return 1;
    }
    fclose(input_file);

    if (!is_valid_ast(root))
    {
        std::cerr << "[SYNTAX ERROR] No se genero un AST valido." << std::endl;
        return 1;
    }

    std::cout << "[DONE] AST terminado." << std::endl;

    for (auto node : root)
    {
        std::cout << "Tipo de nodo raiz: " << node->type()
                  << " | Linea: " << node->line() << "\n";
    }

    SemanticValidation semantic;
    semantic.validate(root);
    std::cout << "[DONE] Analisis semantico completado exitosamente." << std::endl;

    Context cg;
    try
    {
        cg.Generate(root);
    }
    catch (const std::exception &e)
    {
        std::cerr << "[CODE GENERATION ERROR]: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "[DONE] Generacion de codigo completada." << std::endl;
    cg.WriteDownCode("Hulk/Hulk-IR.ll");
    delete_ast(root);

    return 0;
}