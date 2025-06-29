%{
#include "../../.build/parser.tab.hpp"     
#include <string>

int yycolumn = 1;
int lexical_error = 0;

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

[_]+[a-zA-Z0-9_]+       { lexical_error = 1; fprintf(stderr, "Error lexico. Token invalido: '%s' en %d:%d\n", yytext, yylineno, yylloc.first_column); }
[0-9]+[a-zA-Z_]+        { lexical_error = 1; fprintf(stderr, "Error lexico. Token invalido: '%s' en %d:%d\n", yytext, yylineno, yylloc.first_column); }

[0-9]+(\.[0-9]+)?       { 
                            yylval.num = atof(yytext); 
                            return NUMBER; 
                        }
\"(?:\\.|[^"])*\"       {
                            std::string raw = yytext;
                            yylval.str = new std::string(raw.substr(1, raw.size() - 2));
                            return STRING;
                        }
                        
"True"                  {
                            yylval.boolean = true;
                            return BOOLEAN;
                        }
"False"                 {
                            yylval.boolean = false;
                            return BOOLEAN;
                        }
"Null"                  { 
                            return NULL_VAL; 
                        }
[+]                     { return ADD; }
[-]                     { return SUB; }
[*]                     { return MUL; }
[/]                     { return DIV; }
[%]                     { return MOD; }
[\^]                    { return POW; }
[*][*]                  { return POW; }
[<]                     { return LTH; }
[>]                     { return GTH; }
"<="                    { return LEQ; }
">="                    { return GEQ; }
"=="                    { return EQU; }
"!="                    { return NEQ; }
"&"                     { return AND; }
"!"                     { return NOT; }
[|]                     { return OR; }
"@"                     { return CON; }
"@@"                    { return CSP; }
"sin"                   { return SIN; }
"cos"                   { return COS; }
"max"                   { return MAX; }
"min"                   { return MIN; }
"sqrt"                  { return SQRT; }
"exp"                   { return EXP; }
"log"                   { return LOG; }
"rand"                  { return RANDOM; }
":"                     { return COLON; }
"print"                 { return PRINT; }
"PI"                    { return PI; }
"E"                     { return E; }
"function"              { return FUN; }
"=>"                    { return INLINE; }
"let"                   { return LET; }
"in"                    { return IN; }
"="                     { return ASSIGN; }
":="                    { return REASSIGN; }
"if"                    { return IF; }
"else"                  { return ELSE; }
"elif"                  { return ELIF; }
"for"                   { return FOR; }
"while"                 { return WHILE; }
"range"                 { return RANGE; }
"type"                  { return TYPE; }
"new"                   { return NEW; }
"self"                  { return SELF; }
"inherits"              { return INHERITS; }
"base"                  { return BASE; }
"."                     { return DOT; }      
"("                     { return '('; }
")"                     { return ')'; }
"{"                     { return '{'; }
"}"                     { return '}'; }
","                     { return ','; }
[a-zA-Z][a-zA-Z0-9_]*  {
                            yylval.str = new std::string(yytext);
                            return ID;
                        }
";"                     { 
                            return ';'; 
                        }


[ \t]+                  { /* Espacio en blanco manejado por YY_USER_ACTION */ }  // Ignorar espacios
\n                      { yycolumn = 1; }        // Resetear columna al salto de linea
.                       { 
                            lexical_error = 1;
                            fprintf(stderr, "Error lexico: '%s' en %d:%d\n", yytext, yylineno, yylloc.first_column); 
                        }
%%