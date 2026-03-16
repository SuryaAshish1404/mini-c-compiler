%{
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
    
    // generate nested loops
    for (int i = 0; i < tensor->num_dimensions; i++) {
        for (int j = 0; j < i; j++) fprintf(output_file, "    ");
        fprintf(output_file, "for(int %s=0; %s<%d; %s++) {\n", 
                loop_vars[i].c_str(), loop_vars[i].c_str(), 
                tensor->shape[i], loop_vars[i].c_str());
    }
    
    // generate array access
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
    
    // close loops
    for (int i = tensor->num_dimensions - 1; i >= 0; i--) {
        for (int j = 0; j < i; j++) fprintf(output_file, "    ");
        fprintf(output_file, "}\n");
    }
}
%}

%code requires {
    #include <vector>
}

%union {
    int    ival;
    double fval;
    char*  sval;
    std::vector<int>* dim_list;
}

/* ---------- Token declarations ---------- */

/* Keywords */
%token TOKEN_INT TOKEN_FLOAT TOKEN_CHAR TOKEN_VOID TOKEN_TENSOR
%token TOKEN_IF TOKEN_ELSE TOKEN_WHILE TOKEN_FOR TOKEN_RETURN
%token TOKEN_SWITCH TOKEN_CASE TOKEN_BREAK TOKEN_DEFAULT

/* Literals */
%token <ival> TOKEN_INT_LITERAL
%token <fval> TOKEN_FLOAT_LITERAL
%token <sval> TOKEN_IDENTIFIER TOKEN_STRING_LITERAL

/* Operators */
%token TOKEN_PLUS TOKEN_MINUS TOKEN_STAR TOKEN_SLASH TOKEN_PERCENT
%token TOKEN_ASSIGN
%token TOKEN_EQ TOKEN_NEQ TOKEN_LT TOKEN_GT TOKEN_LEQ TOKEN_GEQ
%token TOKEN_AND TOKEN_OR TOKEN_NOT
%token TOKEN_INCREMENT TOKEN_DECREMENT
%token TOKEN_PLUS_ASSIGN TOKEN_MINUS_ASSIGN
%token TOKEN_STAR_ASSIGN TOKEN_SLASH_ASSIGN

/* Delimiters */
%token TOKEN_LPAREN TOKEN_RPAREN
%token TOKEN_LBRACE TOKEN_RBRACE
%token TOKEN_LBRACKET TOKEN_RBRACKET
%token TOKEN_SEMICOLON TOKEN_COMMA TOKEN_COLON

/* ---------- Operator precedence & associativity (low to high) ---------- */
%right TOKEN_ASSIGN TOKEN_PLUS_ASSIGN TOKEN_MINUS_ASSIGN TOKEN_STAR_ASSIGN TOKEN_SLASH_ASSIGN
%left  TOKEN_OR
%left  TOKEN_AND
%left  TOKEN_EQ TOKEN_NEQ
%left  TOKEN_LT TOKEN_GT TOKEN_LEQ TOKEN_GEQ
%left  TOKEN_PLUS TOKEN_MINUS
%left  TOKEN_STAR TOKEN_SLASH TOKEN_PERCENT
%right TOKEN_NOT UMINUS
%left  TOKEN_INCREMENT TOKEN_DECREMENT

/* ---------- Non-terminal types ---------- */
%type <sval> type_specifier
%type <dim_list> dimension_list

/* Start symbol */
%start program

%%

/* ======================== Grammar Rules ======================== */

program
    : declaration_list
    ;

declaration_list
    : declaration_list declaration
    | declaration
    ;

declaration
    : variable_declaration
    | function_declaration
    | tensor_declaration
    ;

/* ---------- Type specifiers ---------- */
type_specifier
    : TOKEN_INT   { $$ = strdup("int");   }
    | TOKEN_FLOAT { $$ = strdup("float"); }
    | TOKEN_CHAR  { $$ = strdup("char");  }
    | TOKEN_VOID  { $$ = strdup("void");  }
    ;

/* ---------- Variable declarations ---------- */
variable_declaration
    : type_specifier TOKEN_IDENTIFIER TOKEN_SEMICOLON
        {
            sym_table.insert($2, $1, SymbolKind::VARIABLE, yylineno);
            free($1); free($2);
        }
    | type_specifier TOKEN_IDENTIFIER TOKEN_ASSIGN expression TOKEN_SEMICOLON
        {
            sym_table.insert($2, $1, SymbolKind::VARIABLE, yylineno);
            free($1); free($2);
        }
    ;

/* ---------- Tensor declarations ---------- */
tensor_declaration
    : TOKEN_TENSOR TOKEN_IDENTIFIER dimension_list TOKEN_SEMICOLON
        {
            sym_table.insert_tensor($2, *$3, yylineno);
            free($2);
            delete $3;
        }
    ;

dimension_list
    : dimension_list TOKEN_LBRACKET TOKEN_INT_LITERAL TOKEN_RBRACKET
        {
            $$ = $1;
            $$->push_back($3);
        }
    | TOKEN_LBRACKET TOKEN_INT_LITERAL TOKEN_RBRACKET
        {
            $$ = new std::vector<int>();
            $$->push_back($2);
        }
    ;

/* ---------- Function declarations ---------- */
function_declaration
    : type_specifier TOKEN_IDENTIFIER TOKEN_LPAREN parameter_list TOKEN_RPAREN compound_statement
        {
            sym_table.insert($2, $1, SymbolKind::FUNCTION, yylineno);
            free($1); free($2);
        }
    | type_specifier TOKEN_IDENTIFIER TOKEN_LPAREN TOKEN_RPAREN compound_statement
        {
            sym_table.insert($2, $1, SymbolKind::FUNCTION, yylineno);
            free($1); free($2);
        }
    ;

parameter_list
    : parameter_list TOKEN_COMMA parameter
    | parameter
    ;

parameter
    : type_specifier TOKEN_IDENTIFIER
        {
            sym_table.insert($2, $1, SymbolKind::PARAMETER, yylineno);
            free($1); free($2);
        }
    ;

/* ---------- Statements ---------- */
compound_statement
    : TOKEN_LBRACE { sym_table.enter_scope(); }
      statement_list
      TOKEN_RBRACE { sym_table.exit_scope(); }
    | TOKEN_LBRACE TOKEN_RBRACE
    ;

statement_list
    : statement_list statement
    | statement
    ;

statement
    : expression_statement
    | variable_declaration
    | compound_statement
    | selection_statement
    | iteration_statement
    | return_statement
    | switch_statement
    | break_statement
    ;

expression_statement
    : expression TOKEN_SEMICOLON
    | TOKEN_SEMICOLON
    ;

/* ---------- Control flow ---------- */
selection_statement
    : TOKEN_IF TOKEN_LPAREN expression TOKEN_RPAREN statement
    | TOKEN_IF TOKEN_LPAREN expression TOKEN_RPAREN statement TOKEN_ELSE statement
    ;

iteration_statement
    : TOKEN_WHILE TOKEN_LPAREN expression TOKEN_RPAREN statement
    | TOKEN_FOR TOKEN_LPAREN expression_statement expression_statement expression TOKEN_RPAREN statement
    | TOKEN_FOR TOKEN_LPAREN variable_declaration expression_statement expression TOKEN_RPAREN statement
    ;

return_statement
    : TOKEN_RETURN expression TOKEN_SEMICOLON
    | TOKEN_RETURN TOKEN_SEMICOLON
    ;

switch_statement
    : TOKEN_SWITCH TOKEN_LPAREN expression TOKEN_RPAREN TOKEN_LBRACE case_list TOKEN_RBRACE
    ;

case_list
    : case_list case_clause
    | case_clause
    ;

case_clause
    : TOKEN_CASE TOKEN_INT_LITERAL TOKEN_COLON statement_list
    | TOKEN_DEFAULT TOKEN_COLON statement_list
    ;

break_statement
    : TOKEN_BREAK TOKEN_SEMICOLON
    ;

/* ---------- Expressions ---------- */
expression
    : assignment_expression
    ;

assignment_expression
    : TOKEN_IDENTIFIER TOKEN_ASSIGN expression
        { free($1); }
    | TOKEN_IDENTIFIER TOKEN_PLUS_ASSIGN expression
        { free($1); }
    | TOKEN_IDENTIFIER TOKEN_MINUS_ASSIGN expression
        { free($1); }
    | TOKEN_IDENTIFIER TOKEN_STAR_ASSIGN expression
        { free($1); }
    | TOKEN_IDENTIFIER TOKEN_SLASH_ASSIGN expression
        { free($1); }
    | logical_or_expression
    ;

logical_or_expression
    : logical_or_expression TOKEN_OR logical_and_expression
    | logical_and_expression
    ;

logical_and_expression
    : logical_and_expression TOKEN_AND equality_expression
    | equality_expression
    ;

equality_expression
    : equality_expression TOKEN_EQ relational_expression
    | equality_expression TOKEN_NEQ relational_expression
    | relational_expression
    ;

relational_expression
    : relational_expression TOKEN_LT additive_expression
    | relational_expression TOKEN_GT additive_expression
    | relational_expression TOKEN_LEQ additive_expression
    | relational_expression TOKEN_GEQ additive_expression
    | additive_expression
    ;

additive_expression
    : additive_expression TOKEN_PLUS multiplicative_expression
    | additive_expression TOKEN_MINUS multiplicative_expression
    | multiplicative_expression
    ;

multiplicative_expression
    : multiplicative_expression TOKEN_STAR unary_expression
    | multiplicative_expression TOKEN_SLASH unary_expression
    | multiplicative_expression TOKEN_PERCENT unary_expression
    | unary_expression
    ;

unary_expression
    : TOKEN_MINUS unary_expression %prec UMINUS
    | TOKEN_NOT unary_expression
    | TOKEN_INCREMENT TOKEN_IDENTIFIER  { free($2); }
    | TOKEN_DECREMENT TOKEN_IDENTIFIER  { free($2); }
    | postfix_expression
    ;

postfix_expression
    : primary_expression
    | TOKEN_IDENTIFIER TOKEN_INCREMENT  { free($1); }
    | TOKEN_IDENTIFIER TOKEN_DECREMENT  { free($1); }
    | TOKEN_IDENTIFIER TOKEN_LPAREN argument_list TOKEN_RPAREN  { free($1); }
    | TOKEN_IDENTIFIER TOKEN_LPAREN TOKEN_RPAREN                { free($1); }
    ;

argument_list
    : argument_list TOKEN_COMMA expression
    | expression
    ;

primary_expression
    : TOKEN_INT_LITERAL
    | TOKEN_FLOAT_LITERAL
    | TOKEN_STRING_LITERAL  { free($1); }
    | TOKEN_IDENTIFIER      { free($1); }
    | TOKEN_LPAREN expression TOKEN_RPAREN
    ;

%%

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

    // Open output file if specified
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
