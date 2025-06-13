/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "src/parser/parser.y"


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


#line 105 ".build/parser.tab.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_NUMBER = 3,                     /* NUMBER  */
  YYSYMBOL_STRING = 4,                     /* STRING  */
  YYSYMBOL_BOOL = 5,                       /* BOOL  */
  YYSYMBOL_NULL_VAL = 6,                   /* NULL_VAL  */
  YYSYMBOL_ID = 7,                         /* ID  */
  YYSYMBOL_8_ = 8,                         /* ','  */
  YYSYMBOL_9_ = 9,                         /* ';'  */
  YYSYMBOL_10_ = 10,                       /* '.'  */
  YYSYMBOL_11_ = 11,                       /* '('  */
  YYSYMBOL_12_ = 12,                       /* ')'  */
  YYSYMBOL_13_ = 13,                       /* '{'  */
  YYSYMBOL_14_ = 14,                       /* '}'  */
  YYSYMBOL_LAMBDA = 15,                    /* LAMBDA  */
  YYSYMBOL_16_ = 16,                       /* '='  */
  YYSYMBOL_REASSIGN = 17,                  /* REASSIGN  */
  YYSYMBOL_ADD = 18,                       /* ADD  */
  YYSYMBOL_SUB = 19,                       /* SUB  */
  YYSYMBOL_MUL = 20,                       /* MUL  */
  YYSYMBOL_DIV = 21,                       /* DIV  */
  YYSYMBOL_MOD = 22,                       /* MOD  */
  YYSYMBOL_POW = 23,                       /* POW  */
  YYSYMBOL_LT = 24,                        /* LT  */
  YYSYMBOL_GT = 25,                        /* GT  */
  YYSYMBOL_LE = 26,                        /* LE  */
  YYSYMBOL_GE = 27,                        /* GE  */
  YYSYMBOL_EQ = 28,                        /* EQ  */
  YYSYMBOL_NE = 29,                        /* NE  */
  YYSYMBOL_AND = 30,                       /* AND  */
  YYSYMBOL_OR = 31,                        /* OR  */
  YYSYMBOL_NOT = 32,                       /* NOT  */
  YYSYMBOL_CONCAT = 33,                    /* CONCAT  */
  YYSYMBOL_CONCAT_SPACE = 34,              /* CONCAT_SPACE  */
  YYSYMBOL_SIN = 35,                       /* SIN  */
  YYSYMBOL_COS = 36,                       /* COS  */
  YYSYMBOL_MAX = 37,                       /* MAX  */
  YYSYMBOL_MIN = 38,                       /* MIN  */
  YYSYMBOL_SQRT = 39,                      /* SQRT  */
  YYSYMBOL_EXP = 40,                       /* EXP  */
  YYSYMBOL_LOG = 41,                       /* LOG  */
  YYSYMBOL_RANDOM = 42,                    /* RANDOM  */
  YYSYMBOL_PRINT = 43,                     /* PRINT  */
  YYSYMBOL_PI = 44,                        /* PI  */
  YYSYMBOL_E = 45,                         /* E  */
  YYSYMBOL_FUNC = 46,                      /* FUNC  */
  YYSYMBOL_LET = 47,                       /* LET  */
  YYSYMBOL_IN = 48,                        /* IN  */
  YYSYMBOL_IF = 49,                        /* IF  */
  YYSYMBOL_ELIF = 50,                      /* ELIF  */
  YYSYMBOL_ELSE = 51,                      /* ELSE  */
  YYSYMBOL_FOR = 52,                       /* FOR  */
  YYSYMBOL_WHILE = 53,                     /* WHILE  */
  YYSYMBOL_RANGE = 54,                     /* RANGE  */
  YYSYMBOL_TYPE = 55,                      /* TYPE  */
  YYSYMBOL_NEW = 56,                       /* NEW  */
  YYSYMBOL_SELF = 57,                      /* SELF  */
  YYSYMBOL_INHERITS = 58,                  /* INHERITS  */
  YYSYMBOL_BASE = 59,                      /* BASE  */
  YYSYMBOL_COLON = 60,                     /* COLON  */
  YYSYMBOL_YYACCEPT = 61,                  /* $accept  */
  YYSYMBOL_program = 62,                   /* program  */
  YYSYMBOL_statement = 63,                 /* statement  */
  YYSYMBOL_expression = 64,                /* expression  */
  YYSYMBOL_elem_expr = 65,                 /* elem_expr  */
  YYSYMBOL_id = 66,                        /* id  */
  YYSYMBOL_block_expr = 67,                /* block_expr  */
  YYSYMBOL_block_body = 68,                /* block_body  */
  YYSYMBOL_params = 69,                    /* params  */
  YYSYMBOL_func_call_expr = 70,            /* func_call_expr  */
  YYSYMBOL_args = 71,                      /* args  */
  YYSYMBOL_assign_expr = 72,               /* assign_expr  */
  YYSYMBOL_let_expr = 73,                  /* let_expr  */
  YYSYMBOL_decl = 74,                      /* decl  */
  YYSYMBOL_body = 75,                      /* body  */
  YYSYMBOL_if_expr = 76,                   /* if_expr  */
  YYSYMBOL_if_head = 77,                   /* if_head  */
  YYSYMBOL_while_expr = 78,                /* while_expr  */
  YYSYMBOL_for_expr = 79,                  /* for_expr  */
  YYSYMBOL_type_decl = 80,                 /* type_decl  */
  YYSYMBOL_type_body = 81,                 /* type_body  */
  YYSYMBOL_attribute_decl = 82,            /* attribute_decl  */
  YYSYMBOL_method_decl = 83,               /* method_decl  */
  YYSYMBOL_self_call = 84,                 /* self_call  */
  YYSYMBOL_method_call = 85,               /* method_call  */
  YYSYMBOL_base_call = 86,                 /* base_call  */
  YYSYMBOL_new_instance = 87               /* new_instance  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1160

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  61
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  27
/* YYNRULES -- Number of rules.  */
#define YYNRULES  111
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  282

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   307


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      11,    12,     2,     2,     8,     2,    10,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     9,
       2,    16,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    13,     2,    14,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,    15,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   173,   173,   175,   176,   180,   181,   185,   186,   187,
     193,   199,   200,   201,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   225,   229,   233,   237,   241,   246,   251,   256,   260,
     265,   270,   275,   280,   285,   290,   296,   300,   301,   303,
     307,   312,   317,   321,   325,   330,   334,   339,   343,   347,
     354,   358,   365,   366,   367,   371,   372,   379,   388,   392,
     393,   394,   398,   399,   403,   404,   405,   406,   410,   417,
     424,   430,   439,   440,   441,   448,   449,   453,   460,   470,
     474,   478,   481,   484,   487,   490,   493,   499,   502,   505,
     508,   514,   518,   525,   529,   533,   537,   544,   548,   551,
     557,   563
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "NUMBER", "STRING",
  "BOOL", "NULL_VAL", "ID", "','", "';'", "'.'", "'('", "')'", "'{'",
  "'}'", "LAMBDA", "'='", "REASSIGN", "ADD", "SUB", "MUL", "DIV", "MOD",
  "POW", "LT", "GT", "LE", "GE", "EQ", "NE", "AND", "OR", "NOT", "CONCAT",
  "CONCAT_SPACE", "SIN", "COS", "MAX", "MIN", "SQRT", "EXP", "LOG",
  "RANDOM", "PRINT", "PI", "E", "FUNC", "LET", "IN", "IF", "ELIF", "ELSE",
  "FOR", "WHILE", "RANGE", "TYPE", "NEW", "SELF", "INHERITS", "BASE",
  "COLON", "$accept", "program", "statement", "expression", "elem_expr",
  "id", "block_expr", "block_body", "params", "func_call_expr", "args",
  "assign_expr", "let_expr", "decl", "body", "if_expr", "if_head",
  "while_expr", "for_expr", "type_decl", "type_body", "attribute_decl",
  "method_decl", "self_call", "method_call", "base_call", "new_instance", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-208)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-61)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -208,   292,  -208,    48,  -208,  -208,  -208,  -208,     7,   577,
     406,   577,   577,     5,    57,    68,    91,   119,   127,   153,
     157,   161,  -208,  -208,    52,   144,   163,   164,   165,   171,
     172,   105,   170,  -208,   737,  -208,   169,  -208,  -208,  -208,
    -208,  -208,   110,  -208,  -208,  -208,   173,  -208,  -208,  -208,
    -208,   175,   577,   849,  -208,  -208,  -208,  -208,  -208,   349,
     135,     8,   577,   577,   577,   577,   577,   577,   577,   176,
     577,   178,   -10,    -7,   577,   185,   577,     2,   184,   189,
     577,  -208,   577,   577,   577,   577,   577,   577,   577,   577,
     577,   577,   577,   577,   577,   577,   577,   577,   577,   186,
     463,   577,   187,  1126,   104,  -208,  -208,  -208,   869,   889,
     619,   643,   909,   929,   667,  -208,   949,   192,   577,   193,
     194,   520,   969,   154,   989,   192,   196,   198,   577,   197,
     114,   135,   135,  -208,  -208,  -208,    88,    88,    88,    88,
      88,    88,    88,   351,   351,     8,     8,  1126,   577,   199,
    -208,   737,  -208,  1126,   577,   577,  -208,  -208,  -208,   577,
     577,  -208,  -208,   577,   200,  -208,   116,  1126,   191,    -4,
     463,   202,   463,   167,   463,   123,    33,   204,   215,   216,
      -6,   124,   577,  -208,  1009,   577,   129,  1126,  1029,  1049,
    1069,  -208,   217,    27,   577,   577,   218,   714,   220,  -208,
    -208,   219,  -208,   -11,   192,   577,  -208,   103,   216,   222,
     577,   196,  -208,   131,   463,  1089,  -208,  -208,  -208,  -208,
    -208,   463,  -208,  1126,  1126,   212,   226,   577,   196,   229,
     132,   760,   577,   192,   138,   223,  -208,  -208,   200,  -208,
     577,  -208,   691,   224,     9,   108,  -208,   783,   140,   228,
    -208,  1126,   577,  -208,   577,   196,   577,   234,  -208,   152,
     196,  1109,   141,   231,   806,  -208,   577,   237,   233,   236,
     239,  -208,  -208,   829,  -208,  -208,   463,   196,  -208,  -208,
     235,  -208
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     1,     0,    14,    15,    16,    17,    18,     0,
      62,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    48,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     3,     0,    21,     0,     8,    23,    26,
      11,    28,    85,    12,    13,     7,    19,    24,    25,    20,
       4,     0,    69,     0,    22,    27,    29,    30,    63,     0,
      37,    59,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      69,     5,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    70,     0,    38,    61,    64,     0,     0,
       0,     0,     0,     0,     0,    46,     0,    65,     0,     0,
       0,     0,     0,     0,     0,    65,    97,     0,    69,   107,
       0,    31,    32,    33,    34,    35,    36,    51,    52,    53,
      54,    55,    56,    57,    58,    49,    50,    72,     0,     0,
      82,    83,    86,    73,    69,     0,    68,    39,    40,     0,
       0,    43,    45,     0,     0,    66,     0,    78,     0,     0,
       0,    74,     0,     0,     0,     0,     0,     0,    98,    99,
       0,     0,    69,   110,     0,     0,     0,    71,     0,     0,
       0,     6,     0,     0,     0,     0,     0,     0,     0,    76,
      87,     0,    89,     0,    65,     0,    92,     0,   100,     0,
      69,    97,   111,     0,     0,     0,   108,    42,    41,    44,
      67,     0,    10,    79,    80,     0,    75,     0,    97,     0,
       0,     0,     0,    65,     0,     0,   109,    88,    84,     9,
       0,    77,     0,     0,     0,     0,   101,     0,     0,     0,
      96,    81,     0,    91,    69,    97,     0,     0,   102,     0,
      97,     0,     0,     0,     0,   104,     0,     0,     0,     0,
       0,    95,   103,     0,   106,    94,     0,    97,   105,    90,
       0,    93
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -208,  -208,    44,    -1,  -208,  -208,    13,  -208,  -106,  -208,
     -77,  -208,    42,  -208,   -96,  -208,  -208,    45,    70,  -208,
    -207,  -208,    51,  -208,  -208,  -208,  -208
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     1,   150,   103,    35,    36,    54,    59,   166,    38,
     104,    39,    55,    73,   152,    41,    42,    56,    57,    45,
     177,   178,   179,    46,    47,    48,    49
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      34,   120,   228,   130,   235,   210,   118,   211,    53,    34,
      60,    61,   195,   125,    37,   126,    62,    51,    52,   175,
     254,   243,   255,    37,   -60,   171,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      10,   121,   221,    40,   204,    33,    43,   229,   263,   205,
     119,   181,    40,   268,    58,    43,   196,    50,    34,    71,
     127,   108,   109,   110,   111,   112,   113,   114,    63,   116,
     280,    44,    37,   122,   198,   124,   200,   186,   202,    64,
      44,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   230,   151,
     153,    40,    65,   107,    43,   213,    82,    83,    84,    85,
      86,    87,   155,    37,   204,    79,   156,   167,   237,   232,
     151,    10,   155,   256,   192,   239,   183,   248,   193,    44,
      66,   192,   155,   234,    37,   203,   212,   155,    67,   155,
     192,   216,    40,   236,   245,    43,   155,   184,   192,   155,
     249,    72,   259,   270,   187,    84,    85,    86,   188,   189,
      99,   100,   190,    40,    68,    10,    43,   266,    69,   197,
      44,   151,    70,   151,    74,    75,    76,   262,    77,    78,
     279,    80,   102,    37,   215,    37,    98,    37,   115,   117,
     101,    44,   123,   223,   224,   128,   129,   148,   154,   165,
     168,   169,   173,   176,   231,   180,   222,   194,   182,   191,
     185,   199,    40,   151,    40,    43,    40,    43,   206,    43,
     151,   201,   207,   209,   220,   225,   242,    37,   240,   208,
     227,   247,   226,   233,    37,   241,   244,   250,   253,   251,
      44,   260,    44,   265,    44,   271,   274,   275,   276,   281,
       0,   261,   277,     0,     0,   264,    40,     0,   257,    43,
       0,     0,     0,    40,     0,   273,    43,     0,     0,     0,
       0,     0,   267,     0,     0,   151,     0,     0,     0,     0,
       0,     0,     0,     0,    44,     0,     0,     0,     0,    37,
       0,    44,     2,     3,     0,     4,     5,     6,     7,     8,
       0,     0,     0,     9,     0,    10,     0,     0,     0,     0,
       0,    11,     0,     0,     0,     0,     0,     0,    40,     0,
       0,    43,     0,     0,    12,     0,     0,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
       0,    26,     0,     0,    27,    28,    44,    29,    30,    31,
       0,    32,     4,     5,     6,     7,     8,     0,     0,     0,
       9,     0,    10,   106,     0,     0,     0,     0,    11,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    12,     0,     0,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,     0,    26,     0,
       0,    27,    28,     0,    29,    30,    31,     0,    32,     4,
       5,     6,     7,     8,     0,     0,     0,     9,     0,    10,
       0,     0,     0,     0,     0,    11,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    12,     0,
       0,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,     0,    26,     0,     0,    27,    28,
       0,    29,    30,    31,     0,    32,     4,     5,     6,     7,
       8,     0,     0,     0,     9,     0,    10,     0,     0,     0,
       0,     0,    11,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    12,     0,     0,    13,    14,
      15,    16,    17,    18,    19,    20,   149,    22,    23,    24,
      25,     0,    26,     0,     0,    27,    28,     0,    29,    30,
      31,     0,    32,     4,     5,     6,     7,     8,     0,     0,
       0,   170,     0,    10,     0,     0,     0,     0,     0,    11,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    12,     0,     0,    13,    14,    15,    16,    17,
      18,    19,    20,   149,    22,    23,    24,    25,     0,    26,
       0,     0,    27,    28,     0,    29,    30,    31,     0,    32,
       4,     5,     6,     7,     8,     0,     0,     0,     9,     0,
      10,     0,     0,     0,     0,     0,    11,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    12,
       0,     0,    13,    14,    15,    16,    17,    18,    19,    20,
       0,    22,    23,     0,    25,     0,    26,   159,     0,    27,
      28,     0,     0,    30,    31,     0,    32,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,   160,    96,    97,     0,     0,     0,     0,     0,     0,
       0,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,   163,    96,    97,     0,     0,
       0,     0,     0,     0,     0,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,   252,
      96,    97,     0,     0,     0,     0,     0,     0,     0,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    81,    96,    97,   105,     0,     0,     0,
       0,     0,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    81,    96,    97,     0,
       0,     0,     0,     0,     0,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,   246,
      96,    97,     0,     0,     0,     0,     0,     0,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,   258,    96,    97,     0,     0,     0,     0,     0,
       0,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,   272,    96,    97,     0,     0,
       0,     0,     0,     0,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,   278,    96,
      97,     0,     0,     0,     0,     0,     0,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,   105,    96,    97,     0,     0,     0,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,   157,    96,    97,     0,     0,     0,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,   158,    96,    97,     0,     0,     0,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,   161,    96,    97,     0,     0,     0,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,   162,    96,    97,     0,     0,     0,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,   164,    96,    97,     0,     0,     0,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,   172,    96,    97,     0,     0,     0,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,   174,    96,    97,     0,     0,     0,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,   214,    96,    97,     0,     0,     0,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,   217,    96,    97,     0,     0,     0,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,   218,    96,    97,     0,     0,     0,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,   219,    96,    97,     0,     0,     0,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,   238,    96,    97,     0,     0,     0,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,   269,    96,    97,     0,     0,     0,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,     0,    96,    97,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,     0,    96,
      97
};

static const yytype_int16 yycheck[] =
{
       1,     8,    13,    80,   211,    11,    16,    13,     9,    10,
      11,    12,    16,    11,     1,    13,    11,    10,    11,   125,
      11,   228,    13,    10,    17,   121,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      13,    48,    15,     1,    11,     1,     1,    58,   255,    16,
      60,   128,    10,   260,    10,    10,    60,     9,    59,     7,
      58,    62,    63,    64,    65,    66,    67,    68,    11,    70,
     277,     1,    59,    74,   170,    76,   172,   154,   174,    11,
      10,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,   204,   100,
     101,    59,    11,    59,    59,   182,    18,    19,    20,    21,
      22,    23,     8,   100,    11,    10,    12,   118,   214,    16,
     121,    13,     8,    15,     8,   221,    12,   233,    12,    59,
      11,     8,     8,   210,   121,    12,    12,     8,    11,     8,
       8,    12,   100,    12,    12,   100,     8,   148,     8,     8,
      12,     7,    12,    12,   155,    20,    21,    22,   159,   160,
      50,    51,   163,   121,    11,    13,   121,    15,    11,   170,
     100,   172,    11,   174,    11,    11,    11,   254,     7,     7,
     276,    11,     7,   170,   185,   172,    17,   174,    12,    11,
      17,   121,     7,   194,   195,    11,     7,    11,    11,     7,
       7,     7,    48,     7,   205,     7,   193,    16,    11,     9,
      11,     9,   170,   214,   172,   170,   174,   172,    14,   174,
     221,    54,     7,     7,     7,     7,   227,   214,    16,   178,
      11,   232,    12,    11,   221,     9,     7,    14,    14,   240,
     170,    13,   172,     9,   174,    14,     9,    14,    12,    14,
      -1,   252,    13,    -1,    -1,   256,   214,    -1,   245,   214,
      -1,    -1,    -1,   221,    -1,   266,   221,    -1,    -1,    -1,
      -1,    -1,   259,    -1,    -1,   276,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   214,    -1,    -1,    -1,    -1,   276,
      -1,   221,     0,     1,    -1,     3,     4,     5,     6,     7,
      -1,    -1,    -1,    11,    -1,    13,    -1,    -1,    -1,    -1,
      -1,    19,    -1,    -1,    -1,    -1,    -1,    -1,   276,    -1,
      -1,   276,    -1,    -1,    32,    -1,    -1,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      -1,    49,    -1,    -1,    52,    53,   276,    55,    56,    57,
      -1,    59,     3,     4,     5,     6,     7,    -1,    -1,    -1,
      11,    -1,    13,    14,    -1,    -1,    -1,    -1,    19,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    32,    -1,    -1,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    -1,    49,    -1,
      -1,    52,    53,    -1,    55,    56,    57,    -1,    59,     3,
       4,     5,     6,     7,    -1,    -1,    -1,    11,    -1,    13,
      -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,
      -1,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    -1,    49,    -1,    -1,    52,    53,
      -1,    55,    56,    57,    -1,    59,     3,     4,     5,     6,
       7,    -1,    -1,    -1,    11,    -1,    13,    -1,    -1,    -1,
      -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    32,    -1,    -1,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    -1,    49,    -1,    -1,    52,    53,    -1,    55,    56,
      57,    -1,    59,     3,     4,     5,     6,     7,    -1,    -1,
      -1,    11,    -1,    13,    -1,    -1,    -1,    -1,    -1,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    32,    -1,    -1,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    -1,    49,
      -1,    -1,    52,    53,    -1,    55,    56,    57,    -1,    59,
       3,     4,     5,     6,     7,    -1,    -1,    -1,    11,    -1,
      13,    -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,
      -1,    -1,    35,    36,    37,    38,    39,    40,    41,    42,
      -1,    44,    45,    -1,    47,    -1,    49,     8,    -1,    52,
      53,    -1,    -1,    56,    57,    -1,    59,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,     8,    33,    34,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,     8,    33,    34,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,     8,
      33,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,     9,    33,    34,    12,    -1,    -1,    -1,
      -1,    -1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,     9,    33,    34,    -1,
      -1,    -1,    -1,    -1,    -1,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,     9,
      33,    34,    -1,    -1,    -1,    -1,    -1,    -1,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,     9,    33,    34,    -1,    -1,    -1,    -1,    -1,
      -1,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,     9,    33,    34,    -1,    -1,
      -1,    -1,    -1,    -1,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,     9,    33,
      34,    -1,    -1,    -1,    -1,    -1,    -1,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    12,    33,    34,    -1,    -1,    -1,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    12,    33,    34,    -1,    -1,    -1,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    12,    33,    34,    -1,    -1,    -1,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    12,    33,    34,    -1,    -1,    -1,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    12,    33,    34,    -1,    -1,    -1,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    12,    33,    34,    -1,    -1,    -1,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    12,    33,    34,    -1,    -1,    -1,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    12,    33,    34,    -1,    -1,    -1,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    12,    33,    34,    -1,    -1,    -1,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    12,    33,    34,    -1,    -1,    -1,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    12,    33,    34,    -1,    -1,    -1,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    12,    33,    34,    -1,    -1,    -1,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    12,    33,    34,    -1,    -1,    -1,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    12,    33,    34,    -1,    -1,    -1,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    -1,    33,    34,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    -1,    33,
      34
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    62,     0,     1,     3,     4,     5,     6,     7,    11,
      13,    19,    32,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    49,    52,    53,    55,
      56,    57,    59,    63,    64,    65,    66,    67,    70,    72,
      73,    76,    77,    78,    79,    80,    84,    85,    86,    87,
       9,    10,    11,    64,    67,    73,    78,    79,    63,    68,
      64,    64,    11,    11,    11,    11,    11,    11,    11,    11,
      11,     7,     7,    74,    11,    11,    11,     7,     7,    10,
      11,     9,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    33,    34,    17,    50,
      51,    17,     7,    64,    71,    12,    14,    63,    64,    64,
      64,    64,    64,    64,    64,    12,    64,    11,    16,    60,
       8,    48,    64,     7,    64,    11,    13,    58,    11,     7,
      71,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    11,    43,
      63,    64,    75,    64,    11,     8,    12,    12,    12,     8,
       8,    12,    12,     8,    12,     7,    69,    64,     7,     7,
      11,    75,    12,    48,    12,    69,     7,    81,    82,    83,
       7,    71,    11,    12,    64,    11,    71,    64,    64,    64,
      64,     9,     8,    12,    16,    16,    60,    64,    75,     9,
      75,    54,    75,    12,    11,    16,    14,     7,    83,     7,
      11,    13,    12,    71,    12,    64,    12,    12,    12,    12,
       7,    15,    67,    64,    64,     7,    12,    11,    13,    58,
      69,    64,    16,    11,    71,    81,    12,    75,    12,    75,
      16,     9,    64,    81,     7,    12,     9,    64,    69,    12,
      14,    64,     8,    14,    11,    13,    15,    67,     9,    12,
      13,    64,    71,    81,    64,     9,    15,    67,    81,    12,
      12,    14,     9,    64,     9,    14,    12,    13,     9,    75,
      81,    14
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    61,    62,    62,    62,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      66,    67,    68,    68,    68,    69,    69,    69,    70,    71,
      71,    71,    72,    72,    73,    73,    73,    73,    74,    74,
      74,    74,    75,    75,    75,    76,    76,    77,    77,    78,
      79,    80,    80,    80,    80,    80,    80,    81,    81,    81,
      81,    82,    82,    83,    83,    83,    83,    84,    85,    85,
      86,    87
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     3,     2,     5,     1,     1,     7,
       6,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     3,     2,     3,     4,
       4,     6,     6,     4,     6,     4,     3,     1,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       1,     3,     0,     1,     2,     0,     1,     3,     4,     0,
       1,     3,     3,     3,     4,     6,     5,     7,     3,     5,
       5,     7,     1,     1,     4,     1,     3,     5,     6,     5,
      12,     8,     5,    13,    10,    10,     7,     0,     1,     1,
       2,     4,     5,     7,     6,     8,     7,     3,     6,     6,
       4,     5
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 3: /* program: program statement  */
#line 175 "src/parser/parser.y"
                                    { root.push_back((yyvsp[0].node)); }
#line 1649 ".build/parser.tab.cpp"
    break;

  case 4: /* program: program error ';'  */
#line 176 "src/parser/parser.y"
                                    { yyerrok; }
#line 1655 ".build/parser.tab.cpp"
    break;

  case 5: /* statement: expression ';'  */
#line 180 "src/parser/parser.y"
                                    { (yyval.node) = (yyvsp[-1].node); }
#line 1661 ".build/parser.tab.cpp"
    break;

  case 6: /* statement: PRINT '(' expression ')' ';'  */
#line 181 "src/parser/parser.y"
                                    { 
                                        std::vector<ASTNode*> args = vectorize((yyvsp[-2].node), nullptr, 1);
                                        (yyval.node) = new BuiltInFunctionNode("print", args, yylloc.first_line);
                                    }
#line 1670 ".build/parser.tab.cpp"
    break;

  case 7: /* statement: type_decl  */
#line 185 "src/parser/parser.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 1676 ".build/parser.tab.cpp"
    break;

  case 8: /* statement: block_expr  */
#line 186 "src/parser/parser.y"
                                    { (yyval.node) = (yyvsp[0].node); }
#line 1682 ".build/parser.tab.cpp"
    break;

  case 9: /* statement: FUNC ID '(' params ')' LAMBDA body  */
#line 188 "src/parser/parser.y"
                                    {
                                        (yyval.node) = new FunctionDeclarationNode(*(yyvsp[-5].str), (yyvsp[-3].param), (yyvsp[0].node), true, yylloc.first_line);
                                        std::cout << "Definición función inline: " << *(yyvsp[-5].str) << std::endl;
                                        
                                    }
#line 1692 ".build/parser.tab.cpp"
    break;

  case 10: /* statement: FUNC ID '(' params ')' block_expr  */
#line 194 "src/parser/parser.y"
                                    {
                                        (yyval.node) = new FunctionDeclarationNode(*(yyvsp[-4].str), (yyvsp[-2].param), (yyvsp[0].node), false, yylloc.first_line);
                                        std::cout << "Definición función bloque: " << *(yyvsp[-4].str) << std::endl;
                                        
                                    }
#line 1702 ".build/parser.tab.cpp"
    break;

  case 11: /* statement: let_expr  */
#line 199 "src/parser/parser.y"
                                    { (yyval.node) = (yyvsp[0].node); std::cout << "let_expr " << std::endl; }
#line 1708 ".build/parser.tab.cpp"
    break;

  case 12: /* statement: while_expr  */
#line 200 "src/parser/parser.y"
                                    { (yyval.node) = (yyvsp[0].node); std::cout << "while_expr " << std::endl; }
#line 1714 ".build/parser.tab.cpp"
    break;

  case 13: /* statement: for_expr  */
#line 201 "src/parser/parser.y"
                                    { (yyval.node) = (yyvsp[0].node); std::cout << "for_expr " << std::endl; }
#line 1720 ".build/parser.tab.cpp"
    break;

  case 14: /* expression: NUMBER  */
#line 205 "src/parser/parser.y"
                                { (yyval.node) = new LiteralNode(std::to_string((yyvsp[0].num)), "Number", yylloc.first_line); }
#line 1726 ".build/parser.tab.cpp"
    break;

  case 15: /* expression: STRING  */
#line 206 "src/parser/parser.y"
                                { (yyval.node) = new LiteralNode(*(yyvsp[0].str), "String", yylloc.first_line);  }
#line 1732 ".build/parser.tab.cpp"
    break;

  case 16: /* expression: BOOL  */
#line 207 "src/parser/parser.y"
                                { (yyval.node) = new LiteralNode((yyvsp[0].boolean) ? "true" : "false", "Boolean", yylloc.first_line); }
#line 1738 ".build/parser.tab.cpp"
    break;

  case 17: /* expression: NULL_VAL  */
#line 208 "src/parser/parser.y"
                                { (yyval.node) = new LiteralNode("null", "Null", yylloc.first_line); }
#line 1744 ".build/parser.tab.cpp"
    break;

  case 18: /* expression: ID  */
#line 209 "src/parser/parser.y"
                                { (yyval.node) = new IdentifierNode(*(yyvsp[0].str), yylloc.first_line); }
#line 1750 ".build/parser.tab.cpp"
    break;

  case 19: /* expression: self_call  */
#line 210 "src/parser/parser.y"
                                { (yyval.node) = (yyvsp[0].node); }
#line 1756 ".build/parser.tab.cpp"
    break;

  case 20: /* expression: new_instance  */
#line 211 "src/parser/parser.y"
                                { (yyval.node) = (yyvsp[0].node); }
#line 1762 ".build/parser.tab.cpp"
    break;

  case 21: /* expression: elem_expr  */
#line 212 "src/parser/parser.y"
                                { (yyval.node) = (yyvsp[0].node); }
#line 1768 ".build/parser.tab.cpp"
    break;

  case 22: /* expression: block_expr  */
#line 213 "src/parser/parser.y"
                                { (yyval.node) = (yyvsp[0].node); }
#line 1774 ".build/parser.tab.cpp"
    break;

  case 23: /* expression: func_call_expr  */
#line 214 "src/parser/parser.y"
                                { (yyval.node) = (yyvsp[0].node); }
#line 1780 ".build/parser.tab.cpp"
    break;

  case 24: /* expression: method_call  */
#line 215 "src/parser/parser.y"
                                { (yyval.node) = (yyvsp[0].node); }
#line 1786 ".build/parser.tab.cpp"
    break;

  case 25: /* expression: base_call  */
#line 216 "src/parser/parser.y"
                                { (yyval.node) = (yyvsp[0].node); }
#line 1792 ".build/parser.tab.cpp"
    break;

  case 26: /* expression: assign_expr  */
#line 217 "src/parser/parser.y"
                                { (yyval.node) = (yyvsp[0].node); }
#line 1798 ".build/parser.tab.cpp"
    break;

  case 27: /* expression: let_expr  */
#line 218 "src/parser/parser.y"
                                { (yyval.node) = (yyvsp[0].node); std::cout << "let_expr " << std::endl; }
#line 1804 ".build/parser.tab.cpp"
    break;

  case 28: /* expression: if_expr  */
#line 219 "src/parser/parser.y"
                                { (yyval.node) = (yyvsp[0].node); std::cout << "if_expr " << std::endl; }
#line 1810 ".build/parser.tab.cpp"
    break;

  case 29: /* expression: while_expr  */
#line 220 "src/parser/parser.y"
                                { (yyval.node) = (yyvsp[0].node); std::cout << "while_expr " << std::endl; }
#line 1816 ".build/parser.tab.cpp"
    break;

  case 30: /* expression: for_expr  */
#line 221 "src/parser/parser.y"
                                { (yyval.node) = (yyvsp[0].node); std::cout << "for_expr " << std::endl; }
#line 1822 ".build/parser.tab.cpp"
    break;

  case 31: /* elem_expr: expression ADD expression  */
#line 225 "src/parser/parser.y"
                                        {
                (yyval.node) = new BinaryOpNode("+", (yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);
                
            }
#line 1831 ".build/parser.tab.cpp"
    break;

  case 32: /* elem_expr: expression SUB expression  */
#line 229 "src/parser/parser.y"
                                        {
                (yyval.node) = new BinaryOpNode("-", (yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);
                
            }
#line 1840 ".build/parser.tab.cpp"
    break;

  case 33: /* elem_expr: expression MUL expression  */
#line 233 "src/parser/parser.y"
                                        {
                (yyval.node) = new BinaryOpNode("*", (yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);
                
            }
#line 1849 ".build/parser.tab.cpp"
    break;

  case 34: /* elem_expr: expression DIV expression  */
#line 237 "src/parser/parser.y"
                                        {
                (yyval.node) = new BinaryOpNode("/", (yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);
                
            }
#line 1858 ".build/parser.tab.cpp"
    break;

  case 35: /* elem_expr: expression MOD expression  */
#line 241 "src/parser/parser.y"
                                        {
                
                (yyval.node) = new BinaryOpNode("%", (yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);
                
            }
#line 1868 ".build/parser.tab.cpp"
    break;

  case 36: /* elem_expr: expression POW expression  */
#line 246 "src/parser/parser.y"
                                        {
                (yyval.node) = new BinaryOpNode("^", (yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);
                
            }
#line 1877 ".build/parser.tab.cpp"
    break;

  case 37: /* elem_expr: SUB expression  */
#line 251 "src/parser/parser.y"
                             {
                (yyval.node) = new UnaryOpNode("-", (yyvsp[0].node), yylloc.first_line);
                
            }
#line 1886 ".build/parser.tab.cpp"
    break;

  case 38: /* elem_expr: '(' expression ')'  */
#line 256 "src/parser/parser.y"
                                 {
                (yyval.node) = (yyvsp[-1].node);
            }
#line 1894 ".build/parser.tab.cpp"
    break;

  case 39: /* elem_expr: SIN '(' expression ')'  */
#line 260 "src/parser/parser.y"
                                     {
                std::vector<ASTNode*> args = vectorize((yyvsp[-1].node), nullptr, 1);
                (yyval.node) = new BuiltInFunctionNode("sin", args, yylloc.first_line);
               
            }
#line 1904 ".build/parser.tab.cpp"
    break;

  case 40: /* elem_expr: COS '(' expression ')'  */
#line 265 "src/parser/parser.y"
                                     {
                std::vector<ASTNode*> args = vectorize((yyvsp[-1].node), nullptr, 1);
                (yyval.node) = new BuiltInFunctionNode("cos", args, yylloc.first_line);
                
            }
#line 1914 ".build/parser.tab.cpp"
    break;

  case 41: /* elem_expr: MIN '(' expression ',' expression ')'  */
#line 270 "src/parser/parser.y"
                                                    {
                std::vector<ASTNode*> args = vectorize((yyvsp[-3].node), (yyvsp[-1].node), 2);
                (yyval.node) = new BuiltInFunctionNode("min", args, yylloc.first_line);
                
            }
#line 1924 ".build/parser.tab.cpp"
    break;

  case 42: /* elem_expr: MAX '(' expression ',' expression ')'  */
#line 275 "src/parser/parser.y"
                                                    {
                std::vector<ASTNode*> args = vectorize((yyvsp[-3].node), (yyvsp[-1].node), 2);
                (yyval.node) = new BuiltInFunctionNode("max", args, yylloc.first_line);
                
            }
#line 1934 ".build/parser.tab.cpp"
    break;

  case 43: /* elem_expr: SQRT '(' expression ')'  */
#line 280 "src/parser/parser.y"
                                      {
                std::vector<ASTNode*> args = vectorize((yyvsp[-1].node), nullptr, 1);
                (yyval.node) = new BuiltInFunctionNode("sqrt", args, yylloc.first_line);
                
            }
#line 1944 ".build/parser.tab.cpp"
    break;

  case 44: /* elem_expr: LOG '(' expression ',' expression ')'  */
#line 285 "src/parser/parser.y"
                                                    {
                std::vector<ASTNode*> args = vectorize((yyvsp[-3].node), (yyvsp[-1].node), 2);
                (yyval.node) = new BuiltInFunctionNode("log", args, yylloc.first_line);
                
            }
#line 1954 ".build/parser.tab.cpp"
    break;

  case 45: /* elem_expr: EXP '(' expression ')'  */
#line 290 "src/parser/parser.y"
                                     {
                std::vector<ASTNode*> args = vectorize((yyvsp[-1].node), nullptr, 1);
                (yyval.node) = new BuiltInFunctionNode("exp", args, yylloc.first_line);
                
            }
#line 1964 ".build/parser.tab.cpp"
    break;

  case 46: /* elem_expr: RANDOM '(' ')'  */
#line 296 "src/parser/parser.y"
                             {
                std::vector<ASTNode*> args = vectorize(nullptr, nullptr, 0);
                (yyval.node) = new BuiltInFunctionNode("rand", args, yylloc.first_line);
            }
#line 1973 ".build/parser.tab.cpp"
    break;

  case 47: /* elem_expr: E  */
#line 300 "src/parser/parser.y"
                    { (yyval.node) = new IdentifierNode("e", yylloc.first_line); }
#line 1979 ".build/parser.tab.cpp"
    break;

  case 48: /* elem_expr: PI  */
#line 301 "src/parser/parser.y"
                    { (yyval.node) = new IdentifierNode("pi", yylloc.first_line); }
#line 1985 ".build/parser.tab.cpp"
    break;

  case 49: /* elem_expr: expression CONCAT expression  */
#line 303 "src/parser/parser.y"
                                           {
                (yyval.node) = new BinaryOpNode("@", (yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);
               
            }
#line 1994 ".build/parser.tab.cpp"
    break;

  case 50: /* elem_expr: expression CONCAT_SPACE expression  */
#line 307 "src/parser/parser.y"
                                                 {
                (yyval.node) = new BinaryOpNode("@@", (yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);
               
            }
#line 2003 ".build/parser.tab.cpp"
    break;

  case 51: /* elem_expr: expression LT expression  */
#line 312 "src/parser/parser.y"
                                       {
                
                (yyval.node) = new BinaryOpNode("<", (yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);
                
            }
#line 2013 ".build/parser.tab.cpp"
    break;

  case 52: /* elem_expr: expression GT expression  */
#line 317 "src/parser/parser.y"
                                       {
                (yyval.node) = new BinaryOpNode(">", (yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);
                
            }
#line 2022 ".build/parser.tab.cpp"
    break;

  case 53: /* elem_expr: expression LE expression  */
#line 321 "src/parser/parser.y"
                                       {
                (yyval.node) = new BinaryOpNode("<=", (yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);
               
            }
#line 2031 ".build/parser.tab.cpp"
    break;

  case 54: /* elem_expr: expression GE expression  */
#line 325 "src/parser/parser.y"
                                       {
                (yyval.node) = new BinaryOpNode(">=", (yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);
                
            }
#line 2040 ".build/parser.tab.cpp"
    break;

  case 55: /* elem_expr: expression EQ expression  */
#line 330 "src/parser/parser.y"
                                       {
                (yyval.node) = new BinaryOpNode("==", (yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);
               
            }
#line 2049 ".build/parser.tab.cpp"
    break;

  case 56: /* elem_expr: expression NE expression  */
#line 334 "src/parser/parser.y"
                                       {
                (yyval.node) = new BinaryOpNode("!=", (yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);
               
            }
#line 2058 ".build/parser.tab.cpp"
    break;

  case 57: /* elem_expr: expression AND expression  */
#line 339 "src/parser/parser.y"
                                        {
                (yyval.node) = new BinaryOpNode("&", (yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);
                
            }
#line 2067 ".build/parser.tab.cpp"
    break;

  case 58: /* elem_expr: expression OR expression  */
#line 343 "src/parser/parser.y"
                                       {
                (yyval.node) = new BinaryOpNode("|", (yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);
                
            }
#line 2076 ".build/parser.tab.cpp"
    break;

  case 59: /* elem_expr: NOT expression  */
#line 347 "src/parser/parser.y"
                             {
                (yyval.node) = new UnaryOpNode("!", (yyvsp[0].node), yylloc.first_line);
               
            }
#line 2085 ".build/parser.tab.cpp"
    break;

  case 60: /* id: ID  */
#line 354 "src/parser/parser.y"
                                  { (yyval.node) = new IdentifierNode(*(yyvsp[0].str), yylloc.first_line); }
#line 2091 ".build/parser.tab.cpp"
    break;

  case 61: /* block_expr: '{' block_body '}'  */
#line 358 "src/parser/parser.y"
                                {
                                    (yyval.node) = new BlockNode(*(yyvsp[-1].list), yylloc.first_line); // Placeholder
                                   
                                }
#line 2100 ".build/parser.tab.cpp"
    break;

  case 62: /* block_body: %empty  */
#line 365 "src/parser/parser.y"
                                            { (yyval.list) = new std::vector<ASTNode*>(); }
#line 2106 ".build/parser.tab.cpp"
    break;

  case 63: /* block_body: statement  */
#line 366 "src/parser/parser.y"
                                            { (yyval.list) = new std::vector<ASTNode*>(); (yyval.list)->push_back((yyvsp[0].node)); }
#line 2112 ".build/parser.tab.cpp"
    break;

  case 64: /* block_body: block_body statement  */
#line 367 "src/parser/parser.y"
                                            { (yyvsp[-1].list)->push_back((yyvsp[0].node)); (yyval.list) = (yyvsp[-1].list); }
#line 2118 ".build/parser.tab.cpp"
    break;

  case 65: /* params: %empty  */
#line 371 "src/parser/parser.y"
                                { (yyval.param) = new std::vector<Parameter>(); }
#line 2124 ".build/parser.tab.cpp"
    break;

  case 66: /* params: ID  */
#line 372 "src/parser/parser.y"
                                { 
                                    Parameter p;
                                    p.name = *(yyvsp[0].str);
                                    (yyval.param) = new std::vector<Parameter>(); 
                                    (yyval.param)->push_back(p); 
                                   
                                }
#line 2136 ".build/parser.tab.cpp"
    break;

  case 67: /* params: params ',' ID  */
#line 379 "src/parser/parser.y"
                                { 
                                    Parameter p;
                                    p.name = *(yyvsp[0].str);
                                    (yyvsp[-2].param)->push_back(p); 
                                    (yyval.param) = (yyvsp[-2].param); 
                                }
#line 2147 ".build/parser.tab.cpp"
    break;

  case 68: /* func_call_expr: ID '(' args ')'  */
#line 388 "src/parser/parser.y"
                                { (yyval.node) = new FunctionCallNode(*(yyvsp[-3].str), *(yyvsp[-1].list), yylloc.first_line); }
#line 2153 ".build/parser.tab.cpp"
    break;

  case 69: /* args: %empty  */
#line 392 "src/parser/parser.y"
                                        { (yyval.list) = new std::vector<ASTNode*>(); }
#line 2159 ".build/parser.tab.cpp"
    break;

  case 70: /* args: expression  */
#line 393 "src/parser/parser.y"
                                        { (yyval.list) = new std::vector<ASTNode*>(); (yyval.list)->push_back((yyvsp[0].node)); }
#line 2165 ".build/parser.tab.cpp"
    break;

  case 71: /* args: args ',' expression  */
#line 394 "src/parser/parser.y"
                                        { (yyvsp[-2].list)->push_back((yyvsp[0].node)); (yyval.list) = (yyvsp[-2].list); }
#line 2171 ".build/parser.tab.cpp"
    break;

  case 72: /* assign_expr: id REASSIGN expression  */
#line 398 "src/parser/parser.y"
                                                { (yyval.node) = new AssignmentNode((yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);  }
#line 2177 ".build/parser.tab.cpp"
    break;

  case 73: /* assign_expr: self_call REASSIGN expression  */
#line 399 "src/parser/parser.y"
                                                { (yyval.node) = new AssignmentNode((yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line);  }
#line 2183 ".build/parser.tab.cpp"
    break;

  case 74: /* let_expr: LET decl IN body  */
#line 403 "src/parser/parser.y"
                                              { (yyval.node) = new LetNode((yyvsp[-2].let_decl), (yyvsp[0].node), yylloc.first_line);  }
#line 2189 ".build/parser.tab.cpp"
    break;

  case 75: /* let_expr: LET decl IN '(' body ')'  */
#line 404 "src/parser/parser.y"
                                              { (yyval.node) = new LetNode((yyvsp[-4].let_decl), (yyvsp[-1].node), yylloc.first_line);  }
#line 2195 ".build/parser.tab.cpp"
    break;

  case 76: /* let_expr: LET decl IN body ';'  */
#line 405 "src/parser/parser.y"
                                              { (yyval.node) = new LetNode((yyvsp[-3].let_decl), (yyvsp[-1].node), yylloc.first_line);  }
#line 2201 ".build/parser.tab.cpp"
    break;

  case 77: /* let_expr: LET decl IN '(' body ')' ';'  */
#line 406 "src/parser/parser.y"
                                              { (yyval.node) = new LetNode((yyvsp[-5].let_decl), (yyvsp[-2].node), yylloc.first_line);  }
#line 2207 ".build/parser.tab.cpp"
    break;

  case 78: /* decl: ID '=' expression  */
#line 410 "src/parser/parser.y"
                              {
                LetDeclaration d;
                d.name = *(yyvsp[-2].str);
                d.initializer = (yyvsp[0].node);
                (yyval.let_decl) = new std::vector<LetDeclaration>();
                (yyval.let_decl)->push_back(d);
            }
#line 2219 ".build/parser.tab.cpp"
    break;

  case 79: /* decl: ID COLON ID '=' expression  */
#line 417 "src/parser/parser.y"
                                        {
                LetDeclaration d;
                d.name = *(yyvsp[-4].str);
                d.initializer = (yyvsp[0].node);
                (yyval.let_decl) = new std::vector<LetDeclaration>();
                (yyval.let_decl)->push_back(d);
            }
#line 2231 ".build/parser.tab.cpp"
    break;

  case 80: /* decl: decl ',' ID '=' expression  */
#line 424 "src/parser/parser.y"
                                         {
                LetDeclaration d;
                d.name = *(yyvsp[-2].str);
                d.initializer = (yyvsp[0].node);
                (yyvsp[-4].let_decl)->push_back(d); (yyval.let_decl) = (yyvsp[-4].let_decl);
            }
#line 2242 ".build/parser.tab.cpp"
    break;

  case 81: /* decl: decl ',' ID COLON ID '=' expression  */
#line 430 "src/parser/parser.y"
                                                  {
                LetDeclaration d;
                d.name = *(yyvsp[-4].str);
                d.initializer = (yyvsp[0].node);
                (yyvsp[-6].let_decl)->push_back(d); (yyval.let_decl) = (yyvsp[-6].let_decl);
            }
#line 2253 ".build/parser.tab.cpp"
    break;

  case 82: /* body: statement  */
#line 439 "src/parser/parser.y"
                                            { (yyval.node) = (yyvsp[0].node); }
#line 2259 ".build/parser.tab.cpp"
    break;

  case 83: /* body: expression  */
#line 440 "src/parser/parser.y"
                                            { (yyval.node) = (yyvsp[0].node); }
#line 2265 ".build/parser.tab.cpp"
    break;

  case 84: /* body: PRINT '(' expression ')'  */
#line 441 "src/parser/parser.y"
                                            { 
                                                std::vector<ASTNode*> args = vectorize((yyvsp[-1].node), nullptr, 1);
                                                (yyval.node) = new BuiltInFunctionNode("print", args, yylloc.first_line);
                                            }
#line 2274 ".build/parser.tab.cpp"
    break;

  case 85: /* if_expr: if_head  */
#line 448 "src/parser/parser.y"
                                        { (yyval.node) = new IfNode((yyvsp[0].if_branch), nullptr, yylloc.first_line); }
#line 2280 ".build/parser.tab.cpp"
    break;

  case 86: /* if_expr: if_head ELSE body  */
#line 449 "src/parser/parser.y"
                                        { (yyval.node) = new IfNode((yyvsp[-2].if_branch), (yyvsp[0].node), yylloc.first_line); }
#line 2286 ".build/parser.tab.cpp"
    break;

  case 87: /* if_head: IF '(' expression ')' body  */
#line 453 "src/parser/parser.y"
                                                            { 
                                                                IfBranch b;
                                                                b.condition = (yyvsp[-2].node);
                                                                b.body = (yyvsp[0].node);
                                                                (yyval.if_branch) = new std::vector<IfBranch>(); 
                                                                (yyval.if_branch)->push_back(b);
                                                            }
#line 2298 ".build/parser.tab.cpp"
    break;

  case 88: /* if_head: if_head ELIF '(' expression ')' body  */
#line 460 "src/parser/parser.y"
                                                            { 
                                                                IfBranch b;
                                                                b.condition = (yyvsp[-2].node);
                                                                b.body = (yyvsp[0].node);
                                                                (yyvsp[-5].if_branch)->push_back(b); 
                                                                (yyval.if_branch) = (yyvsp[-5].if_branch);  
                                                            }
#line 2310 ".build/parser.tab.cpp"
    break;

  case 89: /* while_expr: WHILE '(' expression ')' body  */
#line 470 "src/parser/parser.y"
                                                            { (yyval.node) = new WhileNode((yyvsp[-2].node), (yyvsp[0].node), yylloc.first_line); }
#line 2316 ".build/parser.tab.cpp"
    break;

  case 90: /* for_expr: FOR '(' ID IN RANGE '(' expression ',' expression ')' ')' body  */
#line 474 "src/parser/parser.y"
                                                                                { (yyval.node) = new ForNode(*(yyvsp[-9].str), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[0].node), yylloc.first_line); }
#line 2322 ".build/parser.tab.cpp"
    break;

  case 91: /* type_decl: TYPE ID '(' params ')' '{' type_body '}'  */
#line 478 "src/parser/parser.y"
                                                     {
                (yyval.node) = new TypeDeclarationNode(*(yyvsp[-6].str), (yyvsp[-4].param), (yyvsp[-1].type_body), std::nullopt, std::vector<ASTNode*>(), yylloc.first_line);
            }
#line 2330 ".build/parser.tab.cpp"
    break;

  case 92: /* type_decl: TYPE ID '{' type_body '}'  */
#line 481 "src/parser/parser.y"
                                        {
                (yyval.node) = new TypeDeclarationNode(*(yyvsp[-3].str), new std::vector<Parameter>(), (yyvsp[-1].type_body), std::nullopt, std::vector<ASTNode*>(), yylloc.first_line);
            }
#line 2338 ".build/parser.tab.cpp"
    break;

  case 93: /* type_decl: TYPE ID '(' params ')' INHERITS ID '(' args ')' '{' type_body '}'  */
#line 484 "src/parser/parser.y"
                                                                                {
                (yyval.node) = new TypeDeclarationNode(*(yyvsp[-11].str), (yyvsp[-9].param), (yyvsp[-1].type_body), *(yyvsp[-6].str), *(yyvsp[-4].list), yylloc.first_line);
            }
#line 2346 ".build/parser.tab.cpp"
    break;

  case 94: /* type_decl: TYPE ID INHERITS ID '(' args ')' '{' type_body '}'  */
#line 487 "src/parser/parser.y"
                                                                 {
                (yyval.node) = new TypeDeclarationNode(*(yyvsp[-8].str), new std::vector<Parameter>(), (yyvsp[-1].type_body), std::make_optional(*(yyvsp[-6].str)), *(yyvsp[-4].list), yylloc.first_line);
            }
#line 2354 ".build/parser.tab.cpp"
    break;

  case 95: /* type_decl: TYPE ID '(' params ')' INHERITS ID '{' type_body '}'  */
#line 490 "src/parser/parser.y"
                                                                   {
                (yyval.node) = new TypeDeclarationNode(*(yyvsp[-8].str), (yyvsp[-6].param), (yyvsp[-1].type_body), std::make_optional(*(yyvsp[-3].str)), std::vector<ASTNode*>(), yylloc.first_line);
            }
#line 2362 ".build/parser.tab.cpp"
    break;

  case 96: /* type_decl: TYPE ID INHERITS ID '{' type_body '}'  */
#line 493 "src/parser/parser.y"
                                                    {
                (yyval.node) = new TypeDeclarationNode(*(yyvsp[-5].str), new std::vector<Parameter>(), (yyvsp[-1].type_body), std::make_optional(*(yyvsp[-3].str)), std::vector<ASTNode*>(), yylloc.first_line);
            }
#line 2370 ".build/parser.tab.cpp"
    break;

  case 97: /* type_body: %empty  */
#line 499 "src/parser/parser.y"
                          {
                (yyval.type_body) = new TypeBody(new std::vector<AttributeDeclaration>(), new std::vector<MethodDeclaration>());
            }
#line 2378 ".build/parser.tab.cpp"
    break;

  case 98: /* type_body: attribute_decl  */
#line 502 "src/parser/parser.y"
                             {
                (yyval.type_body) = new TypeBody((yyvsp[0].attr_decl), new std::vector<MethodDeclaration>());
            }
#line 2386 ".build/parser.tab.cpp"
    break;

  case 99: /* type_body: method_decl  */
#line 505 "src/parser/parser.y"
                          {
                (yyval.type_body) = new TypeBody(new std::vector<AttributeDeclaration>(), (yyvsp[0].method_decl));
            }
#line 2394 ".build/parser.tab.cpp"
    break;

  case 100: /* type_body: attribute_decl method_decl  */
#line 508 "src/parser/parser.y"
                                         {
                (yyval.type_body) = new TypeBody((yyvsp[-1].attr_decl), (yyvsp[0].method_decl));
            }
#line 2402 ".build/parser.tab.cpp"
    break;

  case 101: /* attribute_decl: ID '=' expression ';'  */
#line 514 "src/parser/parser.y"
                                          { 
                (yyval.attr_decl) = new std::vector<AttributeDeclaration>();
                (yyval.attr_decl)->push_back(AttributeDeclaration(*(yyvsp[-3].str), (yyvsp[-1].node)));
            }
#line 2411 ".build/parser.tab.cpp"
    break;

  case 102: /* attribute_decl: attribute_decl ID '=' expression ';'  */
#line 518 "src/parser/parser.y"
                                                   { 
                (yyvsp[-4].attr_decl)->push_back(AttributeDeclaration(*(yyvsp[-3].str), (yyvsp[-1].node)));
                (yyval.attr_decl) = (yyvsp[-4].attr_decl);
            }
#line 2420 ".build/parser.tab.cpp"
    break;

  case 103: /* method_decl: ID '(' params ')' LAMBDA expression ';'  */
#line 525 "src/parser/parser.y"
                                                    {
                (yyval.method_decl) = new std::vector<MethodDeclaration>();
                (yyval.method_decl)->push_back(MethodDeclaration(*(yyvsp[-6].str), (yyvsp[-4].param), (yyvsp[-1].node)));
            }
#line 2429 ".build/parser.tab.cpp"
    break;

  case 104: /* method_decl: ID '(' params ')' block_expr ';'  */
#line 529 "src/parser/parser.y"
                                               {
                (yyval.method_decl) = new std::vector<MethodDeclaration>();
                (yyval.method_decl)->push_back(MethodDeclaration(*(yyvsp[-5].str), (yyvsp[-3].param), (yyvsp[-1].node)));
            }
#line 2438 ".build/parser.tab.cpp"
    break;

  case 105: /* method_decl: method_decl ID '(' params ')' LAMBDA expression ';'  */
#line 533 "src/parser/parser.y"
                                                                  {
                (yyvsp[-7].method_decl)->push_back(MethodDeclaration(*(yyvsp[-6].str), (yyvsp[-4].param), (yyvsp[-1].node)));
                (yyval.method_decl) = (yyvsp[-7].method_decl);
            }
#line 2447 ".build/parser.tab.cpp"
    break;

  case 106: /* method_decl: method_decl ID '(' params ')' block_expr ';'  */
#line 537 "src/parser/parser.y"
                                                           {
                (yyvsp[-6].method_decl)->push_back(MethodDeclaration(*(yyvsp[-5].str), (yyvsp[-3].param), (yyvsp[-1].node)));
                (yyval.method_decl) = (yyvsp[-6].method_decl);
            }
#line 2456 ".build/parser.tab.cpp"
    break;

  case 107: /* self_call: SELF '.' ID  */
#line 544 "src/parser/parser.y"
                          { (yyval.node) = new SelfCallNode(*(yyvsp[0].str), yylloc.first_line); }
#line 2462 ".build/parser.tab.cpp"
    break;

  case 108: /* method_call: ID '.' ID '(' args ')'  */
#line 548 "src/parser/parser.y"
                                   {
                (yyval.node) = new MethodCallNode(*(yyvsp[-5].str), *(yyvsp[-3].str), *(yyvsp[-1].list), yylloc.first_line);
            }
#line 2470 ".build/parser.tab.cpp"
    break;

  case 109: /* method_call: SELF '.' ID '(' args ')'  */
#line 551 "src/parser/parser.y"
                                       {
                (yyval.node) = new MethodCallNode("self", *(yyvsp[-3].str), *(yyvsp[-1].list), yylloc.first_line);
            }
#line 2478 ".build/parser.tab.cpp"
    break;

  case 110: /* base_call: BASE '(' args ')'  */
#line 557 "src/parser/parser.y"
                              {
                (yyval.node) = new BaseCallNode(*(yyvsp[-1].list), yylloc.first_line);
            }
#line 2486 ".build/parser.tab.cpp"
    break;

  case 111: /* new_instance: NEW ID '(' args ')'  */
#line 563 "src/parser/parser.y"
                                {
                (yyval.node) = new NewInstanceNode(*(yyvsp[-3].str), *(yyvsp[-1].list), yylloc.first_line);
            }
#line 2494 ".build/parser.tab.cpp"
    break;


#line 2498 ".build/parser.tab.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 568 "src/parser/parser.y"


void yyerror(const char *msg) {
    fprintf(stderr, "Error en línea %d, columna %d: %s\n",
            yylloc.first_line, yylloc.first_column, msg);
}


