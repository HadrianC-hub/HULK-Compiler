%{

#include <stdio.h>
#include "../ast/AST.hpp"

extern int yylex();
void yyerror(const char *msg);

// Definir estructura para ubicación
typedef struct YYLTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
} YYLTYPE;
#define YYLTYPE_IS_DECLARED 1

std::vector<ASTNode*> root;

#define PI_VAL 3.14159265358979323846
#define TRACE(EXPR) std::cout << "elem_expr: " << *EXPR << std::endl;

std::vector<ASTNode*> vectorize(ASTNode* arg1, ASTNode* arg2, int n) {
    
    std::vector<ASTNode*> args = std::vector<ASTNode*>();
    
    if ((n == 1) || (n == 2)) { args.push_back(arg1); }
    if (n == 2) { args.push_back(arg2); }

    return args;
}

%}

%code requires {
    #include <string>
    #include <iostream>
    #include <cmath>
    #include <vector>
    #include "../ast/AST.hpp"
}

// Habilitar seguimiento de ubicaciones
%locations

// Definir la unión de tipos semánticos
%union {
    double num;  // Tipo para números (enteros y decimales)
    std::string* str; 
    bool boolean;
    ASTNode* node;
    std::vector<ASTNode*>* list;
    std::vector<IfBranch>* if_branch;
    std::vector<Parameter>* param;
    std::vector<LetDeclaration>* let_decl;
    std::vector<AttributeDeclaration>* attr_decl;
    std::vector<MethodDeclaration>* method_decl;
}

// --------------------------------------/* Definición de Tokens */------------------------------------------- //

// Literales 
%token <num> NUMBER
%token <str> STRING
%token <boolean> BOOL
%token NULL_VAL
%token <str> ID

%token ',' ';' '.'
%token '(' ')'
%token '{' '}'
%token LAMBDA 
%token '='
%token REASSIGN

// operadores aritméticos
%token ADD
%token SUB
%token MUL
%token DIV
%token MOD
%token POW

// operadores booleanos entre expresiones numéricas
%token LT
%token GT 
%token LE 
%token GE 
%token EQ
%token NE

// operadores booleanos entre expresiones booleanas
%token AND
%token OR 
%token NOT

// operadores para literales de texto
%token CONCAT
%token CONCAT_SPACE

// funciones matemáticas básicas
%token SIN
%token COS
%token MAX
%token MIN
%token SQRT
%token EXP
%token LOG
%token RANDOM

// funciones predeterminadas
%token PRINT

// constantes 
%token PI 
%token E

// palabras clave
%token FUNC
%token LET 
%token IN
%token IF 
%token ELIF 
%token ELSE
%token FOR 
%token WHILE 
%token RANGE
%token TYPE 
%token NEW 
%token SELF
%token INHERITS
%token BASE

// -----------------------------/* Definición de Tipos para las Reglas Gramaticales */------------------------ //
%type <node> statement
%type <node> expression
%type <node> elem_expr
%type <node> block_expr
%type <node> func_call_expr
%type <node> assign_expr
%type <node> let_expr
%type <node> if_expr
%type <node> while_expr 
%type <node> for_expr
%type <node> body
%type <list> block_body
%type <list> args
%type <param> params
%type <if_branch> if_head
%type <let_decl> decl
%type <node> type_decl
%type <attr_decl> attribute_decl
%type <method_decl> method_decl
%type <node> method_call
%type <node> base_call
%type <node> new_instance

// ---------------------------------------/* Precedencia de Operadores */------------------------------------- //
%left ADD SUB
%left MUL DIV MOD
%right  POW SIN COS MIN MAX SQRT LOG EXP RANDOM
%left LT GT LE GE EQ NE
%left AND OR 
%right NOT
%left CONCAT CONCAT_SPACE

%%

program:
    /* vacío */
    | program statement             { root.push_back($2); }
    | program error ';'             { yyerrok; }
;

statement:
    expression ';'                  { $$ = $1; }
    | PRINT '(' expression ')' ';'  { 
                                        std::vector<ASTNode*> args = vectorize($3, nullptr, 1);
                                        $$ = new BuiltInFunctionNode("print", args, yylloc.first_line);
                                    }
    | type_decl                     { $$ = $1; }
    | block_expr                    { $$ = $1; }
    | block_expr ';'                { $$ = $1; }
    | FUNC ID '(' params ')' LAMBDA body
                                    {
                                        $$ = new FunctionDeclarationNode(*$2, $4, $7, true, yylloc.first_line);
                                        std::cout << "Definición función inline: " << *$2 << std::endl;
                                        
                                    }
    | FUNC ID '(' params ')' block_expr
                                    {
                                        $$ = new FunctionDeclarationNode(*$2, $4, $6, false, yylloc.first_line);
                                        std::cout << "Definición función bloque: " << *$2 << std::endl;
                                        
                                    }
    | let_expr                      { $$ = $1; std::cout << "let_expr " << std::endl; }
    | while_expr                    { $$ = $1; std::cout << "while_expr " << std::endl; }
    | for_expr                      { $$ = $1; std::cout << "for_expr " << std::endl; }
;

    expression:
          NUMBER                { $$ = new LiteralNode(std::to_string($1), "Number", yylloc.first_line); }
        | STRING                { $$ = new LiteralNode(*$1, "String", yylloc.first_line);  }
        | BOOL                  { $$ = new LiteralNode($1 ? "true" : "false", "Boolean", yylloc.first_line); }
        | NULL_VAL              { $$ = new LiteralNode("null", "Null", yylloc.first_line); }
        | ID                    { $$ = new IdentifierNode(*$1, yylloc.first_line); }
        | SELF '.' ID           { $$ = new SelfCallNode(*$3, yylloc.first_line); }
        | new_instance          { $$ = $1; }
        | elem_expr             { $$ = $1; }
        | block_expr            { $$ = $1; }
        | func_call_expr        { $$ = $1; }
        | method_call           { $$ = $1; }
        | base_call             { $$ = $1; }
        | assign_expr           { $$ = $1; }
        | let_expr              { $$ = $1; std::cout << "let_expr " << std::endl; }
        | if_expr               { $$ = $1; std::cout << "if_expr " << std::endl; }
        | while_expr            { $$ = $1; std::cout << "while_expr " << std::endl; }
        | for_expr              { $$ = $1; std::cout << "for_expr " << std::endl; }
    ;

        elem_expr:
              expression ADD expression {
                $$ = new BinaryOpNode("+", $1, $3, yylloc.first_line);
                
            }
            | expression SUB expression {
                $$ = new BinaryOpNode("-", $1, $3, yylloc.first_line);
                
            }
            | expression MUL expression {
                $$ = new BinaryOpNode("*", $1, $3, yylloc.first_line);
                
            }
            | expression DIV expression {
                $$ = new BinaryOpNode("/", $1, $3, yylloc.first_line);
                
            }
            | expression MOD expression {
                
                $$ = new BinaryOpNode("%", $1, $3, yylloc.first_line);
                
            }
            | expression POW expression {
                $$ = new BinaryOpNode("^", $1, $3, yylloc.first_line);
                
            }

            | SUB expression {
                $$ = new UnaryOpNode("-", $2, yylloc.first_line);
                
            }

            | '(' expression ')' {
                $$ = $2;
            }

            | SIN '(' expression ')' {
                std::vector<ASTNode*> args = vectorize($3, nullptr, 1);
                $$ = new BuiltInFunctionNode("sin", args, yylloc.first_line);
               
            }
            | COS '(' expression ')' {
                std::vector<ASTNode*> args = vectorize($3, nullptr, 1);
                $$ = new BuiltInFunctionNode("cos", args, yylloc.first_line);
                
            }
            | MIN '(' expression ',' expression ')' {
                std::vector<ASTNode*> args = vectorize($3, $5, 2);
                $$ = new BuiltInFunctionNode("min", args, yylloc.first_line);
                
            }
            | MAX '(' expression ',' expression ')' {
                std::vector<ASTNode*> args = vectorize($3, $5, 2);
                $$ = new BuiltInFunctionNode("max", args, yylloc.first_line);
                
            }
            | SQRT '(' expression ')' {
                std::vector<ASTNode*> args = vectorize($3, nullptr, 1);
                $$ = new BuiltInFunctionNode("sqrt", args, yylloc.first_line);
                
            }
            | LOG '(' expression ',' expression ')' {
                std::vector<ASTNode*> args = vectorize($3, $5, 2);
                $$ = new BuiltInFunctionNode("log", args, yylloc.first_line);
                
            }
            | EXP '(' expression ')' {
                std::vector<ASTNode*> args = vectorize($3, nullptr, 1);
                $$ = new BuiltInFunctionNode("exp", args, yylloc.first_line);
                
            }

            | RANDOM '(' ')' {
                std::vector<ASTNode*> args = vectorize(nullptr, nullptr, 0);
                $$ = new BuiltInFunctionNode("rand", args, yylloc.first_line);
            }
            | E     { $$ = new IdentifierNode("e", yylloc.first_line); }
            | PI    { $$ = new IdentifierNode("pi", yylloc.first_line); }

            | expression CONCAT expression {
                $$ = new BinaryOpNode("@", $1, $3, yylloc.first_line);
               
            }
            | expression CONCAT_SPACE expression {
                $$ = new BinaryOpNode("@@", $1, $3, yylloc.first_line);
               
            }

            | expression LT expression {
                
                $$ = new BinaryOpNode("<", $1, $3, yylloc.first_line);
                
            }
            | expression GT expression {
                $$ = new BinaryOpNode(">", $1, $3, yylloc.first_line);
                
            }
            | expression LE expression {
                $$ = new BinaryOpNode("<=", $1, $3, yylloc.first_line);
               
            }
            | expression GE expression {
                $$ = new BinaryOpNode(">=", $1, $3, yylloc.first_line);
                
            }

            | expression EQ expression {
                $$ = new BinaryOpNode("==", $1, $3, yylloc.first_line);
               
            }
            | expression NE expression {
                $$ = new BinaryOpNode("!=", $1, $3, yylloc.first_line);
               
            }

            | expression AND expression {
                $$ = new BinaryOpNode("&", $1, $3, yylloc.first_line);
                
            }
            | expression OR expression {
                $$ = new BinaryOpNode("|", $1, $3, yylloc.first_line);
                
            }
            | NOT expression {
                $$ = new UnaryOpNode("!", $2, yylloc.first_line);
               
            }
        ;

        block_expr:
            '{' block_body '}'  {
                                    $$ = new BlockNode(*$2, yylloc.first_line); // Placeholder
                                   
                                }
        ;

        block_body:
            /* empty */                     { $$ = new std::vector<ASTNode*>(); }
            | statement                     { $$ = new std::vector<ASTNode*>(); $$->push_back($1); }
            | block_body statement          { $1->push_back($2); $$ = $1; }
        ;

        params:
            /* empty */         { $$ = new std::vector<Parameter>(); }
            | ID                { 
                                    Parameter p;
                                    p.name = *$1;
                                    $$ = new std::vector<Parameter>(); 
                                    $$->push_back(p); 
                                   
                                }
            | params ',' ID     { 
                                    Parameter p;
                                    p.name = *$3;
                                    $1->push_back(p); 
                                    $$ = $1; 
                                }
        ;

        func_call_expr:
            ID '(' args ')'     { $$ = new FunctionCallNode(*$1, *$3, yylloc.first_line); }
        ;

        args: 
            /* empty */                 { $$ = new std::vector<ASTNode*>(); }
            | expression                { $$ = new std::vector<ASTNode*>(); $$->push_back($1); }
            | args ',' expression       { $1->push_back($3); $$ = $1; }
        ;
