#include <cstdio>
#include <iostream>
#include <vector>
#include "ast/AST.hpp"
#include "utils/AST_utils.hpp"
#include "semantic/SemanticAnalyzer.hpp"
#include "codegen/Context.hpp"


extern int yyparse();              
extern FILE *yyin;                  
extern std::vector<ASTNode *> root; 

int main(int argc, char **argv)
{
    const char *filename = "script.hulk"; 

    if (argc >= 2)
    {
        filename = argv[1]; 
    }

    // Abrir archivo
    FILE *input_file = fopen(filename, "r");
    if (!input_file)
    {
        std::cerr << "[ERROR] No se pudo abrir el archivo: " << filename << std::endl;
        perror("perror");
        return 1;
    }

    yyin = input_file;  //

    if (yyparse() != 0)
    { // Realizar el análisis sintáctico
        std::cerr << "[ERROR] Falló el análisis sintáctico." << std::endl;
        fclose(input_file);
        return 1;
    }
    fclose(input_file);

    if (!is_valid_ast(root))
    {
        std::cerr << "[ERROR] No se generó un AST válido." << std::endl;
        return 1;
    }

    std::cout << "[CHECK] AST terminado correctamente." << std::endl;

    for (auto node : root)
    {
        std::cout << "Tipo de nodo raíz: " << node->type()
                  << " | Línea: " << node->line() << "\n";
    }

    return 0;
}