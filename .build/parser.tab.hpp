/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_BUILD_PARSER_TAB_HPP_INCLUDED
# define YY_YY_BUILD_PARSER_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 33 "src/parser/parser.y"

    #include <string>
    #include <iostream>
    #include <cmath>
    #include <vector>
    #include "../ast/AST.hpp"

#line 57 ".build/parser.tab.hpp"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    NUMBER = 258,                  /* NUMBER  */
    STRING = 259,                  /* STRING  */
    BOOLEAN = 260,                 /* BOOLEAN  */
    NULL_VAL = 261,                /* NULL_VAL  */
    ID = 262,                      /* ID  */
    DOT = 263,                     /* DOT  */
    INLINE = 264,                  /* INLINE  */
    ASSIGN = 265,                  /* ASSIGN  */
    REASSIGN = 266,                /* REASSIGN  */
    ADD = 267,                     /* ADD  */
    SUB = 268,                     /* SUB  */
    MUL = 269,                     /* MUL  */
    DIV = 270,                     /* DIV  */
    MOD = 271,                     /* MOD  */
    POW = 272,                     /* POW  */
    LTH = 273,                     /* LTH  */
    GTH = 274,                     /* GTH  */
    LEQ = 275,                     /* LEQ  */
    GEQ = 276,                     /* GEQ  */
    EQU = 277,                     /* EQU  */
    NEQ = 278,                     /* NEQ  */
    AND = 279,                     /* AND  */
    OR = 280,                      /* OR  */
    NOT = 281,                     /* NOT  */
    CON = 282,                     /* CON  */
    CSP = 283,                     /* CSP  */
    SIN = 284,                     /* SIN  */
    COS = 285,                     /* COS  */
    MAX = 286,                     /* MAX  */
    MIN = 287,                     /* MIN  */
    SQRT = 288,                    /* SQRT  */
    EXP = 289,                     /* EXP  */
    LOG = 290,                     /* LOG  */
    RANDOM = 291,                  /* RANDOM  */
    PRINT = 292,                   /* PRINT  */
    PI = 293,                      /* PI  */
    E = 294,                       /* E  */
    FUN = 295,                     /* FUN  */
    LET = 296,                     /* LET  */
    IN = 297,                      /* IN  */
    IF = 298,                      /* IF  */
    ELIF = 299,                    /* ELIF  */
    ELSE = 300,                    /* ELSE  */
    FOR = 301,                     /* FOR  */
    WHILE = 302,                   /* WHILE  */
    RANGE = 303,                   /* RANGE  */
    TYPE = 304,                    /* TYPE  */
    NEW = 305,                     /* NEW  */
    SELF = 306,                    /* SELF  */
    INHERITS = 307,                /* INHERITS  */
    BASE = 308,                    /* BASE  */
    COLON = 309                    /* COLON  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 44 "src/parser/parser.y"

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

#line 142 ".build/parser.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yyparse (void);


#endif /* !YY_YY_BUILD_PARSER_TAB_HPP_INCLUDED  */
