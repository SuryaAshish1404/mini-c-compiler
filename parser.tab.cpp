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
#line 1 "src/parser.y"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include "symbol_table.h"

extern "C" {
#include "ast.h"
#include "ir_gen.h"
#include "semantic.h"
#include "codegen.h"
#include "optimizer.h"
#include "cfg.h"
#include "dag.h"
}

extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;

void yyerror(const char* msg);

SymbolTable sym_table;
int error_count = 0;
FILE* output_file = nullptr;
ASTNode* root_ast = nullptr;

bool check_tensor_compatibility(const std::string& lhs, const std::string& rhs, const std::string& op) {
    SymbolEntry* left = sym_table.lookup(lhs);
    SymbolEntry* right = sym_table.lookup(rhs);
    
    if (!left || !right) {
        std::cerr << "Semantic Error (line " << yylineno << "): undefined tensor in operation\n";
        error_count++;
        return false;
    }
    
    if (!left->is_tensor || !right->is_tensor) {
        return false;
    }
    
    if (left->num_dimensions != right->num_dimensions) {
        std::cerr << "Semantic Error (line " << yylineno << "): tensor dimension mismatch for " 
                  << op << "\n";
        error_count++;
        return false;
    }
    
    for (int i = 0; i < left->num_dimensions; i++) {
        if (left->shape[i] != right->shape[i]) {
            std::cerr << "Semantic Error (line " << yylineno << "): tensor shape mismatch: "
                      << lhs << " and " << rhs << " have incompatible shapes\n";
            error_count++;
            return false;
        }
    }
    
    return true;
}

void generate_tensor_operation(const std::string& dest, const std::string& lhs, 
                               const std::string& rhs, const std::string& op) {
    if (!output_file) return;
    
    SymbolEntry* tensor = sym_table.lookup(lhs);
    if (!tensor || !tensor->is_tensor) return;
    
    std::vector<std::string> loop_vars;
    for (int i = 0; i < tensor->num_dimensions; i++) {
        loop_vars.push_back("i" + std::to_string(i));
    }
    
    for (int i = 0; i < tensor->num_dimensions; i++) {
        for (int j = 0; j < i; j++) fprintf(output_file, "    ");
        fprintf(output_file, "for(int %s=0; %s<%d; %s++) {\n", 
                loop_vars[i].c_str(), loop_vars[i].c_str(), 
                tensor->shape[i], loop_vars[i].c_str());
    }
    
    for (int i = 0; i < tensor->num_dimensions; i++) fprintf(output_file, "    ");
    fprintf(output_file, "%s", dest.c_str());
    for (const auto& var : loop_vars) {
        fprintf(output_file, "[%s]", var.c_str());
    }
    fprintf(output_file, " = %s", lhs.c_str());
    for (const auto& var : loop_vars) {
        fprintf(output_file, "[%s]", var.c_str());
    }
    fprintf(output_file, " %s %s", op.c_str(), rhs.c_str());
    for (const auto& var : loop_vars) {
        fprintf(output_file, "[%s]", var.c_str());
    }
    fprintf(output_file, ";\n");
    
    for (int i = tensor->num_dimensions - 1; i >= 0; i--) {
        for (int j = 0; j < i; j++) fprintf(output_file, "    ");
        fprintf(output_file, "}\n");
    }
}

bool are_both_tensors(ASTNode* left, ASTNode* right) {
    if (!left || !right) return false;
    if (left->type != AST_IDENTIFIER || right->type != AST_IDENTIFIER) return false;
    
    SymbolEntry* left_entry = sym_table.lookup(left->name);
    SymbolEntry* right_entry = sym_table.lookup(right->name);
    
    if (!left_entry || !right_entry) return false;
    return left_entry->is_tensor && right_entry->is_tensor;
}

ASTNode* create_tensor_or_scalar_op(ASTNode* left, ASTNode* right, 
                                     ASTNodeType tensor_type, OperatorType scalar_op) {
    if (are_both_tensors(left, right)) {
        if (check_tensor_compatibility(left->name, right->name, 
                                       tensor_type == AST_TENSOR_ADD ? "+" : 
                                       tensor_type == AST_TENSOR_SUB ? "-" : "*")) {
            return create_tensor_op_node(tensor_type, left, right);
        }
    }
    return create_binary_node(AST_BINARY_OP, scalar_op, left, right);
}

#line 199 "parser.tab.cpp"

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
  YYSYMBOL_TOKEN_INT = 3,                  /* TOKEN_INT  */
  YYSYMBOL_TOKEN_FLOAT = 4,                /* TOKEN_FLOAT  */
  YYSYMBOL_TOKEN_CHAR = 5,                 /* TOKEN_CHAR  */
  YYSYMBOL_TOKEN_VOID = 6,                 /* TOKEN_VOID  */
  YYSYMBOL_TOKEN_TENSOR = 7,               /* TOKEN_TENSOR  */
  YYSYMBOL_TOKEN_IF = 8,                   /* TOKEN_IF  */
  YYSYMBOL_TOKEN_ELSE = 9,                 /* TOKEN_ELSE  */
  YYSYMBOL_TOKEN_WHILE = 10,               /* TOKEN_WHILE  */
  YYSYMBOL_TOKEN_FOR = 11,                 /* TOKEN_FOR  */
  YYSYMBOL_TOKEN_RETURN = 12,              /* TOKEN_RETURN  */
  YYSYMBOL_TOKEN_SWITCH = 13,              /* TOKEN_SWITCH  */
  YYSYMBOL_TOKEN_CASE = 14,                /* TOKEN_CASE  */
  YYSYMBOL_TOKEN_BREAK = 15,               /* TOKEN_BREAK  */
  YYSYMBOL_TOKEN_DEFAULT = 16,             /* TOKEN_DEFAULT  */
  YYSYMBOL_TOKEN_INT_LITERAL = 17,         /* TOKEN_INT_LITERAL  */
  YYSYMBOL_TOKEN_FLOAT_LITERAL = 18,       /* TOKEN_FLOAT_LITERAL  */
  YYSYMBOL_TOKEN_IDENTIFIER = 19,          /* TOKEN_IDENTIFIER  */
  YYSYMBOL_TOKEN_STRING_LITERAL = 20,      /* TOKEN_STRING_LITERAL  */
  YYSYMBOL_TOKEN_PLUS = 21,                /* TOKEN_PLUS  */
  YYSYMBOL_TOKEN_MINUS = 22,               /* TOKEN_MINUS  */
  YYSYMBOL_TOKEN_STAR = 23,                /* TOKEN_STAR  */
  YYSYMBOL_TOKEN_SLASH = 24,               /* TOKEN_SLASH  */
  YYSYMBOL_TOKEN_PERCENT = 25,             /* TOKEN_PERCENT  */
  YYSYMBOL_TOKEN_ASSIGN = 26,              /* TOKEN_ASSIGN  */
  YYSYMBOL_TOKEN_EQ = 27,                  /* TOKEN_EQ  */
  YYSYMBOL_TOKEN_NEQ = 28,                 /* TOKEN_NEQ  */
  YYSYMBOL_TOKEN_LT = 29,                  /* TOKEN_LT  */
  YYSYMBOL_TOKEN_GT = 30,                  /* TOKEN_GT  */
  YYSYMBOL_TOKEN_LEQ = 31,                 /* TOKEN_LEQ  */
  YYSYMBOL_TOKEN_GEQ = 32,                 /* TOKEN_GEQ  */
  YYSYMBOL_TOKEN_AND = 33,                 /* TOKEN_AND  */
  YYSYMBOL_TOKEN_OR = 34,                  /* TOKEN_OR  */
  YYSYMBOL_TOKEN_NOT = 35,                 /* TOKEN_NOT  */
  YYSYMBOL_TOKEN_INCREMENT = 36,           /* TOKEN_INCREMENT  */
  YYSYMBOL_TOKEN_DECREMENT = 37,           /* TOKEN_DECREMENT  */
  YYSYMBOL_TOKEN_PLUS_ASSIGN = 38,         /* TOKEN_PLUS_ASSIGN  */
  YYSYMBOL_TOKEN_MINUS_ASSIGN = 39,        /* TOKEN_MINUS_ASSIGN  */
  YYSYMBOL_TOKEN_STAR_ASSIGN = 40,         /* TOKEN_STAR_ASSIGN  */
  YYSYMBOL_TOKEN_SLASH_ASSIGN = 41,        /* TOKEN_SLASH_ASSIGN  */
  YYSYMBOL_TOKEN_LPAREN = 42,              /* TOKEN_LPAREN  */
  YYSYMBOL_TOKEN_RPAREN = 43,              /* TOKEN_RPAREN  */
  YYSYMBOL_TOKEN_LBRACE = 44,              /* TOKEN_LBRACE  */
  YYSYMBOL_TOKEN_RBRACE = 45,              /* TOKEN_RBRACE  */
  YYSYMBOL_TOKEN_LBRACKET = 46,            /* TOKEN_LBRACKET  */
  YYSYMBOL_TOKEN_RBRACKET = 47,            /* TOKEN_RBRACKET  */
  YYSYMBOL_TOKEN_SEMICOLON = 48,           /* TOKEN_SEMICOLON  */
  YYSYMBOL_TOKEN_COMMA = 49,               /* TOKEN_COMMA  */
  YYSYMBOL_TOKEN_COLON = 50,               /* TOKEN_COLON  */
  YYSYMBOL_UMINUS = 51,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 52,                  /* $accept  */
  YYSYMBOL_program = 53,                   /* program  */
  YYSYMBOL_declaration_list = 54,          /* declaration_list  */
  YYSYMBOL_declaration = 55,               /* declaration  */
  YYSYMBOL_type_specifier = 56,            /* type_specifier  */
  YYSYMBOL_variable_declaration = 57,      /* variable_declaration  */
  YYSYMBOL_tensor_declaration = 58,        /* tensor_declaration  */
  YYSYMBOL_dimension_list = 59,            /* dimension_list  */
  YYSYMBOL_function_declaration = 60,      /* function_declaration  */
  YYSYMBOL_parameter_list = 61,            /* parameter_list  */
  YYSYMBOL_parameter = 62,                 /* parameter  */
  YYSYMBOL_compound_statement = 63,        /* compound_statement  */
  YYSYMBOL_64_1 = 64,                      /* $@1  */
  YYSYMBOL_statement_list = 65,            /* statement_list  */
  YYSYMBOL_statement = 66,                 /* statement  */
  YYSYMBOL_expression_statement = 67,      /* expression_statement  */
  YYSYMBOL_selection_statement = 68,       /* selection_statement  */
  YYSYMBOL_iteration_statement = 69,       /* iteration_statement  */
  YYSYMBOL_return_statement = 70,          /* return_statement  */
  YYSYMBOL_switch_statement = 71,          /* switch_statement  */
  YYSYMBOL_case_list = 72,                 /* case_list  */
  YYSYMBOL_case_clause = 73,               /* case_clause  */
  YYSYMBOL_break_statement = 74,           /* break_statement  */
  YYSYMBOL_expression = 75,                /* expression  */
  YYSYMBOL_assignment_expression = 76,     /* assignment_expression  */
  YYSYMBOL_logical_or_expression = 77,     /* logical_or_expression  */
  YYSYMBOL_logical_and_expression = 78,    /* logical_and_expression  */
  YYSYMBOL_equality_expression = 79,       /* equality_expression  */
  YYSYMBOL_relational_expression = 80,     /* relational_expression  */
  YYSYMBOL_additive_expression = 81,       /* additive_expression  */
  YYSYMBOL_multiplicative_expression = 82, /* multiplicative_expression  */
  YYSYMBOL_unary_expression = 83,          /* unary_expression  */
  YYSYMBOL_postfix_expression = 84,        /* postfix_expression  */
  YYSYMBOL_argument_list = 85,             /* argument_list  */
  YYSYMBOL_primary_expression = 86         /* primary_expression  */
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
typedef yytype_uint8 yy_state_t;

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
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

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
#define YYFINAL  14
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   279

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  52
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  35
/* YYNRULES -- Number of rules.  */
#define YYNRULES  92
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  177

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   306


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
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   194,   194,   203,   208,   216,   217,   218,   222,   223,
     224,   225,   229,   236,   246,   256,   261,   270,   276,   285,
     290,   298,   307,   307,   315,   323,   328,   336,   337,   338,
     339,   340,   341,   342,   343,   347,   352,   360,   364,   371,
     375,   379,   386,   390,   397,   406,   411,   419,   425,   434,
     441,   445,   451,   458,   465,   472,   479,   483,   487,   491,
     495,   499,   503,   507,   511,   515,   519,   523,   527,   531,
     535,   539,   543,   547,   551,   555,   559,   563,   567,   572,
     577,   581,   582,   587,   592,   597,   605,   610,   618,   622,
     626,   631,   637
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
  "\"end of file\"", "error", "\"invalid token\"", "TOKEN_INT",
  "TOKEN_FLOAT", "TOKEN_CHAR", "TOKEN_VOID", "TOKEN_TENSOR", "TOKEN_IF",
  "TOKEN_ELSE", "TOKEN_WHILE", "TOKEN_FOR", "TOKEN_RETURN", "TOKEN_SWITCH",
  "TOKEN_CASE", "TOKEN_BREAK", "TOKEN_DEFAULT", "TOKEN_INT_LITERAL",
  "TOKEN_FLOAT_LITERAL", "TOKEN_IDENTIFIER", "TOKEN_STRING_LITERAL",
  "TOKEN_PLUS", "TOKEN_MINUS", "TOKEN_STAR", "TOKEN_SLASH",
  "TOKEN_PERCENT", "TOKEN_ASSIGN", "TOKEN_EQ", "TOKEN_NEQ", "TOKEN_LT",
  "TOKEN_GT", "TOKEN_LEQ", "TOKEN_GEQ", "TOKEN_AND", "TOKEN_OR",
  "TOKEN_NOT", "TOKEN_INCREMENT", "TOKEN_DECREMENT", "TOKEN_PLUS_ASSIGN",
  "TOKEN_MINUS_ASSIGN", "TOKEN_STAR_ASSIGN", "TOKEN_SLASH_ASSIGN",
  "TOKEN_LPAREN", "TOKEN_RPAREN", "TOKEN_LBRACE", "TOKEN_RBRACE",
  "TOKEN_LBRACKET", "TOKEN_RBRACKET", "TOKEN_SEMICOLON", "TOKEN_COMMA",
  "TOKEN_COLON", "UMINUS", "$accept", "program", "declaration_list",
  "declaration", "type_specifier", "variable_declaration",
  "tensor_declaration", "dimension_list", "function_declaration",
  "parameter_list", "parameter", "compound_statement", "$@1",
  "statement_list", "statement", "expression_statement",
  "selection_statement", "iteration_statement", "return_statement",
  "switch_statement", "case_list", "case_clause", "break_statement",
  "expression", "assignment_expression", "logical_or_expression",
  "logical_and_expression", "equality_expression", "relational_expression",
  "additive_expression", "multiplicative_expression", "unary_expression",
  "postfix_expression", "argument_list", "primary_expression", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-121)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      82,  -121,  -121,  -121,  -121,    52,    37,    82,  -121,    54,
    -121,  -121,  -121,    51,  -121,  -121,   -12,    61,    18,   219,
       6,  -121,    63,    99,  -121,  -121,  -121,   237,  -121,   230,
     230,   104,   110,   219,    85,  -121,   100,    97,     4,    25,
      69,    96,  -121,  -121,  -121,    98,   125,   -21,  -121,  -121,
     102,   219,  -121,  -121,   219,   219,   219,   219,    89,   -17,
    -121,  -121,  -121,  -121,   108,  -121,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   111,
    -121,  -121,    98,    77,  -121,  -121,  -121,  -121,  -121,  -121,
    -121,  -121,   -20,  -121,    97,     4,    25,    25,    69,    69,
      69,    69,    96,    96,  -121,  -121,  -121,  -121,   135,  -121,
    -121,  -121,   219,   116,   121,   122,   177,   123,   118,  -121,
     148,  -121,  -121,    57,  -121,  -121,  -121,  -121,  -121,  -121,
    -121,   120,  -121,   219,   219,   156,  -121,   132,   219,  -121,
     -24,  -121,  -121,  -121,   126,   138,   198,   198,  -121,   139,
     135,   135,   219,   219,   140,   176,  -121,   143,   144,    32,
     135,   135,   135,   171,   150,     2,  -121,  -121,  -121,  -121,
     151,   135,  -121,  -121,   135,   135,   135
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     8,     9,    10,    11,     0,     0,     2,     4,     0,
       5,     7,     6,     0,     1,     3,     0,     0,     0,     0,
       0,    12,     0,     0,    14,    88,    89,    91,    90,     0,
       0,     0,     0,     0,     0,    50,    56,    58,    60,    63,
      68,    71,    75,    80,    81,     0,     0,     0,    20,    16,
       0,     0,    82,    83,     0,     0,     0,     0,     0,    91,
      76,    77,    78,    79,     0,    13,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    22,
      18,    21,     0,     0,    15,    51,    52,    53,    54,    55,
      85,    87,     0,    92,    57,    59,    61,    62,    64,    65,
      66,    67,    69,    70,    72,    73,    74,    24,     0,    17,
      19,    84,     0,     0,     0,     0,     0,     0,     0,    36,
       0,    28,    29,     0,    26,    27,    30,    31,    32,    33,
      34,     0,    86,     0,     0,     0,    43,     0,     0,    49,
       0,    23,    25,    35,     0,     0,     0,     0,    42,     0,
       0,     0,     0,     0,     0,    37,    39,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    46,    38,    41,    40,
       0,     0,    44,    45,     0,    48,    47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -121,  -121,  -121,   182,     1,     0,  -121,  -121,  -121,  -121,
     107,   -32,  -121,   -76,  -117,  -120,  -121,  -121,  -121,  -121,
    -121,    38,  -121,   -16,  -121,  -121,   136,   141,    35,    42,
      53,   -25,  -121,  -121,  -121
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     6,     7,     8,   120,   121,    11,    18,    12,    47,
      48,   122,   108,   123,   124,   125,   126,   127,   128,   129,
     165,   166,   130,   131,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    92,    44
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      10,     9,    19,    34,    60,    61,   142,    10,     9,     1,
       2,     3,     4,    80,    19,   147,   163,    64,   164,    52,
      53,    46,    82,   111,    21,    58,   152,   153,    83,   112,
      20,    68,    69,   155,   156,    85,    21,    14,    86,    87,
      88,    89,    91,   167,   168,   169,   163,   172,   164,    45,
     109,   104,   105,   106,    70,    71,    72,    73,   142,   142,
       1,     2,     3,     4,    23,   113,    24,   114,   115,   116,
     117,    13,   118,    16,    25,    26,    27,    28,    22,    29,
       1,     2,     3,     4,    46,     1,     2,     3,     4,     5,
      74,    75,    30,    31,    32,   175,   132,    17,   176,    33,
     137,    79,   141,    96,    97,   119,    25,    26,    27,    28,
      49,    29,    98,    99,   100,   101,    50,   144,   145,    76,
      77,    78,   149,    62,    30,    31,    32,   102,   103,    63,
      67,    33,    90,    65,    66,   146,   157,   158,     1,     2,
       3,     4,    79,   113,    81,   114,   115,   116,   117,    84,
     118,    93,    25,    26,    27,    28,   107,    29,   133,     1,
       2,     3,     4,   134,   135,   138,   139,   140,   143,   150,
      30,    31,    32,    25,    26,    27,    28,    33,    29,    79,
     148,   151,   154,   119,   159,   160,   161,   162,   170,    15,
     110,    30,    31,    32,    25,    26,    27,    28,    33,    29,
     171,   174,    94,   173,   119,     0,     0,     0,    95,     0,
       0,     0,    30,    31,    32,    25,    26,    27,    28,    33,
      29,     0,     0,     0,     0,   136,     0,     0,     0,     0,
       0,     0,     0,    30,    31,    32,    25,    26,    27,    28,
      33,    29,     0,     0,     0,     0,   119,    25,    26,    59,
      28,     0,    29,     0,    30,    31,    32,     0,     0,     0,
       0,    33,     0,    51,     0,    30,    31,    32,     0,     0,
       0,     0,    33,    52,    53,    54,    55,    56,    57,    58
};

static const yytype_int16 yycheck[] =
{
       0,     0,    26,    19,    29,    30,   123,     7,     7,     3,
       4,     5,     6,    45,    26,   135,    14,    33,    16,    36,
      37,    20,    43,    43,    48,    42,   146,   147,    49,    49,
      42,    27,    28,   150,   151,    51,    48,     0,    54,    55,
      56,    57,    58,   160,   161,   162,    14,    45,    16,    43,
      82,    76,    77,    78,    29,    30,    31,    32,   175,   176,
       3,     4,     5,     6,    46,     8,    48,    10,    11,    12,
      13,    19,    15,    19,    17,    18,    19,    20,    17,    22,
       3,     4,     5,     6,    83,     3,     4,     5,     6,     7,
      21,    22,    35,    36,    37,   171,   112,    46,   174,    42,
     116,    44,    45,    68,    69,    48,    17,    18,    19,    20,
      47,    22,    70,    71,    72,    73,    17,   133,   134,    23,
      24,    25,   138,    19,    35,    36,    37,    74,    75,    19,
      33,    42,    43,    48,    34,   135,   152,   153,     3,     4,
       5,     6,    44,     8,    19,    10,    11,    12,    13,    47,
      15,    43,    17,    18,    19,    20,    45,    22,    42,     3,
       4,     5,     6,    42,    42,    42,    48,    19,    48,    43,
      35,    36,    37,    17,    18,    19,    20,    42,    22,    44,
      48,    43,    43,    48,    44,     9,    43,    43,    17,     7,
      83,    35,    36,    37,    17,    18,    19,    20,    42,    22,
      50,    50,    66,   165,    48,    -1,    -1,    -1,    67,    -1,
      -1,    -1,    35,    36,    37,    17,    18,    19,    20,    42,
      22,    -1,    -1,    -1,    -1,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    35,    36,    37,    17,    18,    19,    20,
      42,    22,    -1,    -1,    -1,    -1,    48,    17,    18,    19,
      20,    -1,    22,    -1,    35,    36,    37,    -1,    -1,    -1,
      -1,    42,    -1,    26,    -1,    35,    36,    37,    -1,    -1,
      -1,    -1,    42,    36,    37,    38,    39,    40,    41,    42
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,     7,    53,    54,    55,    56,
      57,    58,    60,    19,     0,    55,    19,    46,    59,    26,
      42,    48,    17,    46,    48,    17,    18,    19,    20,    22,
      35,    36,    37,    42,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    86,    43,    56,    61,    62,    47,
      17,    26,    36,    37,    38,    39,    40,    41,    42,    19,
      83,    83,    19,    19,    75,    48,    34,    33,    27,    28,
      29,    30,    31,    32,    21,    22,    23,    24,    25,    44,
      63,    19,    43,    49,    47,    75,    75,    75,    75,    75,
      43,    75,    85,    43,    78,    79,    80,    80,    81,    81,
      81,    81,    82,    82,    83,    83,    83,    45,    64,    63,
      62,    43,    49,     8,    10,    11,    12,    13,    15,    48,
      56,    57,    63,    65,    66,    67,    68,    69,    70,    71,
      74,    75,    75,    42,    42,    42,    48,    75,    42,    48,
      19,    45,    66,    48,    75,    75,    57,    67,    48,    75,
      43,    43,    67,    67,    43,    66,    66,    75,    75,    44,
       9,    43,    43,    14,    16,    72,    73,    66,    66,    66,
      17,    50,    45,    73,    50,    65,    65
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    52,    53,    54,    54,    55,    55,    55,    56,    56,
      56,    56,    57,    57,    58,    59,    59,    60,    60,    61,
      61,    62,    64,    63,    63,    65,    65,    66,    66,    66,
      66,    66,    66,    66,    66,    67,    67,    68,    68,    69,
      69,    69,    70,    70,    71,    72,    72,    73,    73,    74,
      75,    76,    76,    76,    76,    76,    76,    77,    77,    78,
      78,    79,    79,    79,    80,    80,    80,    80,    80,    81,
      81,    81,    82,    82,    82,    82,    83,    83,    83,    83,
      83,    84,    84,    84,    84,    84,    85,    85,    86,    86,
      86,    86,    86
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     5,     4,     4,     3,     6,     5,     3,
       1,     2,     0,     4,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     5,     7,     5,
       7,     7,     3,     2,     7,     2,     1,     4,     3,     2,
       1,     3,     3,     3,     3,     3,     1,     3,     1,     3,
       1,     3,     3,     1,     3,     3,     3,     3,     1,     3,
       3,     1,     3,     3,     3,     1,     2,     2,     2,     2,
       1,     1,     2,     2,     4,     3,     3,     1,     1,     1,
       1,     1,     3
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




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
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
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
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
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
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
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
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

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

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

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
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
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

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


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: declaration_list  */
#line 195 "src/parser.y"
        {
            (yyval.node) = create_list_node(AST_PROGRAM);
            if ((yyvsp[0].node)) add_child((yyval.node), (yyvsp[0].node));
            root_ast = (yyval.node);
        }
#line 1426 "parser.tab.cpp"
    break;

  case 3: /* declaration_list: declaration_list declaration  */
#line 204 "src/parser.y"
        {
            (yyval.node) = (yyvsp[-1].node);
            if ((yyvsp[0].node)) add_child((yyval.node), (yyvsp[0].node));
        }
#line 1435 "parser.tab.cpp"
    break;

  case 4: /* declaration_list: declaration  */
#line 209 "src/parser.y"
        {
            (yyval.node) = create_list_node(AST_DECLARATION_LIST);
            if ((yyvsp[0].node)) add_child((yyval.node), (yyvsp[0].node));
        }
#line 1444 "parser.tab.cpp"
    break;

  case 5: /* declaration: variable_declaration  */
#line 216 "src/parser.y"
                           { (yyval.node) = (yyvsp[0].node); }
#line 1450 "parser.tab.cpp"
    break;

  case 6: /* declaration: function_declaration  */
#line 217 "src/parser.y"
                           { (yyval.node) = (yyvsp[0].node); }
#line 1456 "parser.tab.cpp"
    break;

  case 7: /* declaration: tensor_declaration  */
#line 218 "src/parser.y"
                         { (yyval.node) = (yyvsp[0].node); }
#line 1462 "parser.tab.cpp"
    break;

  case 8: /* type_specifier: TOKEN_INT  */
#line 222 "src/parser.y"
                  { (yyval.sval) = strdup("int");   }
#line 1468 "parser.tab.cpp"
    break;

  case 9: /* type_specifier: TOKEN_FLOAT  */
#line 223 "src/parser.y"
                  { (yyval.sval) = strdup("float"); }
#line 1474 "parser.tab.cpp"
    break;

  case 10: /* type_specifier: TOKEN_CHAR  */
#line 224 "src/parser.y"
                  { (yyval.sval) = strdup("char");  }
#line 1480 "parser.tab.cpp"
    break;

  case 11: /* type_specifier: TOKEN_VOID  */
#line 225 "src/parser.y"
                  { (yyval.sval) = strdup("void");  }
#line 1486 "parser.tab.cpp"
    break;

  case 12: /* variable_declaration: type_specifier TOKEN_IDENTIFIER TOKEN_SEMICOLON  */
#line 230 "src/parser.y"
        {
            sym_table.insert((yyvsp[-1].sval), (yyvsp[-2].sval), SymbolKind::VARIABLE, yylineno);
            (yyval.node) = create_variable_decl_node((yyvsp[-2].sval), (yyvsp[-1].sval), NULL);
            (yyval.node)->line_number = yylineno;
            free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
        }
#line 1497 "parser.tab.cpp"
    break;

  case 13: /* variable_declaration: type_specifier TOKEN_IDENTIFIER TOKEN_ASSIGN expression TOKEN_SEMICOLON  */
#line 237 "src/parser.y"
        {
            sym_table.insert((yyvsp[-3].sval), (yyvsp[-4].sval), SymbolKind::VARIABLE, yylineno);
            (yyval.node) = create_variable_decl_node((yyvsp[-4].sval), (yyvsp[-3].sval), (yyvsp[-1].node));
            (yyval.node)->line_number = yylineno;
            free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
        }
#line 1508 "parser.tab.cpp"
    break;

  case 14: /* tensor_declaration: TOKEN_TENSOR TOKEN_IDENTIFIER dimension_list TOKEN_SEMICOLON  */
#line 247 "src/parser.y"
        {
            sym_table.insert_tensor((yyvsp[-2].sval), *(yyvsp[-1].dim_list), yylineno);
            (yyval.node) = create_tensor_decl_node((yyvsp[-2].sval), (yyvsp[-1].dim_list)->data(), (yyvsp[-1].dim_list)->size());
            free((yyvsp[-2].sval));
            delete (yyvsp[-1].dim_list);
        }
#line 1519 "parser.tab.cpp"
    break;

  case 15: /* dimension_list: dimension_list TOKEN_LBRACKET TOKEN_INT_LITERAL TOKEN_RBRACKET  */
#line 257 "src/parser.y"
        {
            (yyval.dim_list) = (yyvsp[-3].dim_list);
            (yyval.dim_list)->push_back((yyvsp[-1].ival));
        }
#line 1528 "parser.tab.cpp"
    break;

  case 16: /* dimension_list: TOKEN_LBRACKET TOKEN_INT_LITERAL TOKEN_RBRACKET  */
#line 262 "src/parser.y"
        {
            (yyval.dim_list) = new std::vector<int>();
            (yyval.dim_list)->push_back((yyvsp[-1].ival));
        }
#line 1537 "parser.tab.cpp"
    break;

  case 17: /* function_declaration: type_specifier TOKEN_IDENTIFIER TOKEN_LPAREN parameter_list TOKEN_RPAREN compound_statement  */
#line 271 "src/parser.y"
        {
            sym_table.insert((yyvsp[-4].sval), (yyvsp[-5].sval), SymbolKind::FUNCTION, yylineno);
            (yyval.node) = create_function_decl_node((yyvsp[-5].sval), (yyvsp[-4].sval), (yyvsp[-2].node), (yyvsp[0].node));
            free((yyvsp[-5].sval)); free((yyvsp[-4].sval));
        }
#line 1547 "parser.tab.cpp"
    break;

  case 18: /* function_declaration: type_specifier TOKEN_IDENTIFIER TOKEN_LPAREN TOKEN_RPAREN compound_statement  */
#line 277 "src/parser.y"
        {
            sym_table.insert((yyvsp[-3].sval), (yyvsp[-4].sval), SymbolKind::FUNCTION, yylineno);
            (yyval.node) = create_function_decl_node((yyvsp[-4].sval), (yyvsp[-3].sval), NULL, (yyvsp[0].node));
            free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
        }
#line 1557 "parser.tab.cpp"
    break;

  case 19: /* parameter_list: parameter_list TOKEN_COMMA parameter  */
#line 286 "src/parser.y"
        {
            (yyval.node) = (yyvsp[-2].node);
            if ((yyvsp[0].node)) add_child((yyval.node), (yyvsp[0].node));
        }
#line 1566 "parser.tab.cpp"
    break;

  case 20: /* parameter_list: parameter  */
#line 291 "src/parser.y"
        {
            (yyval.node) = create_list_node(AST_PARAM_LIST);
            if ((yyvsp[0].node)) add_child((yyval.node), (yyvsp[0].node));
        }
#line 1575 "parser.tab.cpp"
    break;

  case 21: /* parameter: type_specifier TOKEN_IDENTIFIER  */
#line 299 "src/parser.y"
        {
            sym_table.insert((yyvsp[0].sval), (yyvsp[-1].sval), SymbolKind::PARAMETER, yylineno);
            (yyval.node) = create_variable_decl_node((yyvsp[-1].sval), (yyvsp[0].sval), NULL);
            free((yyvsp[-1].sval)); free((yyvsp[0].sval));
        }
#line 1585 "parser.tab.cpp"
    break;

  case 22: /* $@1: %empty  */
#line 307 "src/parser.y"
                   { sym_table.enter_scope(); }
#line 1591 "parser.tab.cpp"
    break;

  case 23: /* compound_statement: TOKEN_LBRACE $@1 statement_list TOKEN_RBRACE  */
#line 310 "src/parser.y"
        { 
            sym_table.exit_scope();
            (yyval.node) = create_node(AST_COMPOUND_STMT);
            (yyval.node)->body = (yyvsp[-1].node);
        }
#line 1601 "parser.tab.cpp"
    break;

  case 24: /* compound_statement: TOKEN_LBRACE TOKEN_RBRACE  */
#line 316 "src/parser.y"
        {
            (yyval.node) = create_node(AST_COMPOUND_STMT);
            (yyval.node)->body = NULL;
        }
#line 1610 "parser.tab.cpp"
    break;

  case 25: /* statement_list: statement_list statement  */
#line 324 "src/parser.y"
        {
            (yyval.node) = (yyvsp[-1].node);
            if ((yyvsp[0].node)) add_child((yyval.node), (yyvsp[0].node));
        }
#line 1619 "parser.tab.cpp"
    break;

  case 26: /* statement_list: statement  */
#line 329 "src/parser.y"
        {
            (yyval.node) = create_list_node(AST_STATEMENT_LIST);
            if ((yyvsp[0].node)) add_child((yyval.node), (yyvsp[0].node));
        }
#line 1628 "parser.tab.cpp"
    break;

  case 27: /* statement: expression_statement  */
#line 336 "src/parser.y"
                           { (yyval.node) = (yyvsp[0].node); }
#line 1634 "parser.tab.cpp"
    break;

  case 28: /* statement: variable_declaration  */
#line 337 "src/parser.y"
                           { (yyval.node) = (yyvsp[0].node); }
#line 1640 "parser.tab.cpp"
    break;

  case 29: /* statement: compound_statement  */
#line 338 "src/parser.y"
                         { (yyval.node) = (yyvsp[0].node); }
#line 1646 "parser.tab.cpp"
    break;

  case 30: /* statement: selection_statement  */
#line 339 "src/parser.y"
                          { (yyval.node) = (yyvsp[0].node); }
#line 1652 "parser.tab.cpp"
    break;

  case 31: /* statement: iteration_statement  */
#line 340 "src/parser.y"
                          { (yyval.node) = (yyvsp[0].node); }
#line 1658 "parser.tab.cpp"
    break;

  case 32: /* statement: return_statement  */
#line 341 "src/parser.y"
                       { (yyval.node) = (yyvsp[0].node); }
#line 1664 "parser.tab.cpp"
    break;

  case 33: /* statement: switch_statement  */
#line 342 "src/parser.y"
                       { (yyval.node) = (yyvsp[0].node); }
#line 1670 "parser.tab.cpp"
    break;

  case 34: /* statement: break_statement  */
#line 343 "src/parser.y"
                      { (yyval.node) = (yyvsp[0].node); }
#line 1676 "parser.tab.cpp"
    break;

  case 35: /* expression_statement: expression TOKEN_SEMICOLON  */
#line 348 "src/parser.y"
        {
            (yyval.node) = create_node(AST_EXPR_STMT);
            (yyval.node)->left = (yyvsp[-1].node);
        }
#line 1685 "parser.tab.cpp"
    break;

  case 36: /* expression_statement: TOKEN_SEMICOLON  */
#line 353 "src/parser.y"
        {
            (yyval.node) = create_node(AST_EXPR_STMT);
            (yyval.node)->left = NULL;
        }
#line 1694 "parser.tab.cpp"
    break;

  case 37: /* selection_statement: TOKEN_IF TOKEN_LPAREN expression TOKEN_RPAREN statement  */
#line 361 "src/parser.y"
        {
            (yyval.node) = create_if_node((yyvsp[-2].node), (yyvsp[0].node), NULL);
        }
#line 1702 "parser.tab.cpp"
    break;

  case 38: /* selection_statement: TOKEN_IF TOKEN_LPAREN expression TOKEN_RPAREN statement TOKEN_ELSE statement  */
#line 365 "src/parser.y"
        {
            (yyval.node) = create_if_node((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node));
        }
#line 1710 "parser.tab.cpp"
    break;

  case 39: /* iteration_statement: TOKEN_WHILE TOKEN_LPAREN expression TOKEN_RPAREN statement  */
#line 372 "src/parser.y"
        {
            (yyval.node) = create_while_node((yyvsp[-2].node), (yyvsp[0].node));
        }
#line 1718 "parser.tab.cpp"
    break;

  case 40: /* iteration_statement: TOKEN_FOR TOKEN_LPAREN expression_statement expression_statement expression TOKEN_RPAREN statement  */
#line 376 "src/parser.y"
        {
            (yyval.node) = create_for_node((yyvsp[-4].node), (yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[0].node));
        }
#line 1726 "parser.tab.cpp"
    break;

  case 41: /* iteration_statement: TOKEN_FOR TOKEN_LPAREN variable_declaration expression_statement expression TOKEN_RPAREN statement  */
#line 380 "src/parser.y"
        {
            (yyval.node) = create_for_node((yyvsp[-4].node), (yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[0].node));
        }
#line 1734 "parser.tab.cpp"
    break;

  case 42: /* return_statement: TOKEN_RETURN expression TOKEN_SEMICOLON  */
#line 387 "src/parser.y"
        {
            (yyval.node) = create_return_node((yyvsp[-1].node));
        }
#line 1742 "parser.tab.cpp"
    break;

  case 43: /* return_statement: TOKEN_RETURN TOKEN_SEMICOLON  */
#line 391 "src/parser.y"
        {
            (yyval.node) = create_return_node(NULL);
        }
#line 1750 "parser.tab.cpp"
    break;

  case 44: /* switch_statement: TOKEN_SWITCH TOKEN_LPAREN expression TOKEN_RPAREN TOKEN_LBRACE case_list TOKEN_RBRACE  */
#line 398 "src/parser.y"
        {
            (yyval.node) = create_node(AST_COMPOUND_STMT);
            (yyval.node)->condition = (yyvsp[-4].node);
            (yyval.node)->body = (yyvsp[-1].node);
        }
#line 1760 "parser.tab.cpp"
    break;

  case 45: /* case_list: case_list case_clause  */
#line 407 "src/parser.y"
        {
            (yyval.node) = (yyvsp[-1].node);
            if ((yyvsp[0].node)) add_child((yyval.node), (yyvsp[0].node));
        }
#line 1769 "parser.tab.cpp"
    break;

  case 46: /* case_list: case_clause  */
#line 412 "src/parser.y"
        {
            (yyval.node) = create_list_node(AST_STATEMENT_LIST);
            if ((yyvsp[0].node)) add_child((yyval.node), (yyvsp[0].node));
        }
#line 1778 "parser.tab.cpp"
    break;

  case 47: /* case_clause: TOKEN_CASE TOKEN_INT_LITERAL TOKEN_COLON statement_list  */
#line 420 "src/parser.y"
        {
            (yyval.node) = create_node(AST_COMPOUND_STMT);
            (yyval.node)->int_value = (yyvsp[-2].ival);
            (yyval.node)->body = (yyvsp[0].node);
        }
#line 1788 "parser.tab.cpp"
    break;

  case 48: /* case_clause: TOKEN_DEFAULT TOKEN_COLON statement_list  */
#line 426 "src/parser.y"
        {
            (yyval.node) = create_node(AST_COMPOUND_STMT);
            (yyval.node)->int_value = -1;
            (yyval.node)->body = (yyvsp[0].node);
        }
#line 1798 "parser.tab.cpp"
    break;

  case 49: /* break_statement: TOKEN_BREAK TOKEN_SEMICOLON  */
#line 435 "src/parser.y"
        {
            (yyval.node) = create_node(AST_EXPR_STMT);
        }
#line 1806 "parser.tab.cpp"
    break;

  case 50: /* expression: assignment_expression  */
#line 441 "src/parser.y"
                            { (yyval.node) = (yyvsp[0].node); }
#line 1812 "parser.tab.cpp"
    break;

  case 51: /* assignment_expression: TOKEN_IDENTIFIER TOKEN_ASSIGN expression  */
#line 446 "src/parser.y"
        { 
            (yyval.node) = create_assignment_node(create_identifier_node((yyvsp[-2].sval)), (yyvsp[0].node));
            (yyval.node)->line_number = yylineno;
            free((yyvsp[-2].sval));
        }
#line 1822 "parser.tab.cpp"
    break;

  case 52: /* assignment_expression: TOKEN_IDENTIFIER TOKEN_PLUS_ASSIGN expression  */
#line 452 "src/parser.y"
        { 
            ASTNode *id = create_identifier_node((yyvsp[-2].sval));
            ASTNode *add = create_binary_node(AST_BINARY_OP, OP_ADD, id, (yyvsp[0].node));
            (yyval.node) = create_assignment_node(id, add);
            free((yyvsp[-2].sval));
        }
#line 1833 "parser.tab.cpp"
    break;

  case 53: /* assignment_expression: TOKEN_IDENTIFIER TOKEN_MINUS_ASSIGN expression  */
#line 459 "src/parser.y"
        { 
            ASTNode *id = create_identifier_node((yyvsp[-2].sval));
            ASTNode *sub = create_binary_node(AST_BINARY_OP, OP_SUB, id, (yyvsp[0].node));
            (yyval.node) = create_assignment_node(id, sub);
            free((yyvsp[-2].sval));
        }
#line 1844 "parser.tab.cpp"
    break;

  case 54: /* assignment_expression: TOKEN_IDENTIFIER TOKEN_STAR_ASSIGN expression  */
#line 466 "src/parser.y"
        { 
            ASTNode *id = create_identifier_node((yyvsp[-2].sval));
            ASTNode *mul = create_binary_node(AST_BINARY_OP, OP_MUL, id, (yyvsp[0].node));
            (yyval.node) = create_assignment_node(id, mul);
            free((yyvsp[-2].sval));
        }
#line 1855 "parser.tab.cpp"
    break;

  case 55: /* assignment_expression: TOKEN_IDENTIFIER TOKEN_SLASH_ASSIGN expression  */
#line 473 "src/parser.y"
        { 
            ASTNode *id = create_identifier_node((yyvsp[-2].sval));
            ASTNode *div = create_binary_node(AST_BINARY_OP, OP_DIV, id, (yyvsp[0].node));
            (yyval.node) = create_assignment_node(id, div);
            free((yyvsp[-2].sval));
        }
#line 1866 "parser.tab.cpp"
    break;

  case 56: /* assignment_expression: logical_or_expression  */
#line 479 "src/parser.y"
                            { (yyval.node) = (yyvsp[0].node); }
#line 1872 "parser.tab.cpp"
    break;

  case 57: /* logical_or_expression: logical_or_expression TOKEN_OR logical_and_expression  */
#line 484 "src/parser.y"
        {
            (yyval.node) = create_binary_node(AST_BINARY_OP, OP_OR, (yyvsp[-2].node), (yyvsp[0].node));
        }
#line 1880 "parser.tab.cpp"
    break;

  case 58: /* logical_or_expression: logical_and_expression  */
#line 487 "src/parser.y"
                             { (yyval.node) = (yyvsp[0].node); }
#line 1886 "parser.tab.cpp"
    break;

  case 59: /* logical_and_expression: logical_and_expression TOKEN_AND equality_expression  */
#line 492 "src/parser.y"
        {
            (yyval.node) = create_binary_node(AST_BINARY_OP, OP_AND, (yyvsp[-2].node), (yyvsp[0].node));
        }
#line 1894 "parser.tab.cpp"
    break;

  case 60: /* logical_and_expression: equality_expression  */
#line 495 "src/parser.y"
                          { (yyval.node) = (yyvsp[0].node); }
#line 1900 "parser.tab.cpp"
    break;

  case 61: /* equality_expression: equality_expression TOKEN_EQ relational_expression  */
#line 500 "src/parser.y"
        {
            (yyval.node) = create_binary_node(AST_BINARY_OP, OP_EQ, (yyvsp[-2].node), (yyvsp[0].node));
        }
#line 1908 "parser.tab.cpp"
    break;

  case 62: /* equality_expression: equality_expression TOKEN_NEQ relational_expression  */
#line 504 "src/parser.y"
        {
            (yyval.node) = create_binary_node(AST_BINARY_OP, OP_NEQ, (yyvsp[-2].node), (yyvsp[0].node));
        }
#line 1916 "parser.tab.cpp"
    break;

  case 63: /* equality_expression: relational_expression  */
#line 507 "src/parser.y"
                            { (yyval.node) = (yyvsp[0].node); }
#line 1922 "parser.tab.cpp"
    break;

  case 64: /* relational_expression: relational_expression TOKEN_LT additive_expression  */
#line 512 "src/parser.y"
        {
            (yyval.node) = create_binary_node(AST_BINARY_OP, OP_LT, (yyvsp[-2].node), (yyvsp[0].node));
        }
#line 1930 "parser.tab.cpp"
    break;

  case 65: /* relational_expression: relational_expression TOKEN_GT additive_expression  */
#line 516 "src/parser.y"
        {
            (yyval.node) = create_binary_node(AST_BINARY_OP, OP_GT, (yyvsp[-2].node), (yyvsp[0].node));
        }
#line 1938 "parser.tab.cpp"
    break;

  case 66: /* relational_expression: relational_expression TOKEN_LEQ additive_expression  */
#line 520 "src/parser.y"
        {
            (yyval.node) = create_binary_node(AST_BINARY_OP, OP_LEQ, (yyvsp[-2].node), (yyvsp[0].node));
        }
#line 1946 "parser.tab.cpp"
    break;

  case 67: /* relational_expression: relational_expression TOKEN_GEQ additive_expression  */
#line 524 "src/parser.y"
        {
            (yyval.node) = create_binary_node(AST_BINARY_OP, OP_GEQ, (yyvsp[-2].node), (yyvsp[0].node));
        }
#line 1954 "parser.tab.cpp"
    break;

  case 68: /* relational_expression: additive_expression  */
#line 527 "src/parser.y"
                          { (yyval.node) = (yyvsp[0].node); }
#line 1960 "parser.tab.cpp"
    break;

  case 69: /* additive_expression: additive_expression TOKEN_PLUS multiplicative_expression  */
#line 532 "src/parser.y"
        {
            (yyval.node) = create_tensor_or_scalar_op((yyvsp[-2].node), (yyvsp[0].node), AST_TENSOR_ADD, OP_ADD);
        }
#line 1968 "parser.tab.cpp"
    break;

  case 70: /* additive_expression: additive_expression TOKEN_MINUS multiplicative_expression  */
#line 536 "src/parser.y"
        {
            (yyval.node) = create_tensor_or_scalar_op((yyvsp[-2].node), (yyvsp[0].node), AST_TENSOR_SUB, OP_SUB);
        }
#line 1976 "parser.tab.cpp"
    break;

  case 71: /* additive_expression: multiplicative_expression  */
#line 539 "src/parser.y"
                                { (yyval.node) = (yyvsp[0].node); }
#line 1982 "parser.tab.cpp"
    break;

  case 72: /* multiplicative_expression: multiplicative_expression TOKEN_STAR unary_expression  */
#line 544 "src/parser.y"
        {
            (yyval.node) = create_tensor_or_scalar_op((yyvsp[-2].node), (yyvsp[0].node), AST_TENSOR_MUL, OP_MUL);
        }
#line 1990 "parser.tab.cpp"
    break;

  case 73: /* multiplicative_expression: multiplicative_expression TOKEN_SLASH unary_expression  */
#line 548 "src/parser.y"
        {
            (yyval.node) = create_binary_node(AST_BINARY_OP, OP_DIV, (yyvsp[-2].node), (yyvsp[0].node));
        }
#line 1998 "parser.tab.cpp"
    break;

  case 74: /* multiplicative_expression: multiplicative_expression TOKEN_PERCENT unary_expression  */
#line 552 "src/parser.y"
        {
            (yyval.node) = create_binary_node(AST_BINARY_OP, OP_MOD, (yyvsp[-2].node), (yyvsp[0].node));
        }
#line 2006 "parser.tab.cpp"
    break;

  case 75: /* multiplicative_expression: unary_expression  */
#line 555 "src/parser.y"
                       { (yyval.node) = (yyvsp[0].node); }
#line 2012 "parser.tab.cpp"
    break;

  case 76: /* unary_expression: TOKEN_MINUS unary_expression  */
#line 560 "src/parser.y"
        {
            (yyval.node) = create_unary_node(AST_UNARY_OP, OP_NEG, (yyvsp[0].node));
        }
#line 2020 "parser.tab.cpp"
    break;

  case 77: /* unary_expression: TOKEN_NOT unary_expression  */
#line 564 "src/parser.y"
        {
            (yyval.node) = create_unary_node(AST_UNARY_OP, OP_NOT, (yyvsp[0].node));
        }
#line 2028 "parser.tab.cpp"
    break;

  case 78: /* unary_expression: TOKEN_INCREMENT TOKEN_IDENTIFIER  */
#line 568 "src/parser.y"
        {
            (yyval.node) = create_identifier_node((yyvsp[0].sval));
            free((yyvsp[0].sval));
        }
#line 2037 "parser.tab.cpp"
    break;

  case 79: /* unary_expression: TOKEN_DECREMENT TOKEN_IDENTIFIER  */
#line 573 "src/parser.y"
        {
            (yyval.node) = create_identifier_node((yyvsp[0].sval));
            free((yyvsp[0].sval));
        }
#line 2046 "parser.tab.cpp"
    break;

  case 80: /* unary_expression: postfix_expression  */
#line 577 "src/parser.y"
                         { (yyval.node) = (yyvsp[0].node); }
#line 2052 "parser.tab.cpp"
    break;

  case 81: /* postfix_expression: primary_expression  */
#line 581 "src/parser.y"
                         { (yyval.node) = (yyvsp[0].node); }
#line 2058 "parser.tab.cpp"
    break;

  case 82: /* postfix_expression: TOKEN_IDENTIFIER TOKEN_INCREMENT  */
#line 583 "src/parser.y"
        {
            (yyval.node) = create_identifier_node((yyvsp[-1].sval));
            free((yyvsp[-1].sval));
        }
#line 2067 "parser.tab.cpp"
    break;

  case 83: /* postfix_expression: TOKEN_IDENTIFIER TOKEN_DECREMENT  */
#line 588 "src/parser.y"
        {
            (yyval.node) = create_identifier_node((yyvsp[-1].sval));
            free((yyvsp[-1].sval));
        }
#line 2076 "parser.tab.cpp"
    break;

  case 84: /* postfix_expression: TOKEN_IDENTIFIER TOKEN_LPAREN argument_list TOKEN_RPAREN  */
#line 593 "src/parser.y"
        {
            (yyval.node) = create_function_call_node((yyvsp[-3].sval), (yyvsp[-1].node));
            free((yyvsp[-3].sval));
        }
#line 2085 "parser.tab.cpp"
    break;

  case 85: /* postfix_expression: TOKEN_IDENTIFIER TOKEN_LPAREN TOKEN_RPAREN  */
#line 598 "src/parser.y"
        {
            (yyval.node) = create_function_call_node((yyvsp[-2].sval), NULL);
            free((yyvsp[-2].sval));
        }
#line 2094 "parser.tab.cpp"
    break;

  case 86: /* argument_list: argument_list TOKEN_COMMA expression  */
#line 606 "src/parser.y"
        {
            (yyval.node) = (yyvsp[-2].node);
            if ((yyvsp[0].node)) add_child((yyval.node), (yyvsp[0].node));
        }
#line 2103 "parser.tab.cpp"
    break;

  case 87: /* argument_list: expression  */
#line 611 "src/parser.y"
        {
            (yyval.node) = create_list_node(AST_ARG_LIST);
            if ((yyvsp[0].node)) add_child((yyval.node), (yyvsp[0].node));
        }
#line 2112 "parser.tab.cpp"
    break;

  case 88: /* primary_expression: TOKEN_INT_LITERAL  */
#line 619 "src/parser.y"
        {
            (yyval.node) = create_number_node((yyvsp[0].ival));
        }
#line 2120 "parser.tab.cpp"
    break;

  case 89: /* primary_expression: TOKEN_FLOAT_LITERAL  */
#line 623 "src/parser.y"
        {
            (yyval.node) = create_float_node((yyvsp[0].fval));
        }
#line 2128 "parser.tab.cpp"
    break;

  case 90: /* primary_expression: TOKEN_STRING_LITERAL  */
#line 627 "src/parser.y"
        {
            (yyval.node) = create_string_node((yyvsp[0].sval));
            free((yyvsp[0].sval));
        }
#line 2137 "parser.tab.cpp"
    break;

  case 91: /* primary_expression: TOKEN_IDENTIFIER  */
#line 632 "src/parser.y"
        {
            (yyval.node) = create_identifier_node((yyvsp[0].sval));
            (yyval.node)->line_number = yylineno;
            free((yyvsp[0].sval));
        }
#line 2147 "parser.tab.cpp"
    break;

  case 92: /* primary_expression: TOKEN_LPAREN expression TOKEN_RPAREN  */
#line 638 "src/parser.y"
        {
            (yyval.node) = (yyvsp[-1].node);
        }
#line 2155 "parser.tab.cpp"
    break;


#line 2159 "parser.tab.cpp"

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
                      yytoken, &yylval);
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


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 643 "src/parser.y"


/* ======================== Error handling ======================== */

void yyerror(const char* msg) {
    error_count++;
    std::cerr << "Syntax Error (line " << yylineno << "): " << msg
              << " near '" << yytext << "'" << std::endl;
}

/* ======================== Main entry point ======================== */

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file.c> [output_file.c]" << std::endl;
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        std::cerr << "Error: cannot open file '" << argv[1] << "'" << std::endl;
        return 1;
    }

    if (argc >= 3) {
        output_file = fopen(argv[2], "w");
        if (!output_file) {
            std::cerr << "Warning: cannot open output file '" << argv[2] << "'" << std::endl;
        }
    }

    yyin = file;
    std::cout << "Parsing '" << argv[1] << "'...\n" << std::endl;
    int result = yyparse();
    fclose(file);
    
    if (output_file) {
        fclose(output_file);
        if (result == 0 && error_count == 0) {
            std::cout << "Generated code written to '" << argv[2] << "'\n";
        }
    }

    sym_table.print();

    if (result == 0 && error_count == 0) {
        std::cout << "Parsing completed successfully. No errors found.\n";

        if (root_ast) {
            SemanticResult* sem_result = create_semantic_result();
            semantic_analyze(root_ast, &sym_table, sem_result);

            if (sem_result->error_count > 0) {
                std::cerr << "Semantic analysis failed with " << sem_result->error_count
                         << " error(s) and " << sem_result->warning_count << " warning(s).\n";
                free(sem_result);
                free_ast(root_ast);
                return 1;
            }

            if (sem_result->warning_count > 0) {
                std::cout << "Semantic analysis completed with " << sem_result->warning_count
                         << " warning(s).\n";
            }
            free(sem_result);

            if (argc >= 4) {
                /* Full compilation pipeline: generate IR, CFG, DAG, and assembly */

                /* ── DAG: deduplicate AST nodes, constant-fold, prune dead subtrees ── */
                DAG* dag = build_dag(root_ast);
                if (dag) {
                    print_dag(dag);
                    FILE* dag_dot = fopen("dag.dot", "w");
                    if (dag_dot) {
                        print_dag_dot(dag, dag_dot);
                        fclose(dag_dot);
                        std::cout << "DAG written to 'dag.dot' (render with: dot -Tpng dag.dot -o dag.png)\n";
                    }
                    free_dag(dag);
                }

                IRList* ir = generate_ir_from_ast(root_ast, &sym_table);
                if (ir) {
                    std::cout << "IR generated: " << ir->count << " instructions\n";
                    int opt_changes = optimize_ir(ir);
                    if (opt_changes > 0) {
                        std::cout << "Optimizations applied: " << opt_changes << " changes\n";
                    }

                    /* ── CFG: basic blocks, dominator tree, loop detection ── */
                    CFG* cfg = build_cfg(ir);
                    if (cfg) {
                        print_cfg(cfg);
                        print_dominator_tree(cfg);
                        print_loops(cfg);

                        /* optional: emit Graphviz DOT file */
                        FILE* dot_file = fopen("cfg.dot", "w");
                        if (dot_file) {
                            print_cfg_dot(cfg, dot_file);
                            fclose(dot_file);
                            std::cout << "CFG written to 'cfg.dot' (render with: dot -Tpng cfg.dot -o cfg.png)\n";
                        }
                        free_cfg(cfg);
                    }

                    FILE* asm_file = fopen(argv[3], "w");
                    if (asm_file) {
                        generate_assembly(ir, asm_file);
                        fclose(asm_file);
                        std::cout << "Assembly code written to '" << argv[3] << "'\n";
                    }
                    free_ir_list(ir);
                } else {
                    std::cout << "No IR generated\n";
                }
            }
            free_ast(root_ast);
        }
    } else {
        std::cerr << "Parsing finished with " << error_count << " error(s).\n";
        if (root_ast) {
            free_ast(root_ast);
        }
    }

    return (result == 0 && error_count == 0) ? 0 : 1;
}
