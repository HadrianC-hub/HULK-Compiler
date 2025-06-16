# HULK Compiler

El siguiente proyecto presenta un compilador diseñado para el lenguaje **HULK** (Havana University Language for Kompilers), desarrollado como parte de un proyecto de la asignatura de Compilacion de tercer año de Ciencias de la Computacion. El objetivo del compilador es traducir programas escritos en el lenguaje HULK a **LLVM IR**, permitiendo su posterior compilación a lenguaje de máquina y ejecución.

## Descripcion

Este compilador toma como entrada un archivo situado en la carpeta raíz del proyecto, llamado `script.hulk` y genera código intermedio en formato `.ll` (Representación intermedia de LLVM). El compilador está implementado en C++ y utiliza las herramientas **Flex** (para el analisis lexico) y **Bison** (para el analisis sintáctico). El compilador procesa llevando hasta representación LLVM las siguientes características de HULK:

- Expresiones
- Funciones
- Variables
- Condicionales
- Ciclos
- Tipos
- Herencia
- Polimorfismo

Además, realiza una inferencia de tipos durante la etapa de análisis semántico, devolviendo errores en casos de realización de operaciones inválidas entre tipos no compatibles.

El flujo del proyecto abarca en su totalidad los siguientes aspectos:
- Análisis léxico-sintáctico\
- Construccion de un **AST (Árbol de Sintaxis Abstracta)**.
- Análisis semántico y manejo de tipos.
- Generacion de código intermedio LLVM.
- Sistema de errores detallado con linea y columna de reporte.

## Arquitectura

1. **Lexer (`lexer.l`)**  
   Genera tokens a partir del codigo fuente utilizando expresiones regulares.

2. **Parser (`parser.y`)**  
   Analiza la estructura gramatical y construye el AST.

3. **AST (`/ast`)**  
   Contiene las clases que representan los nodos del arbol de sintaxis abstracta. También contiene la declaración de las funciones que permitirán visitar a los nodos del AST durante el análisis semántico.

4. **Análisis Seméntico (`/semantic`)**  
   Verifica tipos, declaraciones y coherencia seméntica del código. Contienen estructuras que permiten reconocer y almacenar tipos de datos y funciones que pertenecen al lenguaje y declaradas por el programador.

5. **Generador LLVM (`/code_generation`)**  
   Transforma el AST validado en LLVM IR (Intermediate Representation). Contiene estructuras de contexto que permiten ir almacenando según sea necesario las declaraciones realizadas, resultados obtenidos y limpiar memoria después de generar el código, por cada nodo visitado del AST. 

6. **Utilidades (`/Utils`)**  
   Funciones de utilidad en tiempo de ejecución, como manejos de cadenas y operadores.

## Requisitos

- CMake
- LLVM 20.1
- Flex
- Bison
- C++17 o superior
- Sistema operativo: Linux o Windows (MSYS2 / WSL recomendado)