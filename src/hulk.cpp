#include <cstdio>
#include <iostream>
#include <vector>
#include "AST/AST.hpp"
#include "utils/AST_utils.hpp"
#include "Semantic/Semantic_Component.hpp"
#include "Code_Generator/Context.hpp"


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

    std::cout << "[CHECK] AST terminado." << std::endl;

    // Escribiendo nodos en consola
    for (auto node : root)
    {
        std::cout << "Tipo de nodo raíz: " << node->type()
                  << " | Línea: " << node->line() << "\n";
    }

    // Crear el analizador semántico
    SemanticAnalyzer semanticAnalyzer;
    std::cout << "[CHECK] Instancia anlizador creada." << std::endl;

    // Realizar el análisis semántico
    semanticAnalyzer.analyze(root);
    std::cout << "[CHECK] Análisis semántico completado exitosamente." << std::endl;

    CodeGenContext codegen;
    std::cout << "[CHECK] Instancia generador definida." << std::endl;

    try
    {
        codegen.generateCode(root);
    }
    catch (const std::exception &e)
    {
        std::cerr << "[ERROR] durante la generación de código: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "[CHECK] Generación de código completada." << std::endl;

    codegen.dumpIR("Hulk/Hulk-IR.ll");

    std::cout << "[CHECK] Volcando IR en Hulk-IR.ll." << std::endl;

    // Liberar memoria del AST
    delete_ast(root);

    return 0;
}