%{
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
    struct ASTNode;
}

%union {
    int    ival;
    double fval;
    char*  sval;
    std::vector<int>* dim_list;
    ASTNode* node;
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
%type <node> program declaration_list declaration
%type <node> variable_declaration function_declaration tensor_declaration
%type <node> compound_statement statement_list statement
%type <node> expression_statement selection_statement iteration_statement
%type <node> return_statement switch_statement break_statement
%type <node> expression assignment_expression
%type <node> logical_or_expression logical_and_expression
%type <node> equality_expression relational_expression
%type <node> additive_expression multiplicative_expression
%type <node> unary_expression postfix_expression primary_expression
%type <node> argument_list parameter_list parameter
%type <node> case_list case_clause

/* Start symbol */
%start program

%%

/* ======================== Grammar Rules ======================== */

program
    : declaration_list
        {
            $$ = create_list_node(AST_PROGRAM);
            if ($1) add_child($$, $1);
            root_ast = $$;
        }
    ;

declaration_list
    : declaration_list declaration
        {
            $$ = $1;
            if ($2) add_child($$, $2);
        }
    | declaration
        {
            $$ = create_list_node(AST_DECLARATION_LIST);
            if ($1) add_child($$, $1);
        }
    ;

declaration
    : variable_declaration { $$ = $1; }
    | function_declaration { $$ = $1; }
    | tensor_declaration { $$ = $1; }
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
            $$ = create_variable_decl_node($1, $2, NULL);
            $$->line_number = yylineno;
            free($1); free($2);
        }
    | type_specifier TOKEN_IDENTIFIER TOKEN_ASSIGN expression TOKEN_SEMICOLON
        {
            sym_table.insert($2, $1, SymbolKind::VARIABLE, yylineno);
            $$ = create_variable_decl_node($1, $2, $4);
            $$->line_number = yylineno;
            free($1); free($2);
        }
    ;

/* ---------- Tensor declarations ---------- */
tensor_declaration
    : TOKEN_TENSOR TOKEN_IDENTIFIER dimension_list TOKEN_SEMICOLON
        {
            sym_table.insert_tensor($2, *$3, yylineno);
            $$ = create_tensor_decl_node($2, $3->data(), $3->size());
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
            $$ = create_function_decl_node($1, $2, $4, $6);
            free($1); free($2);
        }
    | type_specifier TOKEN_IDENTIFIER TOKEN_LPAREN TOKEN_RPAREN compound_statement
        {
            sym_table.insert($2, $1, SymbolKind::FUNCTION, yylineno);
            $$ = create_function_decl_node($1, $2, NULL, $5);
            free($1); free($2);
        }
    ;

parameter_list
    : parameter_list TOKEN_COMMA parameter
        {
            $$ = $1;
            if ($3) add_child($$, $3);
        }
    | parameter
        {
            $$ = create_list_node(AST_PARAM_LIST);
            if ($1) add_child($$, $1);
        }
    ;

parameter
    : type_specifier TOKEN_IDENTIFIER
        {
            sym_table.insert($2, $1, SymbolKind::PARAMETER, yylineno);
            $$ = create_variable_decl_node($1, $2, NULL);
            free($1); free($2);
        }
    ;

/* ---------- Statements ---------- */
compound_statement
    : TOKEN_LBRACE { sym_table.enter_scope(); }
      statement_list
      TOKEN_RBRACE 
        { 
            sym_table.exit_scope();
            $$ = create_node(AST_COMPOUND_STMT);
            $$->body = $3;
        }
    | TOKEN_LBRACE TOKEN_RBRACE
        {
            $$ = create_node(AST_COMPOUND_STMT);
            $$->body = NULL;
        }
    ;

statement_list
    : statement_list statement
        {
            $$ = $1;
            if ($2) add_child($$, $2);
        }
    | statement
        {
            $$ = create_list_node(AST_STATEMENT_LIST);
            if ($1) add_child($$, $1);
        }
    ;

statement
    : expression_statement { $$ = $1; }
    | variable_declaration { $$ = $1; }
    | compound_statement { $$ = $1; }
    | selection_statement { $$ = $1; }
    | iteration_statement { $$ = $1; }
    | return_statement { $$ = $1; }
    | switch_statement { $$ = $1; }
    | break_statement { $$ = $1; }
    ;

expression_statement
    : expression TOKEN_SEMICOLON
        {
            $$ = create_node(AST_EXPR_STMT);
            $$->left = $1;
        }
    | TOKEN_SEMICOLON
        {
            $$ = create_node(AST_EXPR_STMT);
            $$->left = NULL;
        }
    ;

/* ---------- Control flow ---------- */
selection_statement
    : TOKEN_IF TOKEN_LPAREN expression TOKEN_RPAREN statement
        {
            $$ = create_if_node($3, $5, NULL);
        }
    | TOKEN_IF TOKEN_LPAREN expression TOKEN_RPAREN statement TOKEN_ELSE statement
        {
            $$ = create_if_node($3, $5, $7);
        }
    ;

iteration_statement
    : TOKEN_WHILE TOKEN_LPAREN expression TOKEN_RPAREN statement
        {
            $$ = create_while_node($3, $5);
        }
    | TOKEN_FOR TOKEN_LPAREN expression_statement expression_statement expression TOKEN_RPAREN statement
        {
            $$ = create_for_node($3, $4, $5, $7);
        }
    | TOKEN_FOR TOKEN_LPAREN variable_declaration expression_statement expression TOKEN_RPAREN statement
        {
            $$ = create_for_node($3, $4, $5, $7);
        }
    ;

return_statement
    : TOKEN_RETURN expression TOKEN_SEMICOLON
        {
            $$ = create_return_node($2);
        }
    | TOKEN_RETURN TOKEN_SEMICOLON
        {
            $$ = create_return_node(NULL);
        }
    ;

switch_statement
    : TOKEN_SWITCH TOKEN_LPAREN expression TOKEN_RPAREN TOKEN_LBRACE case_list TOKEN_RBRACE
        {
            $$ = create_node(AST_COMPOUND_STMT);
            $$->condition = $3;
            $$->body = $6;
        }
    ;

case_list
    : case_list case_clause
        {
            $$ = $1;
            if ($2) add_child($$, $2);
        }
    | case_clause
        {
            $$ = create_list_node(AST_STATEMENT_LIST);
            if ($1) add_child($$, $1);
        }
    ;

case_clause
    : TOKEN_CASE TOKEN_INT_LITERAL TOKEN_COLON statement_list
        {
            $$ = create_node(AST_COMPOUND_STMT);
            $$->int_value = $2;
            $$->body = $4;
        }
    | TOKEN_DEFAULT TOKEN_COLON statement_list
        {
            $$ = create_node(AST_COMPOUND_STMT);
            $$->int_value = -1;
            $$->body = $3;
        }
    ;

break_statement
    : TOKEN_BREAK TOKEN_SEMICOLON
        {
            $$ = create_node(AST_EXPR_STMT);
        }
    ;

/* ---------- Expressions ---------- */
expression
    : assignment_expression { $$ = $1; }
    ;

assignment_expression
    : TOKEN_IDENTIFIER TOKEN_ASSIGN expression
        { 
            $$ = create_assignment_node(create_identifier_node($1), $3);
            $$->line_number = yylineno;
            free($1);
        }
    | TOKEN_IDENTIFIER TOKEN_PLUS_ASSIGN expression
        { 
            ASTNode *id = create_identifier_node($1);
            ASTNode *add = create_binary_node(AST_BINARY_OP, OP_ADD, id, $3);
            $$ = create_assignment_node(id, add);
            free($1);
        }
    | TOKEN_IDENTIFIER TOKEN_MINUS_ASSIGN expression
        { 
            ASTNode *id = create_identifier_node($1);
            ASTNode *sub = create_binary_node(AST_BINARY_OP, OP_SUB, id, $3);
            $$ = create_assignment_node(id, sub);
            free($1);
        }
    | TOKEN_IDENTIFIER TOKEN_STAR_ASSIGN expression
        { 
            ASTNode *id = create_identifier_node($1);
            ASTNode *mul = create_binary_node(AST_BINARY_OP, OP_MUL, id, $3);
            $$ = create_assignment_node(id, mul);
            free($1);
        }
    | TOKEN_IDENTIFIER TOKEN_SLASH_ASSIGN expression
        { 
            ASTNode *id = create_identifier_node($1);
            ASTNode *div = create_binary_node(AST_BINARY_OP, OP_DIV, id, $3);
            $$ = create_assignment_node(id, div);
            free($1);
        }
    | logical_or_expression { $$ = $1; }
    ;

logical_or_expression
    : logical_or_expression TOKEN_OR logical_and_expression
        {
            $$ = create_binary_node(AST_BINARY_OP, OP_OR, $1, $3);
        }
    | logical_and_expression { $$ = $1; }
    ;

logical_and_expression
    : logical_and_expression TOKEN_AND equality_expression
        {
            $$ = create_binary_node(AST_BINARY_OP, OP_AND, $1, $3);
        }
    | equality_expression { $$ = $1; }
    ;

equality_expression
    : equality_expression TOKEN_EQ relational_expression
        {
            $$ = create_binary_node(AST_BINARY_OP, OP_EQ, $1, $3);
        }
    | equality_expression TOKEN_NEQ relational_expression
        {
            $$ = create_binary_node(AST_BINARY_OP, OP_NEQ, $1, $3);
        }
    | relational_expression { $$ = $1; }
    ;

relational_expression
    : relational_expression TOKEN_LT additive_expression
        {
            $$ = create_binary_node(AST_BINARY_OP, OP_LT, $1, $3);
        }
    | relational_expression TOKEN_GT additive_expression
        {
            $$ = create_binary_node(AST_BINARY_OP, OP_GT, $1, $3);
        }
    | relational_expression TOKEN_LEQ additive_expression
        {
            $$ = create_binary_node(AST_BINARY_OP, OP_LEQ, $1, $3);
        }
    | relational_expression TOKEN_GEQ additive_expression
        {
            $$ = create_binary_node(AST_BINARY_OP, OP_GEQ, $1, $3);
        }
    | additive_expression { $$ = $1; }
    ;

additive_expression
    : additive_expression TOKEN_PLUS multiplicative_expression
        {
            $$ = create_binary_node(AST_BINARY_OP, OP_ADD, $1, $3);
        }
    | additive_expression TOKEN_MINUS multiplicative_expression
        {
            $$ = create_binary_node(AST_BINARY_OP, OP_SUB, $1, $3);
        }
    | multiplicative_expression { $$ = $1; }
    ;

multiplicative_expression
    : multiplicative_expression TOKEN_STAR unary_expression
        {
            $$ = create_binary_node(AST_BINARY_OP, OP_MUL, $1, $3);
        }
    | multiplicative_expression TOKEN_SLASH unary_expression
        {
            $$ = create_binary_node(AST_BINARY_OP, OP_DIV, $1, $3);
        }
    | multiplicative_expression TOKEN_PERCENT unary_expression
        {
            $$ = create_binary_node(AST_BINARY_OP, OP_MOD, $1, $3);
        }
    | unary_expression { $$ = $1; }
    ;

unary_expression
    : TOKEN_MINUS unary_expression %prec UMINUS
        {
            $$ = create_unary_node(AST_UNARY_OP, OP_NEG, $2);
        }
    | TOKEN_NOT unary_expression
        {
            $$ = create_unary_node(AST_UNARY_OP, OP_NOT, $2);
        }
    | TOKEN_INCREMENT TOKEN_IDENTIFIER
        {
            $$ = create_identifier_node($2);
            free($2);
        }
    | TOKEN_DECREMENT TOKEN_IDENTIFIER
        {
            $$ = create_identifier_node($2);
            free($2);
        }
    | postfix_expression { $$ = $1; }
    ;

postfix_expression
    : primary_expression { $$ = $1; }
    | TOKEN_IDENTIFIER TOKEN_INCREMENT
        {
            $$ = create_identifier_node($1);
            free($1);
        }
    | TOKEN_IDENTIFIER TOKEN_DECREMENT
        {
            $$ = create_identifier_node($1);
            free($1);
        }
    | TOKEN_IDENTIFIER TOKEN_LPAREN argument_list TOKEN_RPAREN
        {
            $$ = create_function_call_node($1, $3);
            free($1);
        }
    | TOKEN_IDENTIFIER TOKEN_LPAREN TOKEN_RPAREN
        {
            $$ = create_function_call_node($1, NULL);
            free($1);
        }
    ;

argument_list
    : argument_list TOKEN_COMMA expression
        {
            $$ = $1;
            if ($3) add_child($$, $3);
        }
    | expression
        {
            $$ = create_list_node(AST_ARG_LIST);
            if ($1) add_child($$, $1);
        }
    ;

primary_expression
    : TOKEN_INT_LITERAL
        {
            $$ = create_number_node($1);
        }
    | TOKEN_FLOAT_LITERAL
        {
            $$ = create_float_node($1);
        }
    | TOKEN_STRING_LITERAL
        {
            $$ = create_string_node($1);
            free($1);
        }
    | TOKEN_IDENTIFIER
        {
            $$ = create_identifier_node($1);
            $$->line_number = yylineno;
            free($1);
        }
    | TOKEN_LPAREN expression TOKEN_RPAREN
        {
            $$ = $2;
        }
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
            
            IRList* ir = generate_ir_from_ast(root_ast, &sym_table);
            if (ir) {
                int opt_changes = optimize_ir(ir);
                if (opt_changes > 0) {
                    std::cout << "Optimizations applied: " << opt_changes << " changes\n";
                }
                
                if (argc >= 4) {
                    FILE* asm_file = fopen(argv[3], "w");
                    if (asm_file) {
                        generate_assembly(ir, asm_file);
                        fclose(asm_file);
                        std::cout << "Assembly code written to '" << argv[3] << "'\n";
                    }
                }
                free_ir_list(ir);
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
