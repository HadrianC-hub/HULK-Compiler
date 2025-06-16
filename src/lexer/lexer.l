%{
#include "../../.build/parser.tab.hpp"     
#include <string>

int yycolumn = 1;
#define YY_USER_ACTION \
    yylloc.first_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_line = yylineno; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;  // Actualizar despues de registrar la posicion
%}

%option noyywrap         
%option yylineno   
%option nounput       

%%

[_]+[a-zA-Z0-9_]+       { fprintf(stderr, "Error lexico. Variable invalida: '%s' en %d:%d\n", yytext, yylineno, yylloc.first_column); }
[0-9]+[a-zA-Z_]+        { fprintf(stderr, "Error lexico. Variable invalida: '%s' en %d:%d\n", yytext, yylineno, yylloc.first_column); }

[0-9]+(\.[0-9]+)?       { 
                            yylval.num = atof(yytext); 
                            printf("Token: %s → Linea %d, Columna %d\n", yytext, yylineno, yylloc.first_column);
                            return NUMBER; 
                        }
\"(?:\\.|[^"])*\"       {
                            std::string raw = yytext;
                            yylval.str = new std::string(raw.substr(1, raw.size() - 2));
                            printf("String: %s  → Linea %d, Columna %d\n", yytext, yylineno, yylloc.first_column);
                            return STRING;
                        }
                        
"True"                  {
                            yylval.boolean = true;
                            printf("Boolean: %s  → Linea %d, Columna %d\n", yytext, yylineno, yylloc.first_column);
                            return BOOL;
                        }
"False"                 {
                            yylval.boolean = false;
                            printf("Boolean: %s  → Linea %d, Columna %d\n", yytext, yylineno, yylloc.first_column);
                            return BOOL;
                        }
"Null"                  { 
                            printf("Null value → Linea %d, Columna %d\n", yylineno, yylloc.first_column);
                            return NULL_VAL; 
                        }
[+]                     { printf(" TOKEN + \n"); return ADD; }
[-]                     { printf(" TOKEN - \n"); return SUB; }
[*]                     { printf(" TOKEN * \n"); return MUL; }
[/]                     { printf(" TOKEN / \n"); return DIV; }
[%]                     { printf(" TOKEN mod \n"); return MOD; }
[\^]                    { printf(" TOKEN ^ \n"); return POW; }
[*][*]                  { printf(" TOKEN ** \n"); return POW; }
[<]                     { printf(" TOKEN < \n"); return LT; }
[>]                     { printf(" TOKEN > \n"); return GT; }
"<="                    { printf(" TOKEN <= \n"); return LE; }
">="                    { printf(" TOKEN >= \n"); return GE; }
"=="                    { printf(" TOKEN == \n"); return EQ; }
"!="                    { printf(" TOKEN != \n"); return NE; }
"&"                     { printf(" TOKEN & \n"); return AND; }
"!"                     { printf(" TOKEN ! \n"); return NOT; }
[|]                     { printf(" TOKEN | \n"); return OR; }
"@"                     { printf(" TOKEN @ \n"); return CONCAT; }
"@@"                    { printf(" TOKEN @@ \n"); return CONCAT_SPACE; }
"sin"                   { printf(" TOKEN sin \n"); return SIN; }
"cos"                   { printf(" TOKEN cos \n"); return COS; }
"max"                   { printf(" TOKEN max \n"); return MAX; }
"min"                   { printf(" TOKEN min \n"); return MIN; }
"sqrt"                  { printf(" TOKEN sqrt \n"); return SQRT; }
"exp"                   { printf(" TOKEN exp \n"); return EXP; }
"log"                   { printf(" TOKEN log \n"); return LOG; }
"rand"                  { printf(" TOKEN rand \n"); return RANDOM; }
":"                     { return COLON; }
"print"                 { printf("TOKEN print\n"); return PRINT; }
"PI"                    { printf("TOKEN PI\n"); return PI; }
"E"                     { printf("TOKEN E\n"); return E; }
"function"              { printf("TOKEN DEF Funcion\n"); return FUNC; }
"=>"                    { printf("TOKEN lambda \n"); return LAMBDA; }
"let"                   { printf("TOKEN DEF Variable\n"); return LET; }
"in"                    { printf("TOKEN DEF Scope\n"); return IN; }
"="                     { printf("TOKEN asignar valor\n"); return '='; }
":="                    { printf("TOKEN reasignar valor\n"); return REASSIGN; }
"if"                    { printf("TOKEN if\n"); return IF; }
"else"                  { printf("TOKEN else\n"); return ELSE; }
"elif"                  { printf("TOKEN elif\n"); return ELIF; }
"for"                   { printf("TOKEN for\n"); return FOR; }
"while"                 { printf("TOKEN while\n"); return WHILE; }
"range"                 { printf("TOKEN range\n"); return RANGE; }
"type"                  { printf("TOKEN type\n"); return TYPE; }
"new"                   { printf("TOKEN new\n"); return NEW; }
"self"                  { printf("TOKEN self\n"); return SELF; }
"inherits"              { printf("TOKEN inherits\n"); return INHERITS; }
"base"                  { printf("TOKEN base\n"); return BASE; }
"."                     { printf("TOKEN  . \n"); return '.'; }      
"("                     { printf("TOKEN ( \n"); return '('; }
")"                     { printf("TOKEN ) \n"); return ')'; }
"{"                     { printf("TOKEN { \n"); return '{'; }
"}"                     { printf("TOKEN } \n"); return '}'; }
","                     { printf("TOKEN , \n"); return ','; }
[a-zA-Z_][a-zA-Z0-9_]*  {
                            yylval.str = new std::string(yytext);
                            printf("ID: %s  → Linea %d, Columna %d\n", yytext, yylineno, yylloc.first_column);
                            return ID;
                        }
";"                     { 
                            printf("';' symbol  → Linea %d, Columna %d\n", yylineno, yylloc.first_column);
                            return ';'; 
                        }


[ \t]+                  { /* Whitespace handled by YY_USER_ACTION */ }  // Ignorar espacios
\n                      { yycolumn = 1; }        // Resetear columna al salto de linea
.                       { 
                            fprintf(stderr, "Error lexico: '%s' en %d:%d\n", yytext, yylineno, yylloc.first_column); 
                        }
%%