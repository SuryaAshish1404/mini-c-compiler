#ifndef TENSOR_IR_H
#define TENSOR_IR_H

#include "ast.h"
#include "ir.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration to avoid C++ dependency
#ifdef __cplusplus
#include "symbol_table.h"
#else
typedef struct SymbolTable SymbolTable;
typedef struct SymbolEntry SymbolEntry;
#endif

// Generate IR for tensor operations
void generate_tensor_ir(IRList *ir_list, ASTNode *node, void *sym_table);

// Tensor info structure (C-compatible)
typedef struct {
    int is_tensor;
    int num_dimensions;
    int shape[10];
} TensorInfo;

// Generate IR for tensor element-wise operations
void generate_tensor_add_ir(IRList *ir_list, const char *dest, const char *lhs, 
                            const char *rhs, TensorInfo *tensor_info);
void generate_tensor_sub_ir(IRList *ir_list, const char *dest, const char *lhs, 
                            const char *rhs, TensorInfo *tensor_info);
void generate_tensor_mul_ir(IRList *ir_list, const char *dest, const char *lhs, 
                            const char *rhs, TensorInfo *tensor_info);

// Helper to generate nested loops for tensor operations
void generate_tensor_loops(IRList *ir_list, TensorInfo *tensor_info, 
                          const char *dest, const char *lhs, const char *rhs, 
                          const char *op);

#ifdef __cplusplus
}
#endif

#endif // TENSOR_IR_H
