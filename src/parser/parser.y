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

#define TRACE(EXPR) std::cout << "elem_expr: " << *EXPR << std::endl;

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
%token <boolean> BOOL
%token NULL_VAL
%token <str> ID
%token ',' ';' '.'
%token '(' ')'
%token '{' '}'
%token LAMBDA 
%token '='
%token REASSIGN
%token ADD
%token SUB
%token MUL
%token DIV
%token MOD
%token POW
%token LT
%token GT 
%token LE 
%token GE 
%token EQ
%token NE
%token AND
%token OR 
%token NOT
%token CONCAT
%token CONCAT_SPACE
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
%token COLON

// Tipos
%type <node> statement
%type <node> expression
%type <node> elem_expr
%type <node> id
%type <node> block_expr
%type <node> func_call_expr
%type <node> assign_expr
%type <node> let_expr
%type <node> if_expr
%type <node> while_expr 
%type <node> for_expr
%type <node> body
%type <node> method_call
%type <node> base_call
%type <node> new_instance
%type <node> self_call
%type <list> block_body
%type <list> args
%type <param> params
%type <if_branch> if_head
%type <let_decl> decl
%type <node> type_decl
%type <attr_decl> attribute_decl
%type <method_decl> method_decl
%type <type_body> type_body

// Precedencia
%left CONCAT CONCAT_SPACE
%right NOT
%left AND OR 
%left LT GT LE GE EQ NE
%right  POW SIN COS MIN MAX SQRT LOG EXP RANDOM
%left ADD SUB
%left MUL DIV MOD

%%


// PRODUCCIONES

program:
    /* nada */
    | program statement                                 {root.push_back($2);}
;

statement:
    ';'                                                 {$$ = NULL;}
    | expression ';'                                     {$$ = $1;}
    | PRINT '(' expression ')' ';'                      {std::vector<ASTNode*> args = vectorize($3, nullptr, 1); $$ = new BuiltInFunc("print", args, yylloc.first_line); }
    | type_decl                                         {$$ = $1;}
    | block_expr                                        {$$ = $1;}
    | FUNC ID '(' params ')' LAMBDA body ';'            {$$ = new FuncDeclaration(*$2, $4, $7, true, yylloc.first_line);}
    | FUNC ID '(' params ')' block_expr ';'             {$$ = new FuncDeclaration(*$2, $4, $6, false, yylloc.first_line);}
    | FUNC ID '(' params ')' COLON ID LAMBDA body ';'   {$$ = new FuncDeclaration(*$2, $4, $9, true, yylloc.first_line);}
    | FUNC ID '(' params ')' COLON ID block_expr ';'    {$$ = new FuncDeclaration(*$2, $4, $8, false, yylloc.first_line);}
    | FUNC ID '(' params ')' LAMBDA body                {$$ = new FuncDeclaration(*$2, $4, $7, true, yylloc.first_line);}
    | FUNC ID '(' params ')' block_expr                 {$$ = new FuncDeclaration(*$2, $4, $6, false, yylloc.first_line);}
    | FUNC ID '(' params ')' COLON ID LAMBDA body       {$$ = new FuncDeclaration(*$2, $4, $9, true, yylloc.first_line);}
    | FUNC ID '(' params ')' COLON ID block_expr        {$$ = new FuncDeclaration(*$2, $4, $8, false, yylloc.first_line);}
    | let_expr                                          {$$ = $1;}
    | while_expr                                        {$$ = $1;}
    | for_expr                                          {$$ = $1;}
;

expression:
    NUMBER                  { $$ = new DataType(std::to_string($1), "Number", yylloc.first_line); }
    | STRING                { $$ = new DataType(*$1, "String", yylloc.first_line);  }
    | BOOL                  { $$ = new DataType($1 ? "true" : "false", "Boolean", yylloc.first_line); }
    | NULL_VAL              { $$ = new DataType("null", "Null", yylloc.first_line); }
    | ID                    { $$ = new VarFuncName(*$1, yylloc.first_line); }
    | self_call             { $$ = $1; }
    | new_instance          { $$ = $1; }
    | elem_expr             { $$ = $1; }
    | block_expr            { $$ = $1; }
    | func_call_expr        { $$ = $1; }
    | method_call           { $$ = $1; }
    | base_call             { $$ = $1; }
    | assign_expr           { $$ = $1; }
    | let_expr              { $$ = $1; }
    | if_expr               { $$ = $1; }
    | while_expr            { $$ = $1; }
    | for_expr              { $$ = $1; }
;

elem_expr:
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
    | expression CONCAT expression              {$$ = new BinaryOperation("@", $1, $3, yylloc.first_line);}
    | expression CONCAT_SPACE expression        {$$ = new BinaryOperation("@@", $1, $3, yylloc.first_line);}
    | expression LT expression                  {$$ = new BinaryOperation("<", $1, $3, yylloc.first_line);}
    | expression GT expression                  {$$ = new BinaryOperation(">", $1, $3, yylloc.first_line);}
    | expression LE expression                  {$$ = new BinaryOperation("<=", $1, $3, yylloc.first_line);}
    | expression GE expression                  {$$ = new BinaryOperation(">=", $1, $3, yylloc.first_line);}
    | expression EQ expression                  {$$ = new BinaryOperation("==", $1, $3, yylloc.first_line);}
    | expression NE expression                  {$$ = new BinaryOperation("!=", $1, $3, yylloc.first_line);}
    | expression AND expression                 {$$ = new BinaryOperation("&", $1, $3, yylloc.first_line);}
    | expression OR expression                  {$$ = new BinaryOperation("|", $1, $3, yylloc.first_line);}
    | NOT expression                            {$$ = new UnaryOperation("!", $2, yylloc.first_line);}
;

id:
    ID                                  {$$ = new VarFuncName(*$1, yylloc.first_line);}
;   

block_expr: 
    '{' block_body '}'                  {$$ = new Block(*$2, yylloc.first_line);}
;   

block_body: 
    /* nada */                          {$$ = new std::vector<ASTNode*>();}
    | statement                         {$$ = new std::vector<ASTNode*>(); $$->push_back($1);}
    | block_body statement              {$1->push_back($2); $$ = $1;}
;   

params: 
    /* nada */                          {$$ = new std::vector<Parameter>();}
    | ID                                {Parameter p; p.name = *$1; $$ = new std::vector<Parameter>(); $$->push_back(p);}
    | ID COLON ID                       {Parameter p; p.name = *$1; $$ = new std::vector<Parameter>(); $$->push_back(p);}
    | params ',' ID                     {Parameter p; p.name = *$3; $1->push_back(p); $$ = $1;}
    | params ',' ID COLON ID            {Parameter p; p.name = *$3; $1->push_back(p); $$ = $1;}
;   

func_call_expr: 
    ID '(' args ')'                     {$$ = new FuncCall(*$1, *$3, yylloc.first_line);}
;

args: 
    /* nada */                          {$$ = new std::vector<ASTNode*>();}
    | expression                        {$$ = new std::vector<ASTNode*>(); $$->push_back($1);}
    | args ',' expression               {$1->push_back($3); $$ = $1;}
;

assign_expr:
    id REASSIGN expression              {$$ = new Assignment($1, $3, yylloc.first_line);}
    | self_call REASSIGN expression     {$$ = new Assignment($1, $3, yylloc.first_line);}
;

let_expr:
    LET decl IN body                    {$$ = new LetExpression($2, $4, yylloc.first_line);}
    | LET decl IN '(' body ')'          {$$ = new LetExpression($2, $5, yylloc.first_line);}
    | LET decl IN body ';'              {$$ = new LetExpression($2, $4, yylloc.first_line);}
    | LET decl IN '(' body ')' ';'      {$$ = new LetExpression($2, $5, yylloc.first_line);}
    | LET decl ',' let_expr             {$$ = new LetExpression($2, $4, yylloc.first_line);}
;

decl:
    ID '=' expression                       {LetDeclaration d; d.name = *$1; d.initializer = $3; $$ = new std::vector<LetDeclaration>(); $$->push_back(d);}
    | ID COLON ID '=' expression            {LetDeclaration d; d.name = *$1; d.initializer = $5; $$ = new std::vector<LetDeclaration>(); $$->push_back(d);}
    | decl ',' ID '=' expression            {LetDeclaration d; d.name = *$3; d.initializer = $5; $1->push_back(d); $$ = $1;}
    | decl ',' ID COLON ID '=' expression   {LetDeclaration d; d.name = *$3; d.initializer = $7; $1->push_back(d); $$ = $1;}
;

body:
    statement                       {$$ = $1;}
    | expression                    {$$ = $1;}
;

if_expr:
    if_head                         {$$ = new IfExpression($1, nullptr, yylloc.first_line);}
    | if_head ELSE body             {$$ = new IfExpression($1, $3, yylloc.first_line);}
;

if_head:
    IF '(' expression ')' body                  {IfBranch b; b.condition = $3; b.body = $5; $$ = new std::vector<IfBranch>(); $$->push_back(b);}
    | if_head ELIF '(' expression ')' body      {IfBranch b; b.condition = $4; b.body = $6; $1->push_back(b); $$ = $1;}
;

while_expr:
    WHILE '(' expression ')' body               {$$ = new WhileLoop($3, $5, yylloc.first_line);}
;

for_expr:
    FOR '(' ID IN RANGE '(' expression ',' expression ')' ')' body      { $$ = new ForLoop(*$3, $7, $9, $12, yylloc.first_line);}
;

type_decl:
    TYPE ID '(' params ')' '{' type_body '}'                            {$$ = new TypeDeclaration(*$2, $4, $7, std::nullopt, std::vector<ASTNode*>(), yylloc.first_line);}
    | TYPE ID '{' type_body '}'                                         {$$ = new TypeDeclaration(*$2, new std::vector<Parameter>(), $4, std::nullopt, std::vector<ASTNode*>(), yylloc.first_line);}
    | TYPE ID '(' params ')' INHERITS ID '(' args ')' '{' type_body '}' {$$ = new TypeDeclaration(*$2, $4, $12, *$7, *$9, yylloc.first_line);}
    | TYPE ID INHERITS ID '(' args ')' '{' type_body '}'                {$$ = new TypeDeclaration(*$2, new std::vector<Parameter>(), $9, std::make_optional(*$4), *$6, yylloc.first_line);}
    | TYPE ID '(' params ')' INHERITS ID '{' type_body '}'              {$$ = new TypeDeclaration(*$2, $4, $9, std::make_optional(*$7), std::vector<ASTNode*>(), yylloc.first_line);}
    | TYPE ID INHERITS ID '{' type_body '}'                             {$$ = new TypeDeclaration(*$2, new std::vector<Parameter>(), $6, std::make_optional(*$4), std::vector<ASTNode*>(), yylloc.first_line);}
;

type_body
    : /* nada */                    {$$ = new TypeBody(new std::vector<AttributeDeclaration>(), new std::vector<MethodDeclaration>());}
    | attribute_decl                {$$ = new TypeBody($1, new std::vector<MethodDeclaration>());}
    | method_decl                   {$$ = new TypeBody(new std::vector<AttributeDeclaration>(), $1);}
    | attribute_decl method_decl    {$$ = new TypeBody($1, $2);}
;

attribute_decl:
    ID '=' expression ';'                           {$$ = new std::vector<AttributeDeclaration>(); $$->push_back(AttributeDeclaration(*$1, $3));}
    | ID COLON ID '=' expression ';'                {$$ = new std::vector<AttributeDeclaration>(); $$->push_back(AttributeDeclaration(*$1, $5));}
    | attribute_decl ID '=' expression ';'          {$1->push_back(AttributeDeclaration(*$2, $4)); $$ = $1;}
    | attribute_decl ID COLON ID '=' expression ';' {$1->push_back(AttributeDeclaration(*$2, $6)); $$ = $1;}
;

method_decl:
    ID '(' params ')' LAMBDA expression ';'                         {$$ = new std::vector<MethodDeclaration>(); $$->push_back(MethodDeclaration(*$1, $3, $6));}
    | ID '(' params ')' block_expr ';'                              {$$ = new std::vector<MethodDeclaration>(); $$->push_back(MethodDeclaration(*$1, $3, $5));}
    | method_decl ID '(' params ')' LAMBDA expression ';'           {$1->push_back(MethodDeclaration(*$2, $4, $7)); $$ = $1;}
    | method_decl ID '(' params ')' block_expr ';'                  {$1->push_back(MethodDeclaration(*$2, $4, $6)); $$ = $1;}
    | ID '(' params ')' COLON ID LAMBDA expression ';'              {$$ = new std::vector<MethodDeclaration>(); $$->push_back(MethodDeclaration(*$1, $3, $8));}
    | ID '(' params ')' COLON ID block_expr ';'                     {$$ = new std::vector<MethodDeclaration>(); $$->push_back(MethodDeclaration(*$1, $3, $7));}
    | method_decl ID '(' params ')' COLON ID LAMBDA expression ';'  {$1->push_back(MethodDeclaration(*$2, $4, $9)); $$ = $1;}
    | method_decl ID '(' params ')' COLON ID block_expr ';'         {$1->push_back(MethodDeclaration(*$2, $4, $8)); $$ = $1;}
;

self_call:
    SELF '.' ID                 {$$ = new SelfCall(*$3, yylloc.first_line);}
;

method_call:
    ID '.' ID '(' args ')'      {$$ = new MethodCall(*$1, *$3, *$5, yylloc.first_line);}
    | SELF '.' ID '(' args ')'  {$$ = new MethodCall("self", *$3, *$5, yylloc.first_line);}
;

base_call:
    BASE '(' args ')'           {$$ = new BaseCall(*$3, yylloc.first_line);}
;

new_instance:
    NEW ID '(' args ')'         {$$ = new InitInstance(*$2, *$4, yylloc.first_line);}
;

%%

void yyerror(const char *msg)   {fprintf(stderr, "Error en linea %d, columna %d: %s\n", yylloc.first_line, yylloc.first_column, msg);}