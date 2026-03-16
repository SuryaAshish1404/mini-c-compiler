












#define YYBISON 30802


#define YYBISON_VERSION "3.8.2"


#define YYSKELETON_NAME "yacc.c"


#define YYPURE 0


#define YYPUSH 0


#define YYPULL 1





#line 1 "parser.y"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include "symbol_table.h"

extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;

void yyerror(const char* msg);

SymbolTable sym_table;
int error_count = 0;
FILE* output_file = nullptr;

bool check_tensor_compatibility(const std::string& lhs, const std::string& rhs, const std::string& op) {
    SymbolEntry* left = sym_table.lookup(lhs);
    SymbolEntry* right = sym_table.lookup(rhs);
    
    if (!left || !right) {
        std::cerr << "Semantic Error (line " << yylineno << "): undefined tensor in operation\n";
        error_count++;
        return false;
    }
    
    if (!left->is_tensor || !right->is_tensor) {
        std::cerr << "Semantic Error (line " << yylineno << "): " << op 
                  << " requires tensor operands\n";
        error_count++;
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
    
    
    for (int i = 0; i < tensor->num_dimensions; i++) {
        fprintf(output_file, "    ");
    }
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

#line 173 "parser.tab.cpp"

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

enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      
  YYSYMBOL_YYerror = 1,                    
  YYSYMBOL_YYUNDEF = 2,                    
  YYSYMBOL_TOKEN_INT = 3,                  
  YYSYMBOL_TOKEN_FLOAT = 4,                
  YYSYMBOL_TOKEN_CHAR = 5,                 
  YYSYMBOL_TOKEN_VOID = 6,                 
  YYSYMBOL_TOKEN_TENSOR = 7,               
  YYSYMBOL_TOKEN_IF = 8,                   
  YYSYMBOL_TOKEN_ELSE = 9,                 
  YYSYMBOL_TOKEN_WHILE = 10,               
  YYSYMBOL_TOKEN_FOR = 11,                 
  YYSYMBOL_TOKEN_RETURN = 12,              
  YYSYMBOL_TOKEN_INT_LITERAL = 13,         
  YYSYMBOL_TOKEN_FLOAT_LITERAL = 14,       
  YYSYMBOL_TOKEN_IDENTIFIER = 15,          
  YYSYMBOL_TOKEN_STRING_LITERAL = 16,      
  YYSYMBOL_TOKEN_PLUS = 17,                
  YYSYMBOL_TOKEN_MINUS = 18,               
  YYSYMBOL_TOKEN_STAR = 19,                
  YYSYMBOL_TOKEN_SLASH = 20,               
  YYSYMBOL_TOKEN_PERCENT = 21,             
  YYSYMBOL_TOKEN_ASSIGN = 22,              
  YYSYMBOL_TOKEN_EQ = 23,                  
  YYSYMBOL_TOKEN_NEQ = 24,                 
  YYSYMBOL_TOKEN_LT = 25,                  
  YYSYMBOL_TOKEN_GT = 26,                  
  YYSYMBOL_TOKEN_LEQ = 27,                 
  YYSYMBOL_TOKEN_GEQ = 28,                 
  YYSYMBOL_TOKEN_AND = 29,                 
  YYSYMBOL_TOKEN_OR = 30,                  
  YYSYMBOL_TOKEN_NOT = 31,                 
  YYSYMBOL_TOKEN_INCREMENT = 32,           
  YYSYMBOL_TOKEN_DECREMENT = 33,           
  YYSYMBOL_TOKEN_PLUS_ASSIGN = 34,         
  YYSYMBOL_TOKEN_MINUS_ASSIGN = 35,        
  YYSYMBOL_TOKEN_STAR_ASSIGN = 36,         
  YYSYMBOL_TOKEN_SLASH_ASSIGN = 37,        
  YYSYMBOL_TOKEN_LPAREN = 38,              
  YYSYMBOL_TOKEN_RPAREN = 39,              
  YYSYMBOL_TOKEN_LBRACE = 40,              
  YYSYMBOL_TOKEN_RBRACE = 41,              
  YYSYMBOL_TOKEN_LBRACKET = 42,            
  YYSYMBOL_TOKEN_RBRACKET = 43,            
  YYSYMBOL_TOKEN_SEMICOLON = 44,           
  YYSYMBOL_TOKEN_COMMA = 45,               
  YYSYMBOL_UMINUS = 46,                    
  YYSYMBOL_YYACCEPT = 47,                  
  YYSYMBOL_program = 48,                   
  YYSYMBOL_declaration_list = 49,          
  YYSYMBOL_declaration = 50,               
  YYSYMBOL_type_specifier = 51,            
  YYSYMBOL_variable_declaration = 52,      
  YYSYMBOL_tensor_declaration = 53,        
  YYSYMBOL_dimension_list = 54,            
  YYSYMBOL_function_declaration = 55,      
  YYSYMBOL_parameter_list = 56,            
  YYSYMBOL_parameter = 57,                 
  YYSYMBOL_compound_statement = 58,        
  YYSYMBOL_59_1 = 59,                      
  YYSYMBOL_statement_list = 60,            
  YYSYMBOL_statement = 61,                 
  YYSYMBOL_expression_statement = 62,      
  YYSYMBOL_selection_statement = 63,       
  YYSYMBOL_iteration_statement = 64,       
  YYSYMBOL_return_statement = 65,          
  YYSYMBOL_expression = 66,                
  YYSYMBOL_assignment_expression = 67,     
  YYSYMBOL_logical_or_expression = 68,     
  YYSYMBOL_logical_and_expression = 69,    
  YYSYMBOL_equality_expression = 70,       
  YYSYMBOL_relational_expression = 71,     
  YYSYMBOL_additive_expression = 72,       
  YYSYMBOL_multiplicative_expression = 73, 
  YYSYMBOL_unary_expression = 74,          
  YYSYMBOL_postfix_expression = 75,        
  YYSYMBOL_argument_list = 76,             
  YYSYMBOL_primary_expression = 77         
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif



#ifndef __PTRDIFF_MAX__
# include <limits.h> 
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> 
#  define YY_STDINT_H
# endif
#endif



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
#   include <stddef.h> 
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
#  include <stddef.h> 
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



typedef yytype_uint8 yy_state_t;


typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> 
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


#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) 
#endif


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
# define YY_INITIAL_VALUE(Value) 
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



# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> 
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> 
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> 
      
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   
#  define YYSTACK_FREE(Ptr) do { ; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    
#   define YYSTACK_ALLOC_MAXIMUM 4032 
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
#   include <stdlib.h> 
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); 
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); 
#   endif
#  endif
# endif
#endif 

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))


union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};


# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)


# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1


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
#endif 


#define YYFINAL  14

#define YYLAST   294


#define YYNTOKENS  47

#define YYNNTS  31

#define YYNRULES  87

#define YYNSTATES  164


#define YYMAXUTOK   301



#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)


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
      45,    46
};

#if YYDEBUG

static const yytype_int16 yyrline[] =
{
       0,   163,   163,   167,   168,   172,   173,   174,   179,   180,
     181,   182,   187,   192,   201,   210,   215,   224,   229,   237,
     238,   242,   251,   251,   254,   258,   259,   263,   264,   265,
     266,   267,   268,   272,   273,   278,   279,   283,   284,   285,
     289,   290,   295,   299,   301,   303,   305,   307,   309,   321,
     333,   345,   349,   350,   354,   355,   359,   360,   361,   365,
     366,   367,   368,   369,   373,   374,   375,   379,   380,   381,
     382,   386,   387,   388,   389,   390,   394,   395,   396,   397,
     398,   402,   403,   407,   408,   409,   410,   411
};
#endif


#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0

static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;


static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "TOKEN_INT",
  "TOKEN_FLOAT", "TOKEN_CHAR", "TOKEN_VOID", "TOKEN_TENSOR", "TOKEN_IF",
  "TOKEN_ELSE", "TOKEN_WHILE", "TOKEN_FOR", "TOKEN_RETURN",
  "TOKEN_INT_LITERAL", "TOKEN_FLOAT_LITERAL", "TOKEN_IDENTIFIER",
  "TOKEN_STRING_LITERAL", "TOKEN_PLUS", "TOKEN_MINUS", "TOKEN_STAR",
  "TOKEN_SLASH", "TOKEN_PERCENT", "TOKEN_ASSIGN", "TOKEN_EQ", "TOKEN_NEQ",
  "TOKEN_LT", "TOKEN_GT", "TOKEN_LEQ", "TOKEN_GEQ", "TOKEN_AND",
  "TOKEN_OR", "TOKEN_NOT", "TOKEN_INCREMENT", "TOKEN_DECREMENT",
  "TOKEN_PLUS_ASSIGN", "TOKEN_MINUS_ASSIGN", "TOKEN_STAR_ASSIGN",
  "TOKEN_SLASH_ASSIGN", "TOKEN_LPAREN", "TOKEN_RPAREN", "TOKEN_LBRACE",
  "TOKEN_RBRACE", "TOKEN_LBRACKET", "TOKEN_RBRACKET", "TOKEN_SEMICOLON",
  "TOKEN_COMMA", "UMINUS", "$accept", "program", "declaration_list",
  "declaration", "type_specifier", "variable_declaration",
  "tensor_declaration", "dimension_list", "function_declaration",
  "parameter_list", "parameter", "compound_statement", "$@1",
  "statement_list", "statement", "expression_statement",
  "selection_statement", "iteration_statement", "return_statement",
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

#define YYPACT_NINF (-123)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0


static const yytype_int16 yypact[] =
{
      68,  -123,  -123,  -123,  -123,    12,    34,    68,  -123,    26,
    -123,  -123,  -123,     1,  -123,  -123,   -14,    32,   -23,   210,
       7,  -123,    14,    46,  -123,  -123,  -123,    79,  -123,   231,
     231,    52,    61,   210,    36,  -123,    54,    60,     8,   101,
      69,    28,  -123,  -123,  -123,    57,    84,   -25,  -123,  -123,
      66,   239,  -123,  -123,   210,   210,   210,   210,   199,   -15,
    -123,  -123,  -123,  -123,    64,  -123,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,    59,
    -123,  -123,    57,   152,  -123,   256,  -123,  -123,  -123,  -123,
    -123,  -123,  -123,   -10,  -123,    60,     8,   101,   101,    69,
      69,    69,    69,    28,    28,  -123,  -123,  -123,  -123,   136,
    -123,  -123,    89,   105,   106,  -123,   210,    93,    94,    95,
      92,  -123,   107,  -123,  -123,    50,  -123,  -123,  -123,  -123,
    -123,    99,  -123,  -123,  -123,  -123,   210,   210,   157,  -123,
     109,   -16,  -123,  -123,  -123,    98,   120,   178,   178,  -123,
     136,   136,   210,   210,   155,  -123,   126,   127,   136,   136,
     136,  -123,  -123,  -123
};


static const yytype_int8 yydefact[] =
{
       0,     8,     9,    10,    11,     0,     0,     2,     4,     0,
       5,     7,     6,     0,     1,     3,     0,     0,     0,     0,
       0,    12,     0,     0,    14,    83,    84,    86,    85,     0,
       0,     0,     0,     0,     0,    42,    51,    53,    55,    58,
      63,    66,    70,    75,    76,     0,     0,     0,    20,    16,
       0,     0,    77,    78,     0,     0,     0,     0,     0,    86,
      71,    72,    73,    74,     0,    13,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    22,
      18,    21,     0,     0,    15,    86,    43,    44,    45,    46,
      47,    80,    82,     0,    87,    52,    54,    56,    57,    59,
      60,    61,    62,    64,    65,    67,    68,    69,    24,     0,
      17,    19,     0,     0,     0,    79,     0,     0,     0,     0,
       0,    34,     0,    28,    29,     0,    26,    27,    30,    31,
      32,     0,    48,    49,    50,    81,     0,     0,     0,    41,
       0,     0,    23,    25,    33,     0,     0,     0,     0,    40,
       0,     0,     0,     0,    35,    37,     0,     0,     0,     0,
       0,    36,    39,    38
};


static const yytype_int16 yypgoto[] =
{
    -123,  -123,  -123,   138,     2,     0,  -123,  -123,  -123,  -123,
      96,   -40,  -123,  -123,   -81,  -122,  -123,  -123,  -123,   -18,
    -123,  -123,   112,   110,    24,   111,    21,   -26,  -123,  -123,
    -123
};


static const yytype_uint8 yydefgoto[] =
{
       0,     6,     7,     8,   122,   123,    11,    18,    12,    47,
      48,   124,   109,   125,   126,   127,   128,   129,   130,   131,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    93,
      44
};


static const yytype_uint8 yytable[] =
{
      10,    34,     9,    60,    61,    80,    19,    10,    19,     9,
       1,     2,     3,     4,    82,    64,   148,    52,    53,    23,
      83,    24,    46,    58,    20,   152,   153,    13,    21,   115,
      21,    68,    69,    86,    14,   116,    87,    88,    89,    90,
      92,    16,   110,    17,   143,    22,    45,    76,    77,    78,
     105,   106,   107,     1,     2,     3,     4,    49,   117,    50,
     118,   119,   120,    25,    26,    27,    28,    62,    29,   154,
     155,     1,     2,     3,     4,     5,    63,   161,   162,   163,
      65,    30,    31,    32,    66,    46,    74,    75,    33,    67,
      79,   142,    97,    98,   121,   103,   104,    79,   135,    81,
     108,    51,   140,    94,   132,    25,    26,    27,    28,    84,
      29,    52,    53,    54,    55,    56,    57,    58,   145,   146,
     133,   134,   141,    30,    31,    32,    70,    71,    72,    73,
      33,   136,   137,   138,   156,   157,   139,   150,   147,     1,
       2,     3,     4,   144,   117,    15,   118,   119,   120,    25,
      26,    27,    28,   149,    29,     1,     2,     3,     4,   151,
       1,     2,     3,     4,   158,   159,   160,    30,    31,    32,
      25,    26,    27,    28,    33,    29,    79,    96,    95,   111,
     121,    99,   100,   101,   102,     0,     0,     0,    30,    31,
      32,    25,    26,    27,    28,    33,    29,     0,     0,     0,
       0,   121,     0,     0,     0,     0,     0,     0,     0,    30,
      31,    32,    25,    26,    27,    28,    33,    29,     0,     0,
       0,     0,   121,    25,    26,    27,    28,     0,    29,     0,
      30,    31,    32,     0,     0,     0,     0,    33,    91,     0,
       0,    30,    31,    32,    25,    26,    59,    28,    33,    29,
       0,     0,    25,    26,    85,    28,     0,    29,     0,     0,
       0,     0,    30,    31,    32,     0,     0,     0,     0,    33,
      30,    31,    32,   112,   113,   114,     0,    33,    51,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    52,    53,
      54,    55,    56,    57,    58
};

static const yytype_int16 yycheck[] =
{
       0,    19,     0,    29,    30,    45,    22,     7,    22,     7,
       3,     4,     5,     6,    39,    33,   138,    32,    33,    42,
      45,    44,    20,    38,    38,   147,   148,    15,    44,    39,
      44,    23,    24,    51,     0,    45,    54,    55,    56,    57,
      58,    15,    82,    42,   125,    13,    39,    19,    20,    21,
      76,    77,    78,     3,     4,     5,     6,    43,     8,    13,
      10,    11,    12,    13,    14,    15,    16,    15,    18,   150,
     151,     3,     4,     5,     6,     7,    15,   158,   159,   160,
      44,    31,    32,    33,    30,    83,    17,    18,    38,    29,
      40,    41,    68,    69,    44,    74,    75,    40,   116,    15,
      41,    22,   120,    39,    15,    13,    14,    15,    16,    43,
      18,    32,    33,    34,    35,    36,    37,    38,   136,   137,
      15,    15,    15,    31,    32,    33,    25,    26,    27,    28,
      38,    38,    38,    38,   152,   153,    44,    39,   138,     3,
       4,     5,     6,    44,     8,     7,    10,    11,    12,    13,
      14,    15,    16,    44,    18,     3,     4,     5,     6,    39,
       3,     4,     5,     6,     9,    39,    39,    31,    32,    33,
      13,    14,    15,    16,    38,    18,    40,    67,    66,    83,
      44,    70,    71,    72,    73,    -1,    -1,    -1,    31,    32,
      33,    13,    14,    15,    16,    38,    18,    -1,    -1,    -1,
      -1,    44,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,
      32,    33,    13,    14,    15,    16,    38,    18,    -1,    -1,
      -1,    -1,    44,    13,    14,    15,    16,    -1,    18,    -1,
      31,    32,    33,    -1,    -1,    -1,    -1,    38,    39,    -1,
      -1,    31,    32,    33,    13,    14,    15,    16,    38,    18,
      -1,    -1,    13,    14,    15,    16,    -1,    18,    -1,    -1,
      -1,    -1,    31,    32,    33,    -1,    -1,    -1,    -1,    38,
      31,    32,    33,    17,    18,    19,    -1,    38,    22,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    33,
      34,    35,    36,    37,    38
};


static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,     7,    48,    49,    50,    51,
      52,    53,    55,    15,     0,    50,    15,    42,    54,    22,
      38,    44,    13,    42,    44,    13,    14,    15,    16,    18,
      31,    32,    33,    38,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    77,    39,    51,    56,    57,    43,
      13,    22,    32,    33,    34,    35,    36,    37,    38,    15,
      74,    74,    15,    15,    66,    44,    30,    29,    23,    24,
      25,    26,    27,    28,    17,    18,    19,    20,    21,    40,
      58,    15,    39,    45,    43,    15,    66,    66,    66,    66,
      66,    39,    66,    76,    39,    69,    70,    71,    71,    72,
      72,    72,    72,    73,    73,    74,    74,    74,    41,    59,
      58,    57,    17,    18,    19,    39,    45,     8,    10,    11,
      12,    44,    51,    52,    58,    60,    61,    62,    63,    64,
      65,    66,    15,    15,    15,    66,    38,    38,    38,    44,
      66,    15,    41,    61,    44,    66,    66,    52,    62,    44,
      39,    39,    62,    62,    61,    61,    66,    66,     9,    39,
      39,    61,    61,    61
};


static const yytype_int8 yyr1[] =
{
       0,    47,    48,    49,    49,    50,    50,    50,    51,    51,
      51,    51,    52,    52,    53,    54,    54,    55,    55,    56,
      56,    57,    59,    58,    58,    60,    60,    61,    61,    61,
      61,    61,    61,    62,    62,    63,    63,    64,    64,    64,
      65,    65,    66,    67,    67,    67,    67,    67,    67,    67,
      67,    67,    68,    68,    69,    69,    70,    70,    70,    71,
      71,    71,    71,    71,    72,    72,    72,    73,    73,    73,
      73,    74,    74,    74,    74,    74,    75,    75,    75,    75,
      75,    76,    76,    77,    77,    77,    77,    77
};


static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     5,     4,     4,     3,     6,     5,     3,
       1,     2,     0,     4,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     5,     7,     5,     7,     7,
       3,     2,     1,     3,     3,     3,     3,     3,     5,     5,
       5,     1,     3,     1,     3,     1,     3,     3,     1,     3,
       3,     3,     3,     1,     3,     3,     1,     3,     3,     3,
       1,     2,     2,     2,     2,     1,     1,     2,     2,     4,
       3,     3,     1,     1,     1,     1,     1,     3
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


#define YYERRCODE YYUNDEF



#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> 
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




static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}



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




static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  
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


int yydebug;
#else 
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif 



#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif



#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif








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



int yychar;


YYSTYPE yylval;

int yynerrs;






int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    
    int yyerrstatus = 0;

    

    
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  
  int yyresult;
  
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; 

  goto yysetstate;



yynewstate:
  
  yyssp++;



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
      
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else 
      
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
#endif 


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;



yybackup:
  

  
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  

  
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
      
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  
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

  
  if (yyerrstatus)
    yyerrstatus--;

  
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  
  yychar = YYEMPTY;
  goto yynewstate;



yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;



yyreduce:
  
  yylen = yyr2[yyn];

  
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 8: 
#line 179 "parser.y"
                  { (yyval.sval) = strdup("int");   }
#line 1383 "parser.tab.cpp"
    break;

  case 9: 
#line 180 "parser.y"
                  { (yyval.sval) = strdup("float"); }
#line 1389 "parser.tab.cpp"
    break;

  case 10: 
#line 181 "parser.y"
                  { (yyval.sval) = strdup("char");  }
#line 1395 "parser.tab.cpp"
    break;

  case 11: 
#line 182 "parser.y"
                  { (yyval.sval) = strdup("void");  }
#line 1401 "parser.tab.cpp"
    break;

  case 12: 
#line 188 "parser.y"
        {
            sym_table.insert((yyvsp[-1].sval), (yyvsp[-2].sval), SymbolKind::VARIABLE, yylineno);
            free((yyvsp[-2].sval)); free((yyvsp[-1].sval));
        }
#line 1410 "parser.tab.cpp"
    break;

  case 13: 
#line 193 "parser.y"
        {
            sym_table.insert((yyvsp[-3].sval), (yyvsp[-4].sval), SymbolKind::VARIABLE, yylineno);
            free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
        }
#line 1419 "parser.tab.cpp"
    break;

  case 14: 
#line 202 "parser.y"
        {
            sym_table.insert_tensor((yyvsp[-2].sval), *(yyvsp[-1].dim_list), yylineno);
            free((yyvsp[-2].sval));
            delete (yyvsp[-1].dim_list);
        }
#line 1429 "parser.tab.cpp"
    break;

  case 15: 
#line 211 "parser.y"
        {
            (yyval.dim_list) = (yyvsp[-3].dim_list);
            (yyval.dim_list)->push_back((yyvsp[-1].ival));
        }
#line 1438 "parser.tab.cpp"
    break;

  case 16: 
#line 216 "parser.y"
        {
            (yyval.dim_list) = new std::vector<int>();
            (yyval.dim_list)->push_back((yyvsp[-1].ival));
        }
#line 1447 "parser.tab.cpp"
    break;

  case 17: 
#line 225 "parser.y"
        {
            sym_table.insert((yyvsp[-4].sval), (yyvsp[-5].sval), SymbolKind::FUNCTION, yylineno);
            free((yyvsp[-5].sval)); free((yyvsp[-4].sval));
        }
#line 1456 "parser.tab.cpp"
    break;

  case 18: 
#line 230 "parser.y"
        {
            sym_table.insert((yyvsp[-3].sval), (yyvsp[-4].sval), SymbolKind::FUNCTION, yylineno);
            free((yyvsp[-4].sval)); free((yyvsp[-3].sval));
        }
#line 1465 "parser.tab.cpp"
    break;

  case 21: 
#line 243 "parser.y"
        {
            sym_table.insert((yyvsp[0].sval), (yyvsp[-1].sval), SymbolKind::PARAMETER, yylineno);
            free((yyvsp[-1].sval)); free((yyvsp[0].sval));
        }
#line 1474 "parser.tab.cpp"
    break;

  case 22: 
#line 251 "parser.y"
                   { sym_table.enter_scope(); }
#line 1480 "parser.tab.cpp"
    break;

  case 23: 
#line 253 "parser.y"
                   { sym_table.exit_scope(); }
#line 1486 "parser.tab.cpp"
    break;

  case 43: 
#line 300 "parser.y"
        { free((yyvsp[-2].sval)); }
#line 1492 "parser.tab.cpp"
    break;

  case 44: 
#line 302 "parser.y"
        { free((yyvsp[-2].sval)); }
#line 1498 "parser.tab.cpp"
    break;

  case 45: 
#line 304 "parser.y"
        { free((yyvsp[-2].sval)); }
#line 1504 "parser.tab.cpp"
    break;

  case 46: 
#line 306 "parser.y"
        { free((yyvsp[-2].sval)); }
#line 1510 "parser.tab.cpp"
    break;

  case 47: 
#line 308 "parser.y"
        { free((yyvsp[-2].sval)); }
#line 1516 "parser.tab.cpp"
    break;

  case 48: 
#line 310 "parser.y"
        {
            if (check_tensor_compatibility((yyvsp[-2].sval), (yyvsp[0].sval), "addition")) {
                SymbolEntry* dest = sym_table.lookup((yyvsp[-4].sval));
                if (dest && dest->is_tensor) {
                    if (check_tensor_compatibility((yyvsp[-4].sval), (yyvsp[-2].sval), "assignment")) {
                        generate_tensor_operation((yyvsp[-4].sval), (yyvsp[-2].sval), (yyvsp[0].sval), "+");
                    }
                }
            }
            free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[0].sval));
        }
#line 1532 "parser.tab.cpp"
    break;

  case 49: 
#line 322 "parser.y"
        {
            if (check_tensor_compatibility((yyvsp[-2].sval), (yyvsp[0].sval), "subtraction")) {
                SymbolEntry* dest = sym_table.lookup((yyvsp[-4].sval));
                if (dest && dest->is_tensor) {
                    if (check_tensor_compatibility((yyvsp[-4].sval), (yyvsp[-2].sval), "assignment")) {
                        generate_tensor_operation((yyvsp[-4].sval), (yyvsp[-2].sval), (yyvsp[0].sval), "-");
                    }
                }
            }
            free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[0].sval));
        }
#line 1548 "parser.tab.cpp"
    break;

  case 50: 
#line 334 "parser.y"
        {
            if (check_tensor_compatibility((yyvsp[-2].sval), (yyvsp[0].sval), "multiplication")) {
                SymbolEntry* dest = sym_table.lookup((yyvsp[-4].sval));
                if (dest && dest->is_tensor) {
                    if (check_tensor_compatibility((yyvsp[-4].sval), (yyvsp[-2].sval), "assignment")) {
                        generate_tensor_operation((yyvsp[-4].sval), (yyvsp[-2].sval), (yyvsp[0].sval), "*");
                    }
                }
            }
            free((yyvsp[-4].sval)); free((yyvsp[-2].sval)); free((yyvsp[0].sval));
        }
#line 1564 "parser.tab.cpp"
    break;

  case 73: 
#line 388 "parser.y"
                                        { free((yyvsp[0].sval)); }
#line 1570 "parser.tab.cpp"
    break;

  case 74: 
#line 389 "parser.y"
                                        { free((yyvsp[0].sval)); }
#line 1576 "parser.tab.cpp"
    break;

  case 77: 
#line 395 "parser.y"
                                        { free((yyvsp[-1].sval)); }
#line 1582 "parser.tab.cpp"
    break;

  case 78: 
#line 396 "parser.y"
                                        { free((yyvsp[-1].sval)); }
#line 1588 "parser.tab.cpp"
    break;

  case 79: 
#line 397 "parser.y"
                                                                { free((yyvsp[-3].sval)); }
#line 1594 "parser.tab.cpp"
    break;

  case 80: 
#line 398 "parser.y"
                                                                { free((yyvsp[-2].sval)); }
#line 1600 "parser.tab.cpp"
    break;

  case 85: 
#line 409 "parser.y"
                            { free((yyvsp[0].sval)); }
#line 1606 "parser.tab.cpp"
    break;

  case 86: 
#line 410 "parser.y"
                            { free((yyvsp[0].sval)); }
#line 1612 "parser.tab.cpp"
    break;


#line 1616 "parser.tab.cpp"

      default: break;
    }
  
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;



yyerrlab:
  
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      

      if (yychar <= YYEOF)
        {
          
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

  
  goto yyerrlab1;



yyerrorlab:
  
  if (0)
    YYERROR;
  ++yynerrs;

  
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;



yyerrlab1:
  yyerrstatus = 3;      

  
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


  
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;



yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;



yyabortlab:
  yyresult = 1;
  goto yyreturnlab;



yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;



yyreturnlab:
  if (yychar != YYEMPTY)
    {
      
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  
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

#line 414 "parser.y"




void yyerror(const char* msg) {
    error_count++;
    std::cerr << "Syntax Error (line " << yylineno << "): " << msg
              << " near '" << yytext << "'" << std::endl;
}



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
    } else {
        std::cerr << "Parsing finished with " << error_count << " error(s).\n";
    }

    return (result == 0 && error_count == 0) ? 0 : 1;
}
