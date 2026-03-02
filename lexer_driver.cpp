#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include "parser.tab.hpp"

extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;
YYSTYPE yylval;

std::string token_to_string(int token) {
    switch (token) {
        case TOKEN_INT: return "TOKEN_INT";
        case TOKEN_FLOAT: return "TOKEN_FLOAT";
        case TOKEN_CHAR: return "TOKEN_CHAR";
        case TOKEN_VOID: return "TOKEN_VOID";
        case TOKEN_TENSOR: return "TOKEN_TENSOR";
        case TOKEN_IF: return "TOKEN_IF";
        case TOKEN_ELSE: return "TOKEN_ELSE";
        case TOKEN_WHILE: return "TOKEN_WHILE";
        case TOKEN_FOR: return "TOKEN_FOR";
        case TOKEN_RETURN: return "TOKEN_RETURN";

        case TOKEN_INT_LITERAL: return "TOKEN_INT_LITERAL";
        case TOKEN_FLOAT_LITERAL: return "TOKEN_FLOAT_LITERAL";
        case TOKEN_IDENTIFIER: return "TOKEN_IDENTIFIER";
        case TOKEN_STRING_LITERAL: return "TOKEN_STRING_LITERAL";

        case TOKEN_PLUS: return "TOKEN_PLUS";
        case TOKEN_MINUS: return "TOKEN_MINUS";
        case TOKEN_STAR: return "TOKEN_STAR";
        case TOKEN_SLASH: return "TOKEN_SLASH";
        case TOKEN_PERCENT: return "TOKEN_PERCENT";
        case TOKEN_ASSIGN: return "TOKEN_ASSIGN";
        case TOKEN_EQ: return "TOKEN_EQ";
        case TOKEN_NEQ: return "TOKEN_NEQ";
        case TOKEN_LT: return "TOKEN_LT";
        case TOKEN_GT: return "TOKEN_GT";
        case TOKEN_LEQ: return "TOKEN_LEQ";
        case TOKEN_GEQ: return "TOKEN_GEQ";
        case TOKEN_AND: return "TOKEN_AND";
        case TOKEN_OR: return "TOKEN_OR";
        case TOKEN_NOT: return "TOKEN_NOT";
        case TOKEN_INCREMENT: return "TOKEN_INCREMENT";
        case TOKEN_DECREMENT: return "TOKEN_DECREMENT";
        case TOKEN_PLUS_ASSIGN: return "TOKEN_PLUS_ASSIGN";
        case TOKEN_MINUS_ASSIGN: return "TOKEN_MINUS_ASSIGN";
        case TOKEN_STAR_ASSIGN: return "TOKEN_STAR_ASSIGN";
        case TOKEN_SLASH_ASSIGN: return "TOKEN_SLASH_ASSIGN";

        case TOKEN_LPAREN: return "TOKEN_LPAREN";
        case TOKEN_RPAREN: return "TOKEN_RPAREN";
        case TOKEN_LBRACE: return "TOKEN_LBRACE";
        case TOKEN_RBRACE: return "TOKEN_RBRACE";
        case TOKEN_LBRACKET: return "TOKEN_LBRACKET";
        case TOKEN_RBRACKET: return "TOKEN_RBRACKET";
        case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
        case TOKEN_COMMA: return "TOKEN_COMMA";

        default:
            if (token >= 0 && token < 128 && std::isprint(token)) {
                return std::string("CHAR(") + static_cast<char>(token) + ")";
            }
            return "UNRECOGNIZED_TOKEN";
    }
}

void pretty_print(int token) {
    std::string lexeme = yytext ? std::string(yytext) : std::string();
    std::cout << std::setw(5) << yylineno << " | "
              << std::setw(22) << token_to_string(token) << " | ";

    switch (token) {
        case TOKEN_INT_LITERAL:
            std::cout << lexeme << " (value=" << yylval.ival << ")";
            break;
        case TOKEN_FLOAT_LITERAL:
            std::cout << lexeme << " (value=" << yylval.fval << ")";
            break;
        case TOKEN_IDENTIFIER:
        case TOKEN_STRING_LITERAL:
            std::cout << lexeme;
            free(yylval.sval);
            break;
        default:
            std::cout << lexeme;
            break;
    }
    std::cout << '\n';
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file.c>" << std::endl;
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        std::cerr << "Error: cannot open file '" << argv[1] << "'" << std::endl;
        return 1;
    }

    yyin = file;
    std::cout << "Tokenizing '" << argv[1] << "'..." << std::endl;
    std::cout << std::setw(5) << "Line" << " | "
              << std::setw(22) << "Token"
              << " | Lexeme" << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    int token;
    while ((token = yylex()) != 0) {
        pretty_print(token);
    }

    fclose(file);
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "Lexical analysis completed." << std::endl;
    return 0;
}
