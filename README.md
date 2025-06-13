# HULK Compiler

HULK es un compilador diseñado para el lenguaje **HULK**, desarrollado como parte de un proyecto de la asignatura de Compilación de tercer año de Ciencias de la Computación. El objetivo del compilador es traducir programas escritos en el lenguaje HULK a **LLVM IR**, permitiendo su posterior optimización y ejecución eficiente.

## Descripción

Este compilador toma como entrada archivos `.hulk` y genera código intermedio en formato `.ll` compatible con LLVM. Está implementado en C++ utilizando herramientas tradicionales de compilación como **Flex** (para el análisis léxico) y **Bison** (para el análisis sintáctico). Además, el compilador incluye:

- Análisis semántico y manejo de tipos.
- Construcción de un **AST (Árbol de Sintaxis Abstracta)**.
- Generación de código intermedio LLVM.
- Soporte para estructuras del lenguaje HULK: funciones, expresiones, variables, condicionales, ciclos, clases con herencia, etc.
- Sistema de errores detallado con línea y columna de reporte.

## Arquitectura

1. **Lexer (`Lexer.l`)**  
   Genera tokens a partir del código fuente utilizando expresiones regulares.

2. **Parser (`Parser.y`)**  
   Analiza la estructura gramatical y construye el AST.

3. **AST (`/ast`)**  
   Contiene las clases que representan los nodos del árbol de sintaxis abstracta.

4. **Análisis Semántico (`/semantic`)**  
   Verifica tipos, declaraciones y coherencia semántica del código.

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