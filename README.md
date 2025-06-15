# HULK Compiler

HULK es un compilador diseñado para el lenguaje **HULK**, desarrollado como parte de un proyecto de la asignatura de Compilacion de tercer año de Ciencias de la Computacion. El objetivo del compilador es traducir programas escritos en el lenguaje HULK a **LLVM IR**, permitiendo su posterior optimizacion y ejecucion eficiente.

## Descripcion

Este compilador toma como entrada archivos `.hulk` y genera codigo intermedio en formato `.ll` compatible con LLVM. Esta implementado en C++ utilizando herramientas tradicionales de compilacion como **Flex** (para el analisis lexico) y **Bison** (para el analisis sintactico). Ademas, el compilador incluye:

- Analisis semantico y manejo de tipos.
- Construccion de un **AST (arbol de Sintaxis Abstracta)**.
- Generacion de codigo intermedio LLVM.
- Soporte para estructuras del lenguaje HULK: funciones, expresiones, variables, condicionales, ciclos, clases con herencia, etc.
- Sistema de errores detallado con linea y columna de reporte.

## Arquitectura

1. **Lexer (`Lexer.l`)**  
   Genera tokens a partir del codigo fuente utilizando expresiones regulares.

2. **Parser (`Parser.y`)**  
   Analiza la estructura gramatical y construye el AST.

3. **AST (`/ast`)**  
   Contiene las clases que representan los nodos del arbol de sintaxis abstracta.

4. **Analisis Semantico (`/semantic`)**  
   Verifica tipos, declaraciones y coherencia semantica del codigo.

5. **Generador LLVM (`/code_generation`)**  
   Transforma el AST validado en LLVM IR.

6. **Utilidades (`/Utils`)**  
   Funciones de utilidad como manejo de cadenas, operadores, etc.

## Requisitos

- CMake
- LLVM >= 14
- Flex
- Bison
- C++17 o superior
- Sistema operativo: Linux o Windows (MSYS2 / WSL recomendado)