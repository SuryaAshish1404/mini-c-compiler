#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "symbol_table.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int error_count;
    int warning_count;
} SemanticResult;

SemanticResult* create_semantic_result();
void semantic_analyze(ASTNode* ast, SymbolTable* sym_table, SemanticResult* result);
void check_types(ASTNode* node, SymbolTable* sym_table, SemanticResult* result);
void check_tensor_op(ASTNode* node, SymbolTable* sym_table, SemanticResult* result);
const char* get_expr_type(ASTNode* node, SymbolTable* sym_table, SemanticResult* result);
int types_compatible(const char* type1, const char* type2);

#ifdef __cplusplus
}
#endif

#endif
