%{
#include <stdio.h>
#include "../ast/AST.hpp"

extern int yylex();
void yyerror(const char *msg);

typedef struct YYLTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
} YYLTYPE;
#define YYLTYPE_IS_DECLARED 1

std::vector<ASTNode*> root;

#define TRACE(EXPR) std::cout << "elemental_expression: " << *EXPR << std::endl;

std::vector<ASTNode*> vectorize(ASTNode* arg1, ASTNode* arg2, int n) {
    
    std::vector<ASTNode*> args = std::vector<ASTNode*>();
    
    if ((n == 1) || (n == 2)) { args.push_back(arg1); }
    if (n == 2) { args.push_back(arg2); }

    return args;
}

%}

%code requires 
{
    #include <string>
    #include <iostream>
    #include <cmath>
    #include <vector>
    #include "../ast/AST.hpp"
}

%locations

%union 
{
    double num;
    std::string* str; 
    bool boolean;
    ASTNode* node;
    std::vector<ASTNode*>* list;
    std::vector<IfBranch>* if_branch;
    std::vector<Parameter>* param;
    std::vector<LetDeclaration>* let_decl;
    std::vector<AttributeDeclaration>* attr_decl;
    std::vector<MethodDeclaration>* method_decl;
    TypeBody* type_body;
}

// DEFINICIONES

// Literales
%token <num> NUMBER
%token <str> STRING
%token <boolean> BOOLEAN
%token NULL_VAL
%token <str> ID
%token ',' ';' DOT
%token '(' ')'
%token '{' '}'
%token INLINE 
%token ASSIGN
%token REASSIGN
%token ADD
%token SUB
%token MUL
%token DIV
%token MOD
%token POW
%token LTH
%token GTH 
%token LEQ 
%token GEQ 
%token EQU
%token NEQ
%token AND
%token OR 
%token NOT
%token CON
%token CSP
%token SIN
%token COS
%token MAX
%token MIN
%token SQRT
%token EXP
%token LOG
%token RANDOM
%token PRINT
%token PI 
%token E
%token FUN
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
%token COLON

// Tipos
%type <node> statement
%type <node> expression
%type <node> elemental_expression
%type <node> id
%type <node> block_expression
%type <node> func_call
%type <node> assign
%type <node> let
%type <node> if_e
%type <node> while_e
%type <node> for_e
%type <node> body
%type <node> method_call
%type <node> base_call
%type <node> new_instance
%type <node> self_call
%type <list> block_body
%type <list> args
%type <param> params
%type <if_branch> if_head
%type <let_decl> declaration
%type <node> type_declaration
%type <attr_decl> attribute_declaration
%type <method_decl> method_declaration
%type <type_body> type_body

// Precedencia
%left CON CSP
%right NOT
%left AND OR 
%left LTH GTH LEQ GEQ EQU NEQ
%right SIN COS MIN MAX SQRT LOG EXP RANDOM
%left ADD SUB
%left MUL DIV MOD
%right POW
%%


// PRODUCCIONES

program:
    /* nada */
    | program statement                                 {root.push_back($2);}
;

statement:
    ';'                                                     {$$ = NULL;}
    | expression ';'                                        {$$ = $1;}
    | PRINT '(' expression ')' ';'                          {std::vector<ASTNode*> args = vectorize($3, nullptr, 1); $$ = new BuiltInFunc("print", args, yylloc.first_line); }
    | type_declaration                                      {$$ = $1;}
    | block_expression                                      {$$ = $1;}
    | FUN ID '(' params ')' INLINE body ';'                 {$$ = new FuncDeclaration(*$2, $4, $7, true, yylloc.first_line);}
    | FUN ID '(' params ')' block_expression ';'            {$$ = new FuncDeclaration(*$2, $4, $6, false, yylloc.first_line);}
    | FUN ID '(' params ')' COLON ID INLINE body ';'        {$$ = new FuncDeclaration(*$2, $4, $9, true, yylloc.first_line, *$7);}
    | FUN ID '(' params ')' COLON ID block_expression ';'   {$$ = new FuncDeclaration(*$2, $4, $8, false, yylloc.first_line, *$7);}
    | FUN ID '(' params ')' INLINE body                     {$$ = new FuncDeclaration(*$2, $4, $7, true, yylloc.first_line);}
    | FUN ID '(' params ')' block_expression                {$$ = new FuncDeclaration(*$2, $4, $6, false, yylloc.first_line);}
    | FUN ID '(' params ')' COLON ID INLINE body            {$$ = new FuncDeclaration(*$2, $4, $9, true, yylloc.first_line, *$7);}
    | FUN ID '(' params ')' COLON ID block_expression       {$$ = new FuncDeclaration(*$2, $4, $8, false, yylloc.first_line, *$7);}
    | let                                                   {$$ = $1;}
    | while_e                                               {$$ = $1;}
    | for_e                                                 {$$ = $1;}
;

expression:
    NUMBER                  { $$ = new DataType(std::to_string($1), "Number", yylloc.first_line); }
    | STRING                { $$ = new DataType(*$1, "String", yylloc.first_line);  }
    | BOOLEAN               { $$ = new DataType($1 ? "true" : "false", "Boolean", yylloc.first_line); }
    | NULL_VAL              { $$ = new DataType("null", "Null", yylloc.first_line); }
    | ID                    { $$ = new VarFuncName(*$1, yylloc.first_line); }
    | self_call             { $$ = $1; }
    | new_instance          { $$ = $1; }
    | elemental_expression  { $$ = $1; }
    | block_expression      { $$ = $1; }
    | func_call             { $$ = $1; }
    | method_call           { $$ = $1; }
    | base_call             { $$ = $1; }
    | assign                { $$ = $1; }
    | let                   { $$ = $1; }
    | if_e                  { $$ = $1; }
    | while_e               { $$ = $1; }
    | for_e                 { $$ = $1; }
;

elemental_expression:
    expression ADD expression                   {$$ = new BinaryOperation("+", $1, $3, yylloc.first_line);}
    | expression SUB expression                 {$$ = new BinaryOperation("-", $1, $3, yylloc.first_line);}
    | expression MUL expression                 {$$ = new BinaryOperation("*", $1, $3, yylloc.first_line);}
    | expression DIV expression                 {$$ = new BinaryOperation("/", $1, $3, yylloc.first_line);}
    | expression MOD expression                 {$$ = new BinaryOperation("%", $1, $3, yylloc.first_line);}
    | expression POW expression                 {$$ = new BinaryOperation("^", $1, $3, yylloc.first_line);}
    | SUB expression                            {$$ = new UnaryOperation("-", $2, yylloc.first_line);}
    | '(' expression ')'                        {$$ = $2;}
    | SIN '(' expression ')'                    {std::vector<ASTNode*> args = vectorize($3, nullptr, 1); $$ = new BuiltInFunc("sin", args, yylloc.first_line);}
    | COS '(' expression ')'                    {std::vector<ASTNode*> args = vectorize($3, nullptr, 1); $$ = new BuiltInFunc("cos", args, yylloc.first_line);}
    | MIN '(' expression ',' expression ')'     {std::vector<ASTNode*> args = vectorize($3, $5, 2); $$ = new BuiltInFunc("min", args, yylloc.first_line);}
    | MAX '(' expression ',' expression ')'     {std::vector<ASTNode*> args = vectorize($3, $5, 2); $$ = new BuiltInFunc("max", args, yylloc.first_line);}
    | SQRT '(' expression ')'                   {std::vector<ASTNode*> args = vectorize($3, nullptr, 1);$$ = new BuiltInFunc("sqrt", args, yylloc.first_line);}
    | LOG '(' expression ',' expression ')'     {std::vector<ASTNode*> args = vectorize($3, $5, 2); $$ = new BuiltInFunc("log", args, yylloc.first_line);}
    | EXP '(' expression ')'                    {std::vector<ASTNode*> args = vectorize($3, nullptr, 1); $$ = new BuiltInFunc("exp", args, yylloc.first_line);}
    | RANDOM '(' ')'                            {std::vector<ASTNode*> args = vectorize(nullptr, nullptr, 0); $$ = new BuiltInFunc("rand", args, yylloc.first_line);}
    | E                                         {$$ = new VarFuncName("e", yylloc.first_line);}
    | PI                                        {$$ = new VarFuncName("pi", yylloc.first_line);}
    | expression CON expression                 {$$ = new BinaryOperation("@", $1, $3, yylloc.first_line);}
    | expression CSP expression                 {$$ = new BinaryOperation("@@", $1, $3, yylloc.first_line);}
    | expression LTH expression                 {$$ = new BinaryOperation("<", $1, $3, yylloc.first_line);}
    | expression GTH expression                 {$$ = new BinaryOperation(">", $1, $3, yylloc.first_line);}
    | expression LEQ expression                 {$$ = new BinaryOperation("<=", $1, $3, yylloc.first_line);}
    | expression GEQ expression                 {$$ = new BinaryOperation(">=", $1, $3, yylloc.first_line);}
    | expression EQU expression                 {$$ = new BinaryOperation("==", $1, $3, yylloc.first_line);}
    | expression NEQ expression                 {$$ = new BinaryOperation("!=", $1, $3, yylloc.first_line);}
    | expression AND expression                 {$$ = new BinaryOperation("&", $1, $3, yylloc.first_line);}
    | expression OR expression                  {$$ = new BinaryOperation("|", $1, $3, yylloc.first_line);}
    | NOT expression                            {$$ = new UnaryOperation("!", $2, yylloc.first_line);}
;

id:
    ID                                  {$$ = new VarFuncName(*$1, yylloc.first_line);}
;   

block_expression: 
    '{' block_body '}'                  {$$ = new Block(*$2, yylloc.first_line);}
;   

block_body: 
    /* nada */                          {$$ = new std::vector<ASTNode*>();}
    | statement                         {$$ = new std::vector<ASTNode*>(); $$->push_back($1);}
    | block_body statement              {$1->push_back($2); $$ = $1;}
;   

params: 
    /* nada */                          {$$ = new std::vector<Parameter>();}
    | ID                                {Parameter p; p.name = *$1; p.type = ""; $$ = new std::vector<Parameter>(); $$->push_back(p);}
    | ID COLON ID                       {Parameter p; p.name = *$1; p.type = *$3; $$ = new std::vector<Parameter>(); $$->push_back(p);}
    | params ',' ID                     {Parameter p; p.name = *$3; p.type = ""; $1->push_back(p); $$ = $1;}
    | params ',' ID COLON ID            {Parameter p; p.name = *$3; p.type = *$5; $1->push_back(p); $$ = $1;}
;   

func_call: 
    ID '(' args ')'                     {$$ = new FuncCall(*$1, *$3, yylloc.first_line);}
;

args: 
    /* nada */                          {$$ = new std::vector<ASTNode*>();}
    | expression                        {$$ = new std::vector<ASTNode*>(); $$->push_back($1);}
    | args ',' expression               {$1->push_back($3); $$ = $1;}
;

assign:
    id REASSIGN expression              {$$ = new Assignment($1, $3, yylloc.first_line);}
    | self_call REASSIGN expression     {$$ = new Assignment($1, $3, yylloc.first_line);}
;

let:
    LET declaration IN body                    {$$ = new LetExpression($2, $4, yylloc.first_line);}
    | LET declaration IN '(' body ')'          {$$ = new LetExpression($2, $5, yylloc.first_line);}
    | LET declaration IN body ';'              {$$ = new LetExpression($2, $4, yylloc.first_line);}
    | LET declaration IN '(' body ')' ';'      {$$ = new LetExpression($2, $5, yylloc.first_line);}
    | LET declaration ',' let                  {$$ = new LetExpression($2, $4, yylloc.first_line);}
;

declaration:
    ID ASSIGN expression                              {LetDeclaration d; d.name = *$1; d.declaredType = ""; d.initializer = $3; $$ = new std::vector<LetDeclaration>(); $$->push_back(d);}
    | ID COLON ID ASSIGN expression                   {LetDeclaration d; d.name = *$1; d.declaredType = *$3; d.initializer = $5; $$ = new std::vector<LetDeclaration>(); $$->push_back(d);}
    | declaration ',' ID ASSIGN expression            {LetDeclaration d; d.name = *$3; d.declaredType = ""; d.initializer = $5; $1->push_back(d); $$ = $1;}
    | declaration ',' ID COLON ID ASSIGN expression   {LetDeclaration d; d.name = *$3; d.declaredType = *$5; d.initializer = $7; $1->push_back(d); $$ = $1;}
;

body:
    statement                       {$$ = $1;}
    | expression                    {$$ = $1;}
;

if_e:
    if_head                         {$$ = new IfExpression($1, nullptr, yylloc.first_line);}
    | if_head ELSE body             {$$ = new IfExpression($1, $3, yylloc.first_line);}
;

if_head:
    IF '(' expression ')' body                  {IfBranch b; b.condition = $3; b.body = $5; $$ = new std::vector<IfBranch>(); $$->push_back(b);}
    | if_head ELIF '(' expression ')' body      {IfBranch b; b.condition = $4; b.body = $6; $1->push_back(b); $$ = $1;}
;

while_e:
    WHILE '(' expression ')' body               {$$ = new WhileLoop($3, $5, yylloc.first_line);}
;

for_e:
    FOR '(' ID IN RANGE '(' expression ',' expression ')' ')' body      { $$ = new ForLoop(*$3, $7, $9, $12, yylloc.first_line);}
;

type_declaration:
    TYPE ID '(' params ')' '{' type_body '}'                            {$$ = new TypeDeclaration(*$2, $4, $7, std::nullopt, std::vector<ASTNode*>(), yylloc.first_line);}
    | TYPE ID '{' type_body '}'                                         {$$ = new TypeDeclaration(*$2, new std::vector<Parameter>(), $4, std::nullopt, std::vector<ASTNode*>(), yylloc.first_line);}
    | TYPE ID '(' params ')' INHERITS ID '(' args ')' '{' type_body '}' {$$ = new TypeDeclaration(*$2, $4, $12, *$7, *$9, yylloc.first_line);}
    | TYPE ID INHERITS ID '(' args ')' '{' type_body '}'                {$$ = new TypeDeclaration(*$2, new std::vector<Parameter>(), $9, std::make_optional(*$4), *$6, yylloc.first_line);}
    | TYPE ID '(' params ')' INHERITS ID '{' type_body '}'              {$$ = new TypeDeclaration(*$2, $4, $9, std::make_optional(*$7), std::vector<ASTNode*>(), yylloc.first_line);}
    | TYPE ID INHERITS ID '{' type_body '}'                             {$$ = new TypeDeclaration(*$2, new std::vector<Parameter>(), $6, std::make_optional(*$4), std::vector<ASTNode*>(), yylloc.first_line);}
;

type_body
    : /* nada */                                {$$ = new TypeBody(new std::vector<AttributeDeclaration>(), new std::vector<MethodDeclaration>());}
    | attribute_declaration                     {$$ = new TypeBody($1, new std::vector<MethodDeclaration>());}
    | method_declaration                        {$$ = new TypeBody(new std::vector<AttributeDeclaration>(), $1);}
    | attribute_declaration method_declaration  {$$ = new TypeBody($1, $2);}
;

attribute_declaration:
    ID ASSIGN expression ';'                                    {$$ = new std::vector<AttributeDeclaration>(); $$->push_back(AttributeDeclaration(*$1, "", $3));}
    | ID COLON ID ASSIGN expression ';'                         {$$ = new std::vector<AttributeDeclaration>(); $$->push_back(AttributeDeclaration(*$1, *$3, $5));}
    | attribute_declaration ID ASSIGN expression ';'            {$1->push_back(AttributeDeclaration(*$2, "", $4)); $$ = $1;}
    | attribute_declaration ID COLON ID ASSIGN expression ';'   {$1->push_back(AttributeDeclaration(*$2, *$4, $6)); $$ = $1;}
;

method_declaration:
    ID '(' params ')' INLINE expression ';'                                 {$$ = new std::vector<MethodDeclaration>(); $$->push_back(MethodDeclaration(*$1, $3, $6, ""));}
    | ID '(' params ')' block_expression ';'                                {$$ = new std::vector<MethodDeclaration>(); $$->push_back(MethodDeclaration(*$1, $3, $5, ""));}
    | method_declaration ID '(' params ')' INLINE expression ';'            {$1->push_back(MethodDeclaration(*$2, $4, $7, "")); $$ = $1;}
    | method_declaration ID '(' params ')' block_expression ';'             {$1->push_back(MethodDeclaration(*$2, $4, $6, "")); $$ = $1;}
    | ID '(' params ')' COLON ID INLINE expression ';'                      {$$ = new std::vector<MethodDeclaration>(); $$->push_back(MethodDeclaration(*$1, $3, $8, *$6));}
    | ID '(' params ')' COLON ID block_expression ';'                       {$$ = new std::vector<MethodDeclaration>(); $$->push_back(MethodDeclaration(*$1, $3, $7, *$6));}
    | method_declaration ID '(' params ')' COLON ID INLINE expression ';'   {$1->push_back(MethodDeclaration(*$2, $4, $9, *$7)); $$ = $1;}
    | method_declaration ID '(' params ')' COLON ID block_expression ';'    {$1->push_back(MethodDeclaration(*$2, $4, $8, *$7)); $$ = $1;}
;

self_call:
    SELF DOT ID                 {$$ = new SelfCall(*$3, yylloc.first_line);}
;

method_call: 
    ID DOT ID '(' args ')'      { $$ = new MethodCall(*$1, *$3, *$5, true, yylloc.first_line); }  // Método
    | ID DOT ID                 { $$ = new MethodCall(*$1, *$3, std::vector<ASTNode*>(), false, yylloc.first_line); }  // Atributo
    | SELF DOT ID '(' args ')'  { $$ = new MethodCall("self", *$3, *$5, true, yylloc.first_line); }
    | SELF DOT ID               { $$ = new MethodCall("self", *$3, std::vector<ASTNode*>(), false, yylloc.first_line); }
;

base_call:
    BASE '(' args ')'           {$$ = new BaseCall(*$3, yylloc.first_line);}
;

new_instance:
    NEW ID '(' args ')'         {$$ = new InitInstance(*$2, *$4, yylloc.first_line);}
;

%%

// Función para obtener el nombre del token
const char* token_name(int token) {
    switch (token) {
        case NUMBER: return "NUMBER";
        case STRING: return "STRING";
        case BOOLEAN: return "BOOLEAN";
        case NULL_VAL: return "null";
        case ID: return "ID";
        case INLINE: return "inline";
        case ASSIGN: return "=";
        case REASSIGN: return "<-";
        case ADD: return "+";
        case SUB: return "-";
        case MUL: return "*";
        case DIV: return "/";
        case MOD: return "%";
        case POW: return "^";
        case LTH: return "<";
        case GTH: return ">";
        case LEQ: return "<=";
        case GEQ: return ">=";
        case EQU: return "==";
        case NEQ: return "!=";
        case AND: return "&";
        case OR: return "|";
        case NOT: return "!";
        case CON: return "@";
        case CSP: return "@@";
        case SIN: return "sin";
        case COS: return "cos";
        case MAX: return "max";
        case MIN: return "min";
        case SQRT: return "sqrt";
        case EXP: return "exp";
        case LOG: return "log";
        case RANDOM: return "rand";
        case PRINT: return "print";
        case PI: return "PI";
        case E: return "E";
        case FUN: return "function";
        case LET: return "let";
        case IN: return "in";
        case IF: return "if";
        case ELIF: return "elif";
        case ELSE: return "else";
        case FOR: return "for";
        case WHILE: return "while";
        case RANGE: return "..";
        case TYPE: return "type";
        case NEW: return "new";
        case SELF: return "self";
        case INHERITS: return "inherits";
        case BASE: return "base";
        case COLON: return ":";
        case ',': return "','";
        case ';': return "';'";
        case '.': return "'.'";
        case '(': return "'('";
        case ')': return "')'";
        case '{': return "'{'";
        case '}': return "'}'";
        case 0: return "end of file";
        default: return "unknown token";
    }
}

void yyerror(const char *msg)   
{
    extern int yychar; // Token actual provisto por Bison
    if (yychar == -1) 
    {
        fprintf(stderr, "Error en linea %d, columna %d: %s\n", 
                yylloc.first_line, yylloc.first_column, msg);
    }
    else 
    {
        fprintf(stderr, "Error en linea %d, columna %d: %s. Token inesperado: '%s'\n", 
                yylloc.first_line, yylloc.first_column, msg, token_name(yychar));
    }
}