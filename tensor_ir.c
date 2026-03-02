#include "tensor_ir.h"
#include "temp_var.h"
#include <string.h>

// Generate nested loops for tensor operations
void generate_tensor_loops(IRList *ir_list, TensorInfo *tensor_info, 
                          const char *dest, const char *lhs, const char *rhs, 
                          const char *op) {
    if (!tensor_info || !tensor_info->is_tensor) return;
    
    char loop_vars[10][32];
    char start[32], end[32];
    
    // Generate loop variables
    for (int i = 0; i < tensor_info->num_dimensions; i++) {
        get_loop_var(loop_vars[i], sizeof(loop_vars[i]), i);
    }
    
    // Generate FOR_BEGIN instructions for each dimension
    for (int i = 0; i < tensor_info->num_dimensions; i++) {
        sprintf(start, "0");
        sprintf(end, "%d", tensor_info->shape[i]);
        append_ir(ir_list, create_ir_for_begin(loop_vars[i], start, end));
    }
    
    // Build index string for array access
    char lhs_index[256], rhs_index[256], dest_index[256];
    lhs_index[0] = rhs_index[0] = dest_index[0] = '\0';
    
    for (int i = 0; i < tensor_info->num_dimensions; i++) {
        if (i > 0) {
            strcat(lhs_index, "][");
            strcat(rhs_index, "][");
            strcat(dest_index, "][");
        }
        strcat(lhs_index, loop_vars[i]);
        strcat(rhs_index, loop_vars[i]);
        strcat(dest_index, loop_vars[i]);
    }
    
    // Generate the operation
    char temp1[32], temp2[32], temp3[32];
    get_temp_var(temp1, sizeof(temp1));
    get_temp_var(temp2, sizeof(temp2));
    get_temp_var(temp3, sizeof(temp3));
    
    // LOAD lhs[indices]
    append_ir(ir_list, create_ir_load(temp1, lhs, lhs_index));
    
    // LOAD rhs[indices]
    append_ir(ir_list, create_ir_load(temp2, rhs, rhs_index));
    
    // Perform operation
    IROpcode opcode;
    if (strcmp(op, "+") == 0) opcode = IR_ADD;
    else if (strcmp(op, "-") == 0) opcode = IR_SUB;
    else if (strcmp(op, "*") == 0) opcode = IR_MUL;
    else opcode = IR_ADD;
    
    append_ir(ir_list, create_ir_binary(opcode, temp3, temp1, temp2));
    
    // STORE result to dest[indices]
    append_ir(ir_list, create_ir_store(dest, dest_index, temp3));
    
    // Generate FOR_END instructions
    for (int i = 0; i < tensor_info->num_dimensions; i++) {
        append_ir(ir_list, create_ir_for_end());
    }
}

// Generate IR for tensor addition
void generate_tensor_add_ir(IRList *ir_list, const char *dest, const char *lhs, 
                            const char *rhs, TensorInfo *tensor_info) {
    generate_tensor_loops(ir_list, tensor_info, dest, lhs, rhs, "+");
}

// Generate IR for tensor subtraction
void generate_tensor_sub_ir(IRList *ir_list, const char *dest, const char *lhs, 
                            const char *rhs, TensorInfo *tensor_info) {
    generate_tensor_loops(ir_list, tensor_info, dest, lhs, rhs, "-");
}

// Generate IR for tensor multiplication
void generate_tensor_mul_ir(IRList *ir_list, const char *dest, const char *lhs, 
                            const char *rhs, TensorInfo *tensor_info) {
    generate_tensor_loops(ir_list, tensor_info, dest, lhs, rhs, "*");
}

// Main tensor IR generation dispatcher
void generate_tensor_ir(IRList *ir_list, ASTNode *node, void *sym_table) {
    // This function is kept for compatibility but not used in C-only context
    (void)ir_list;
    (void)node;
    (void)sym_table;
}
