#include <cstdio>
#include <iostream>
#include <vector>
#include "ast/
#include "utils/
#include "semantic/
#include "codegen/


extern int yyparse();               // Bison
extern FILE *yyin;                  // Archivo fuente
extern std::vector<ASTNode *> root; // Nodo raíz del AST generado por el parser

int main(int argc, char **argv)
{
    // Archivo por defecto
    const char *filename = "script.hulk"; 

    if (argc >= 2)
    {
        filename = argv[1]; // Usar nombre pasado como argumento
    }

    // Abrir archivo
    FILE *input_file = fopen(filename, "r");
    if (!input_file)
    {
        std::cerr << "Error abriendo archivo: " << filename << std::endl;
        perror("perror");
        return 1;
    }

    yyin = input_file;  //

    if (yyparse() != 0)
    { // Realizar el análisis sintáctico
        std::cerr << "Error: Falló el análisis sintáctico." << std::endl;
        fclose(input_file);
        return 1;
    }
    fclose(input_file);


    return 0;
}