#include <cstdio>
#include <iostream>
#include <vector>

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
    
    return 0;
}