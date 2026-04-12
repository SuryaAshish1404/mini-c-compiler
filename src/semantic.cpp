#include "semantic.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

SemanticResult* create_semantic_result() {
    SemanticResult* result = (SemanticResult*)malloc(sizeof(SemanticResult));
    result->error_count = 0;
    result->warning_count = 0;
    return result;
}

int types_compatible(const char* type1, const char* type2) {
    if (!type1 || !type2) return 0;
    if (strcmp(type1, type2) == 0) return 1;
    if ((strcmp(type1, "int") == 0 && strcmp(type2, "float") == 0) ||
        (strcmp(type1, "float") == 0 && strcmp(type2, "int") == 0)) {
        return 1;
    }
    return 0;
}

const char* get_expr_type(ASTNode* node, SymbolTable* sym_table, SemanticResult* result) {
    if (!node) return "void";
    
    switch (node->type) {
        case AST_NUMBER:
            return "int";
        case AST_FLOAT_NUMBER:
            return "float";
        case AST_STRING:
            return "char*";
        case AST_IDENTIFIER: {
            SymbolEntry* entry = sym_table->lookup(node->name);
            if (!entry) {
                return "int";
            }
            return entry->type.c_str();
        }
        case AST_BINARY_OP: {
            const char* left_type = get_expr_type(node->left, sym_table, result);
            const char* right_type = get_expr_type(node->right, sym_table, result);
            if (strcmp(left_type, "float") == 0 || strcmp(right_type, "float") == 0) {
                return "float";
            }
            return "int";
        }
        case AST_UNARY_OP:
            return get_expr_type(node->left, sym_table, result);
        case AST_ASSIGNMENT:
            return get_expr_type(node->left, sym_table, result);
        case AST_FUNCTION_CALL: {
            SymbolEntry* entry = sym_table->lookup(node->name);
            if (!entry) {
                return "int";
            }
            return entry->type.c_str();
        }
        default:
            return "int";
    }
}

void check_assignment(ASTNode* node, SymbolTable* sym_table, SemanticResult* result) {
    if (!node || !node->left || !node->right) return;
    
    const char* lhs_type = get_expr_type(node->left, sym_table, result);
    const char* rhs_type = get_expr_type(node->right, sym_table, result);
    
    if (!types_compatible(lhs_type, rhs_type)) {
        fprintf(stderr, "Semantic Warning (line %d): type mismatch in assignment: '%s' = '%s'\n",
                node->line_number, lhs_type, rhs_type);
        result->warning_count++;
    }
}

void check_binary_op(ASTNode* node, SymbolTable* sym_table, SemanticResult* result) {
    if (!node || !node->left || !node->right) return;
    
    const char* left_type = get_expr_type(node->left, sym_table, result);
    const char* right_type = get_expr_type(node->right, sym_table, result);
    
    if (node->op == OP_MOD) {
        if (strcmp(left_type, "float") == 0 || strcmp(right_type, "float") == 0) {
            fprintf(stderr, "Semantic Error (line %d): modulo operation requires integer operands\n",
                    node->line_number);
            result->error_count++;
        }
    }
}

void check_function_call(ASTNode* node, SymbolTable* sym_table, SemanticResult* result) {
    if (!node) return;
    
    SymbolEntry* entry = sym_table->lookup(node->name);
    if (entry && entry->kind != SymbolKind::FUNCTION) {
        fprintf(stderr, "Semantic Error (line %d): '%s' is not a function\n",
                node->line_number, node->name);
        result->error_count++;
    }
}

void check_types(ASTNode* node, SymbolTable* sym_table, SemanticResult* result) {
    if (!node) return;
    
    switch (node->type) {
        case AST_ASSIGNMENT:
            check_assignment(node, sym_table, result);
            check_types(node->left, sym_table, result);
            check_types(node->right, sym_table, result);
            break;
        case AST_BINARY_OP:
            check_binary_op(node, sym_table, result);
            check_types(node->left, sym_table, result);
            check_types(node->right, sym_table, result);
            break;
        case AST_UNARY_OP:
            check_types(node->left, sym_table, result);
            break;
        case AST_FUNCTION_CALL:
            check_function_call(node, sym_table, result);
            if (node->left) check_types(node->left, sym_table, result);
            break;
        case AST_IF_STMT:
            check_types(node->condition, sym_table, result);
            check_types(node->then_branch, sym_table, result);
            if (node->else_branch) check_types(node->else_branch, sym_table, result);
            break;
        case AST_WHILE_STMT:
            check_types(node->condition, sym_table, result);
            check_types(node->body, sym_table, result);
            break;
        case AST_FOR_STMT:
            if (node->init) check_types(node->init, sym_table, result);
            if (node->condition) check_types(node->condition, sym_table, result);
            if (node->update) check_types(node->update, sym_table, result);
            check_types(node->body, sym_table, result);
            break;
        case AST_RETURN_STMT:
            if (node->left) check_types(node->left, sym_table, result);
            break;
        case AST_COMPOUND_STMT:
            if (node->body) check_types(node->body, sym_table, result);
            break;
        case AST_EXPR_STMT:
            if (node->left) check_types(node->left, sym_table, result);
            break;
        case AST_FUNCTION_DECL:
            if (node->body) check_types(node->body, sym_table, result);
            break;
        case AST_VARIABLE_DECL:
            if (node->right) check_types(node->right, sym_table, result);
            break;
        case AST_PROGRAM:
        case AST_DECLARATION_LIST:
        case AST_STATEMENT_LIST:
        case AST_ARG_LIST:
        case AST_PARAM_LIST:
            for (int i = 0; i < node->num_children; i++) {
                check_types(node->children[i], sym_table, result);
            }
            break;
        default:
            break;
    }
}

void semantic_analyze(ASTNode* ast, SymbolTable* sym_table, SemanticResult* result) {
    if (!ast || !sym_table || !result) return;
    check_types(ast, sym_table, result);
}
