#ifndef IR_GEN_H
#define IR_GEN_H

#include "ast.h"
#include "ir.h"

#ifdef __cplusplus
#include "symbol_table.h"

extern "C" {
#endif


#ifdef __cplusplus
IRList* generate_ir_from_ast(ASTNode *ast, SymbolTable *sym_table);


char* generate_expr_ir(IRList *ir_list, ASTNode *node, SymbolTable *sym_table);
void generate_stmt_ir(IRList *ir_list, ASTNode *node, SymbolTable *sym_table);
void generate_decl_ir(IRList *ir_list, ASTNode *node, SymbolTable *sym_table);
#endif

#ifdef __cplusplus
}
#endif

#endif 
