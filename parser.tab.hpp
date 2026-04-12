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

#ifndef YY_YY_PARSER_TAB_HPP_INCLUDED
# define YY_YY_PARSER_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 107 "src/parser.y"

    #include <vector>
    struct ASTNode;

#line 54 "parser.tab.hpp"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    TOKEN_INT = 258,               /* TOKEN_INT  */
    TOKEN_FLOAT = 259,             /* TOKEN_FLOAT  */
    TOKEN_CHAR = 260,              /* TOKEN_CHAR  */
    TOKEN_VOID = 261,              /* TOKEN_VOID  */
    TOKEN_TENSOR = 262,            /* TOKEN_TENSOR  */
    TOKEN_IF = 263,                /* TOKEN_IF  */
    TOKEN_ELSE = 264,              /* TOKEN_ELSE  */
    TOKEN_WHILE = 265,             /* TOKEN_WHILE  */
    TOKEN_FOR = 266,               /* TOKEN_FOR  */
    TOKEN_RETURN = 267,            /* TOKEN_RETURN  */
    TOKEN_SWITCH = 268,            /* TOKEN_SWITCH  */
    TOKEN_CASE = 269,              /* TOKEN_CASE  */
    TOKEN_BREAK = 270,             /* TOKEN_BREAK  */
    TOKEN_DEFAULT = 271,           /* TOKEN_DEFAULT  */
    TOKEN_INT_LITERAL = 272,       /* TOKEN_INT_LITERAL  */
    TOKEN_FLOAT_LITERAL = 273,     /* TOKEN_FLOAT_LITERAL  */
    TOKEN_IDENTIFIER = 274,        /* TOKEN_IDENTIFIER  */
    TOKEN_STRING_LITERAL = 275,    /* TOKEN_STRING_LITERAL  */
    TOKEN_PLUS = 276,              /* TOKEN_PLUS  */
    TOKEN_MINUS = 277,             /* TOKEN_MINUS  */
    TOKEN_STAR = 278,              /* TOKEN_STAR  */
    TOKEN_SLASH = 279,             /* TOKEN_SLASH  */
    TOKEN_PERCENT = 280,           /* TOKEN_PERCENT  */
    TOKEN_ASSIGN = 281,            /* TOKEN_ASSIGN  */
    TOKEN_EQ = 282,                /* TOKEN_EQ  */
    TOKEN_NEQ = 283,               /* TOKEN_NEQ  */
    TOKEN_LT = 284,                /* TOKEN_LT  */
    TOKEN_GT = 285,                /* TOKEN_GT  */
    TOKEN_LEQ = 286,               /* TOKEN_LEQ  */
    TOKEN_GEQ = 287,               /* TOKEN_GEQ  */
    TOKEN_AND = 288,               /* TOKEN_AND  */
    TOKEN_OR = 289,                /* TOKEN_OR  */
    TOKEN_NOT = 290,               /* TOKEN_NOT  */
    TOKEN_INCREMENT = 291,         /* TOKEN_INCREMENT  */
    TOKEN_DECREMENT = 292,         /* TOKEN_DECREMENT  */
    TOKEN_PLUS_ASSIGN = 293,       /* TOKEN_PLUS_ASSIGN  */
    TOKEN_MINUS_ASSIGN = 294,      /* TOKEN_MINUS_ASSIGN  */
    TOKEN_STAR_ASSIGN = 295,       /* TOKEN_STAR_ASSIGN  */
    TOKEN_SLASH_ASSIGN = 296,      /* TOKEN_SLASH_ASSIGN  */
    TOKEN_LPAREN = 297,            /* TOKEN_LPAREN  */
    TOKEN_RPAREN = 298,            /* TOKEN_RPAREN  */
    TOKEN_LBRACE = 299,            /* TOKEN_LBRACE  */
    TOKEN_RBRACE = 300,            /* TOKEN_RBRACE  */
    TOKEN_LBRACKET = 301,          /* TOKEN_LBRACKET  */
    TOKEN_RBRACKET = 302,          /* TOKEN_RBRACKET  */
    TOKEN_SEMICOLON = 303,         /* TOKEN_SEMICOLON  */
    TOKEN_COMMA = 304,             /* TOKEN_COMMA  */
    TOKEN_COLON = 305,             /* TOKEN_COLON  */
    UMINUS = 306                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 112 "src/parser.y"

    int    ival;
    double fval;
    char*  sval;
    std::vector<int>* dim_list;
    ASTNode* node;

#line 130 "parser.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_HPP_INCLUDED  */
