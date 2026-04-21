#include "optimizer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define MAX_EXPRESSIONS 256

typedef struct {
    IROpcode opcode;
    char arg1[64];
    char arg2[64];
    char result[64];
} Expression;

static int is_constant(const char* str) {
    if (!str || str[0] == '\0') return 0;
    int i = 0;
    if (str[i] == '-') i++;
    while (str[i]) {
        if (!isdigit(str[i])) return 0;
        i++;
    }
    return 1;
}

static int is_cse_eligible(IROpcode opcode) {
    switch (opcode) {
        case IR_ADD:
        case IR_SUB:
        case IR_MUL:
        case IR_DIV:
        case IR_MOD:
        case IR_EQ:
        case IR_NEQ:
        case IR_LT:
        case IR_GT:
        case IR_LEQ:
        case IR_GEQ:
        case IR_AND:
        case IR_OR:
            return 1;
        default:
            return 0;
    }
}

static int eval_constant_op(int left, int right, IROpcode op) {
    switch (op) {
        case IR_ADD: return left + right;
        case IR_SUB: return left - right;
        case IR_MUL: return left * right;
        case IR_DIV: return right != 0 ? left / right : 0;
        case IR_MOD: return right != 0 ? left % right : 0;
        case IR_EQ:  return left == right;
        case IR_NEQ: return left != right;
        case IR_LT:  return left < right;
        case IR_GT:  return left > right;
        case IR_LEQ: return left <= right;
        case IR_GEQ: return left >= right;
        case IR_AND: return left && right;
        case IR_OR:  return left || right;
        default: return 0;
    }
}

int optimize_constant_folding(IRList* ir_list) {
    if (!ir_list) return 0;
    
    int changes = 0;
    IR* current = ir_list->head;
    
    while (current) {
        if (is_cse_eligible(current->opcode)) {
            if (is_constant(current->arg1) && is_constant(current->arg2)) {
                int left_val = atoi(current->arg1);
                int right_val = atoi(current->arg2);
                int result_val = eval_constant_op(left_val, right_val, current->opcode);
                
                current->opcode = IR_ASSIGN;
                snprintf(current->arg1, sizeof(current->arg1), "%d", result_val);
                current->arg2[0] = '\0';
                strcpy(current->op, "=");
                changes++;
            }
        }
        else if (current->opcode == IR_NEG) {
            if (is_constant(current->arg1)) {
                int val = atoi(current->arg1);
                current->opcode = IR_ASSIGN;
                snprintf(current->arg1, sizeof(current->arg1), "%d", -val);
                strcpy(current->op, "=");
                changes++;
            }
        }
        else if (current->opcode == IR_NOT) {
            if (is_constant(current->arg1)) {
                int val = atoi(current->arg1);
                current->opcode = IR_ASSIGN;
                snprintf(current->arg1, sizeof(current->arg1), "%d", !val);
                strcpy(current->op, "=");
                changes++;
            }
        }
        current = current->next;
    }
    
    return changes;
}

static int is_variable_used_after(IRList* ir_list, IR* start, const char* var) {
    if (!start || !var) return 0;
    
    IR* current = start->next;
    while (current) {
        if (strcmp(current->arg1, var) == 0 || strcmp(current->arg2, var) == 0) {
            return 1;
        }
        if (current->opcode == IR_IF_FALSE && strcmp(current->arg1, var) == 0) {
            return 1;
        }
        if (current->opcode == IR_RETURN && strcmp(current->arg1, var) == 0) {
            return 1;
        }
        if (current->opcode == IR_PARAM && strcmp(current->arg1, var) == 0) {
            return 1;
        }
        if (current->opcode == IR_STORE && strcmp(current->arg2, var) == 0) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

int optimize_dead_code_elimination(IRList* ir_list) {
    if (!ir_list) return 0;

    /* DCE is only safe for straight-line code; skip if control flow exists. */
    for (IR *scan = ir_list->head; scan; scan = scan->next) {
        if (scan->opcode == IR_LABEL || scan->opcode == IR_GOTO ||
            scan->opcode == IR_IF_FALSE || scan->opcode == IR_IF_TRUE ||
            scan->opcode == IR_FOR_BEGIN || scan->opcode == IR_FOR_END) {
            return 0;
        }
    }
    
    int changes = 0;
    IR* current = ir_list->head;
    IR* prev = NULL;
    
    while (current) {
        IR* next = current->next;
        int should_remove = 0;
        
        if (current->opcode == IR_ASSIGN ||
            is_cse_eligible(current->opcode) ||
            current->opcode == IR_NOT ||
            current->opcode == IR_NEG) {
            if (current->result[0] != '\0' && current->result[0] != '_') {
                if (!is_variable_used_after(ir_list, current, current->result)) {
                    should_remove = 1;
                }
            }
        }
        
        if (should_remove) {
            if (prev) {
                prev->next = next;
            } else {
                ir_list->head = next;
            }
            if (current == ir_list->tail) {
                ir_list->tail = prev;
            }
            free(current);
            ir_list->count--;
            changes++;
            current = next;
        } else {
            prev = current;
            current = next;
        }
    }
    
    return changes;
}

static int expressions_equal(Expression* e1, Expression* e2) {
    if (e1->opcode != e2->opcode) return 0;
    if (strcmp(e1->arg1, e2->arg1) != 0) return 0;
    if (strcmp(e1->arg2, e2->arg2) != 0) return 0;
    return 1;
}

static int is_commutative(IROpcode op) {
    return (op == IR_ADD || op == IR_MUL || op == IR_EQ || 
            op == IR_NEQ || op == IR_AND || op == IR_OR);
}

static int find_existing_expression(Expression* expr_table, int expr_count, Expression* expr) {
    for (int i = 0; i < expr_count; i++) {
        if (expressions_equal(&expr_table[i], expr)) {
            return i;
        }
        if (is_commutative(expr->opcode)) {
            if (expr_table[i].opcode == expr->opcode &&
                strcmp(expr_table[i].arg1, expr->arg2) == 0 &&
                strcmp(expr_table[i].arg2, expr->arg1) == 0) {
                return i;
            }
        }
    }
    return -1;
}

int optimize_common_subexpression_elimination(IRList* ir_list) {
    if (!ir_list) return 0;
    
    Expression expr_table[MAX_EXPRESSIONS];
    int expr_count = 0;
    int changes = 0;
    
    IR* current = ir_list->head;
    while (current) {
        if (is_cse_eligible(current->opcode)) {
            Expression expr;
            expr.opcode = current->opcode;
            strncpy(expr.arg1, current->arg1, sizeof(expr.arg1) - 1);
            strncpy(expr.arg2, current->arg2, sizeof(expr.arg2) - 1);
            strncpy(expr.result, current->result, sizeof(expr.result) - 1);
            
            int existing = find_existing_expression(expr_table, expr_count, &expr);
            if (existing >= 0) {
                current->opcode = IR_ASSIGN;
                strncpy(current->arg1, expr_table[existing].result, sizeof(current->arg1) - 1);
                current->arg2[0] = '\0';
                strcpy(current->op, "=");
                changes++;
            } else {
                if (expr_count < MAX_EXPRESSIONS) {
                    expr_table[expr_count++] = expr;
                }
            }
        }
        else if (current->opcode == IR_ASSIGN) {
            for (int i = 0; i < expr_count; i++) {
                if (strcmp(expr_table[i].arg1, current->result) == 0 ||
                    strcmp(expr_table[i].arg2, current->result) == 0 ||
                    strcmp(expr_table[i].result, current->result) == 0) {
                    if (i < expr_count - 1) {
                        memmove(&expr_table[i], &expr_table[i + 1], 
                                (expr_count - i - 1) * sizeof(Expression));
                    }
                    expr_count--;
                    i--;
                }
            }
        }
        current = current->next;
    }
    
    return changes;
}

int optimize_ir(IRList* ir_list) {
    if (!ir_list) return 0;
    
    int total_changes = 0;
    int pass_changes;
    int max_passes = 10;
    int pass = 0;
    
    do {
        pass_changes = 0;
        pass_changes += optimize_constant_folding(ir_list);
        pass_changes += optimize_common_subexpression_elimination(ir_list);
        pass_changes += optimize_dead_code_elimination(ir_list);
        total_changes += pass_changes;
        pass++;
    } while (pass_changes > 0 && pass < max_passes);
    
    return total_changes;
}
