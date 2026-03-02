%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include "parser.tab.hpp"

void report_lexical_error(const char* text, int line);
%}

%option noyywrap
%option yylineno

/* ======================== Definitions ======================== */

DIGIT       [0-9]
LETTER      [a-zA-Z_]
ID          {LETTER}({LETTER}|{DIGIT})*
INT_LIT     {DIGIT}+
FLOAT_LIT   {DIGIT}+"."{DIGIT}*|"."{DIGIT}+
STRING_LIT  \"([^"\\]|\\.)*\"
WHITESPACE  [ \t\r]+
NEWLINE     \n

%%

 /* ======================== Rules ======================== */

 /* ---------- Comments ---------- */
"//".*                  { /* single-line comment: skip */ }
"/*"([^*]|\*+[^*/])*\*+"/"  { /* multi-line comment: skip */ }

 /* ---------- Keywords ---------- */
"int"       { return TOKEN_INT;    }
"float"     { return TOKEN_FLOAT;  }
"char"      { return TOKEN_CHAR;   }
"void"      { return TOKEN_VOID;   }
"tensor"    { return TOKEN_TENSOR; }
"if"        { return TOKEN_IF;     }
"else"      { return TOKEN_ELSE;   }
"while"     { return TOKEN_WHILE;  }
"for"       { return TOKEN_FOR;    }
"return"    { return TOKEN_RETURN; }

 /* ---------- Operators (multi-char before single-char) ---------- */
"++"        { return TOKEN_INCREMENT;    }
"--"        { return TOKEN_DECREMENT;    }
"+="        { return TOKEN_PLUS_ASSIGN;  }
"-="        { return TOKEN_MINUS_ASSIGN; }
"*="        { return TOKEN_STAR_ASSIGN;  }
"/="        { return TOKEN_SLASH_ASSIGN; }
"=="        { return TOKEN_EQ;           }
"!="        { return TOKEN_NEQ;          }
"<="        { return TOKEN_LEQ;          }
">="        { return TOKEN_GEQ;          }
"&&"        { return TOKEN_AND;          }
"||"        { return TOKEN_OR;           }
"+"         { return TOKEN_PLUS;         }
"-"         { return TOKEN_MINUS;        }
"*"         { return TOKEN_STAR;         }
"/"         { return TOKEN_SLASH;        }
"%"         { return TOKEN_PERCENT;      }
"="         { return TOKEN_ASSIGN;       }
"<"         { return TOKEN_LT;           }
">"         { return TOKEN_GT;           }
"!"         { return TOKEN_NOT;          }

 /* ---------- Delimiters ---------- */
"("         { return TOKEN_LPAREN;    }
")"         { return TOKEN_RPAREN;    }
"{"         { return TOKEN_LBRACE;    }
"}"         { return TOKEN_RBRACE;    }
"["         { return TOKEN_LBRACKET;  }
"]"         { return TOKEN_RBRACKET;  }
";"         { return TOKEN_SEMICOLON; }
","         { return TOKEN_COMMA;     }

 /* ---------- Literals ---------- */
{FLOAT_LIT} {
    yylval.fval = atof(yytext);
    return TOKEN_FLOAT_LITERAL;
}

{INT_LIT} {
    yylval.ival = atoi(yytext);
    return TOKEN_INT_LITERAL;
}

{STRING_LIT} {
    yylval.sval = strdup(yytext);
    return TOKEN_STRING_LITERAL;
}

 /* ---------- Identifiers ---------- */
{ID} {
    yylval.sval = strdup(yytext);
    return TOKEN_IDENTIFIER;
}

 /* ---------- Whitespace ---------- */
{WHITESPACE}  { /* skip */ }
{NEWLINE}     { /* skip; yylineno updated automatically */ }

 /* ---------- Catch-all for unrecognized characters ---------- */
. {
    report_lexical_error(yytext, yylineno);
}

%%

/* ======================== User Code ======================== */

void report_lexical_error(const char* text, int line) {
    std::cerr << "Lexical Error (line " << line
              << "): unrecognized character '" << text << "'" << std::endl;
}
