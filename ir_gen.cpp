#include "ir_gen.h"
#include "temp_var.h"
#include "tensor_ir.h"
#include "symbol_table.h"
#include <cstring>
#include <cstdlib>

extern "C" {

// Forward declarations
static char* gen_expr(IRList *ir_list, ASTNode *node, SymbolTable *sym_table);
static void gen_stmt(IRList *ir_list, ASTNode *node, SymbolTable *sym_table);

// Generate IR for expressions (returns the temporary variable holding the result)
char* generate_expr_ir(IRList *ir_list, ASTNode *node, SymbolTable *sym_table) {
    return gen_expr(ir_list, node, sym_table);
}

// Generate IR for statements
void generate_stmt_ir(IRList *ir_list, ASTNode *node, SymbolTable *sym_table) {
    gen_stmt(ir_list, node, sym_table);
}

// Generate IR for declarations
void generate_decl_ir(IRList *ir_list, ASTNode *node, SymbolTable *sym_table) {
    if (!node) return;
    
    switch (node->type) {
        case AST_TENSOR_DECL:
            // Tensor declarations are handled in symbol table
            // No IR needed for declaration itself
            break;
            
        case AST_VARIABLE_DECL:
            // Variable declaration with initialization
            if (node->right) {
                char *init_val = gen_expr(ir_list, node->right, sym_table);
                if (init_val) {
                    append_ir(ir_list, create_ir_assign(node->name, init_val));
                    free(init_val);
                }
            }
            break;
            
        case AST_FUNCTION_DECL:
            // Function body
            if (node->body) {
                gen_stmt(ir_list, node->body, sym_table);
            }
            break;
            
        default:
            break;
    }
}

// Generate IR for expressions
static char* gen_expr(IRList *ir_list, ASTNode *node, SymbolTable *sym_table) {
    if (!node) return NULL;
    
    char *result = (char*)malloc(64);
    
    switch (node->type) {
        case AST_NUMBER: {
            sprintf(result, "%d", node->int_value);
            return result;
        }
        
        case AST_FLOAT_NUMBER: {
            sprintf(result, "%f", node->float_value);
            return result;
        }
        
        case AST_IDENTIFIER: {
            strcpy(result, node->name);
            return result;
        }
        
        case AST_BINARY_OP: {
            char *left = gen_expr(ir_list, node->left, sym_table);
            char *right = gen_expr(ir_list, node->right, sym_table);
            
            get_temp_var(result, 64);
            
            IROpcode opcode;
            switch (node->op) {
                case OP_ADD: opcode = IR_ADD; break;
                case OP_SUB: opcode = IR_SUB; break;
                case OP_MUL: opcode = IR_MUL; break;
                case OP_DIV: opcode = IR_DIV; break;
                case OP_MOD: opcode = IR_MOD; break;
                case OP_EQ:  opcode = IR_EQ; break;
                case OP_NEQ: opcode = IR_NEQ; break;
                case OP_LT:  opcode = IR_LT; break;
                case OP_GT:  opcode = IR_GT; break;
                case OP_LEQ: opcode = IR_LEQ; break;
                case OP_GEQ: opcode = IR_GEQ; break;
                case OP_AND: opcode = IR_AND; break;
                case OP_OR:  opcode = IR_OR; break;
                default: opcode = IR_ADD; break;
            }
            
            append_ir(ir_list, create_ir_binary(opcode, result, left, right));
            
            free(left);
            free(right);
            return result;
        }
        
        case AST_UNARY_OP: {
            char *operand = gen_expr(ir_list, node->left, sym_table);
            get_temp_var(result, 64);
            
            IROpcode opcode = (node->op == OP_NOT) ? IR_NOT : IR_NEG;
            append_ir(ir_list, create_ir_unary(opcode, result, operand));
            
            free(operand);
            return result;
        }
        
        case AST_ASSIGNMENT: {
            char *rhs = gen_expr(ir_list, node->right, sym_table);
            
            if (node->left && node->left->type == AST_IDENTIFIER) {
                append_ir(ir_list, create_ir_assign(node->left->name, rhs));
                strcpy(result, node->left->name);
            }
            
            free(rhs);
            return result;
        }
        
        case AST_TENSOR_ADD:
        case AST_TENSOR_SUB:
        case AST_TENSOR_MUL: {
            // These are handled in assignment context
            free(result);
            return NULL;
        }
        
        case AST_FUNCTION_CALL: {
            // Generate IR for arguments
            if (node->left && node->left->type == AST_ARG_LIST) {
                for (int i = 0; i < node->left->num_children; i++) {
                    char *arg = gen_expr(ir_list, node->left->children[i], sym_table);
                    if (arg) {
                        IR *param_ir = create_ir(IR_PARAM);
                        strcpy(param_ir->op, "PARAM");
                        strncpy(param_ir->arg1, arg, sizeof(param_ir->arg1) - 1);
                        append_ir(ir_list, param_ir);
                        free(arg);
                    }
                }
            }
            
            get_temp_var(result, 64);
            int num_args = (node->left && node->left->type == AST_ARG_LIST) ? 
                          node->left->num_children : 0;
            append_ir(ir_list, create_ir_call(result, node->name, num_args));
            return result;
        }
        
        default:
            free(result);
            return NULL;
    }
}

// Generate IR for statements
static void gen_stmt(IRList *ir_list, ASTNode *node, SymbolTable *sym_table) {
    if (!node) return;
    
    switch (node->type) {
        case AST_COMPOUND_STMT:
        case AST_STATEMENT_LIST:
            if (node->children) {
                for (int i = 0; i < node->num_children; i++) {
                    gen_stmt(ir_list, node->children[i], sym_table);
                }
            }
            break;
            
        case AST_EXPR_STMT:
            if (node->left) {
                char *result = gen_expr(ir_list, node->left, sym_table);
                if (result) free(result);
            }
            break;
            
        case AST_ASSIGNMENT: {
            // Check if this is a tensor operation
            if (node->right && 
                (node->right->type == AST_TENSOR_ADD ||
                 node->right->type == AST_TENSOR_SUB ||
                 node->right->type == AST_TENSOR_MUL)) {
                
                ASTNode *tensor_op = node->right;
                if (tensor_op->left && tensor_op->right && node->left) {
                    const char *dest = node->left->name;
                    const char *lhs = tensor_op->left->name;
                    const char *rhs = tensor_op->right->name;
                    
                    SymbolEntry *entry = sym_table->lookup(lhs);
                    if (entry && entry->is_tensor) {
                        TensorInfo tensor_info;
                        tensor_info.is_tensor = entry->is_tensor;
                        tensor_info.num_dimensions = entry->num_dimensions;
                        for (int i = 0; i < entry->num_dimensions && i < 10; i++) {
                            tensor_info.shape[i] = entry->shape[i];
                        }
                        
                        const char *op = (tensor_op->type == AST_TENSOR_ADD) ? "+" :
                                       (tensor_op->type == AST_TENSOR_SUB) ? "-" : "*";
                        generate_tensor_loops(ir_list, &tensor_info, dest, lhs, rhs, op);
                    }
                }
            } else {
                // Regular assignment
                char *rhs = gen_expr(ir_list, node->right, sym_table);
                if (rhs && node->left && node->left->type == AST_IDENTIFIER) {
                    append_ir(ir_list, create_ir_assign(node->left->name, rhs));
                }
                if (rhs) free(rhs);
            }
            break;
        }
        
        case AST_IF_STMT: {
            char *cond = gen_expr(ir_list, node->condition, sym_table);
            
            char else_label[32], end_label[32];
            get_label(else_label, sizeof(else_label));
            get_label(end_label, sizeof(end_label));
            
            // IF_FALSE condition GOTO else_label
            IR *if_ir = create_ir(IR_IF_FALSE);
            strcpy(if_ir->op, "IF_FALSE");
            strncpy(if_ir->arg1, cond, sizeof(if_ir->arg1) - 1);
            strncpy(if_ir->result, node->else_branch ? else_label : end_label, 
                   sizeof(if_ir->result) - 1);
            append_ir(ir_list, if_ir);
            
            // Then branch
            gen_stmt(ir_list, node->then_branch, sym_table);
            
            if (node->else_branch) {
                // GOTO end_label
                IR *goto_ir = create_ir(IR_GOTO);
                strcpy(goto_ir->op, "GOTO");
                strncpy(goto_ir->result, end_label, sizeof(goto_ir->result) - 1);
                append_ir(ir_list, goto_ir);
                
                // else_label:
                IR *else_label_ir = create_ir(IR_LABEL);
                strcpy(else_label_ir->op, "LABEL");
                strncpy(else_label_ir->result, else_label, sizeof(else_label_ir->result) - 1);
                append_ir(ir_list, else_label_ir);
                
                // Else branch
                gen_stmt(ir_list, node->else_branch, sym_table);
            }
            
            // end_label:
            IR *end_label_ir = create_ir(IR_LABEL);
            strcpy(end_label_ir->op, "LABEL");
            strncpy(end_label_ir->result, end_label, sizeof(end_label_ir->result) - 1);
            append_ir(ir_list, end_label_ir);
            
            if (cond) free(cond);
            break;
        }
        
        case AST_WHILE_STMT: {
            char start_label[32], end_label[32];
            get_label(start_label, sizeof(start_label));
            get_label(end_label, sizeof(end_label));
            
            // start_label:
            IR *start_ir = create_ir(IR_LABEL);
            strcpy(start_ir->op, "LABEL");
            strncpy(start_ir->result, start_label, sizeof(start_ir->result) - 1);
            append_ir(ir_list, start_ir);
            
            // Condition
            char *cond = gen_expr(ir_list, node->condition, sym_table);
            
            // IF_FALSE condition GOTO end_label
            IR *if_ir = create_ir(IR_IF_FALSE);
            strcpy(if_ir->op, "IF_FALSE");
            strncpy(if_ir->arg1, cond, sizeof(if_ir->arg1) - 1);
            strncpy(if_ir->result, end_label, sizeof(if_ir->result) - 1);
            append_ir(ir_list, if_ir);
            
            // Body
            gen_stmt(ir_list, node->body, sym_table);
            
            // GOTO start_label
            IR *goto_ir = create_ir(IR_GOTO);
            strcpy(goto_ir->op, "GOTO");
            strncpy(goto_ir->result, start_label, sizeof(goto_ir->result) - 1);
            append_ir(ir_list, goto_ir);
            
            // end_label:
            IR *end_ir = create_ir(IR_LABEL);
            strcpy(end_ir->op, "LABEL");
            strncpy(end_ir->result, end_label, sizeof(end_ir->result) - 1);
            append_ir(ir_list, end_ir);
            
            if (cond) free(cond);
            break;
        }
        
        case AST_RETURN_STMT: {
            if (node->left) {
                char *ret_val = gen_expr(ir_list, node->left, sym_table);
                append_ir(ir_list, create_ir_return(ret_val));
                if (ret_val) free(ret_val);
            } else {
                append_ir(ir_list, create_ir_return(NULL));
            }
            break;
        }
        
        case AST_VARIABLE_DECL:
            generate_decl_ir(ir_list, node, sym_table);
            break;
            
        default:
            break;
    }
}

// Main IR generation function
IRList* generate_ir_from_ast(ASTNode *ast, SymbolTable *sym_table) {
    if (!ast) return NULL;
    
    IRList *ir_list = create_ir_list();
    reset_temp_counter();
    
    // Handle program node
    if (ast->type == AST_PROGRAM || ast->type == AST_DECLARATION_LIST) {
        if (ast->children) {
            for (int i = 0; i < ast->num_children; i++) {
                ASTNode *decl = ast->children[i];
                
                if (decl->type == AST_FUNCTION_DECL) {
                    // Generate IR for function
                    generate_decl_ir(ir_list, decl, sym_table);
                } else if (decl->type == AST_VARIABLE_DECL) {
                    generate_decl_ir(ir_list, decl, sym_table);
                }
                // Tensor declarations don't need IR
            }
        }
    } else {
        // Single node
        if (ast->type == AST_FUNCTION_DECL || ast->type == AST_VARIABLE_DECL) {
            generate_decl_ir(ir_list, ast, sym_table);
        } else {
            gen_stmt(ir_list, ast, sym_table);
        }
    }
    
    return ir_list;
}

} // extern "C"
