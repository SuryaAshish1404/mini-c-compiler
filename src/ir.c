#include "ir.h"

IRList* create_ir_list() {
    IRList *list = (IRList*)malloc(sizeof(IRList));
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
    return list;
}

void append_ir(IRList *list, IR *instruction) {
    if (!list || !instruction) return;
    
    instruction->next = NULL;
    
    if (list->tail) {
        list->tail->next = instruction;
        list->tail = instruction;
    } else {
        list->head = list->tail = instruction;
    }
    
    list->count++;
}

void free_ir_list(IRList *list) {
    if (!list) return;
    
    IR *current = list->head;
    while (current) {
        IR *next = current->next;
        free(current);
        current = next;
    }
    
    free(list);
}

IR* create_ir(IROpcode opcode) {
    IR *ir = (IR*)malloc(sizeof(IR));
    memset(ir, 0, sizeof(IR));
    ir->opcode = opcode;
    ir->next = NULL;
    ir->label_num = -1;
    return ir;
}

IR* create_ir_assign(const char *result, const char *arg1) {
    IR *ir = create_ir(IR_ASSIGN);
    strcpy(ir->op, "=");
    strncpy(ir->result, result, sizeof(ir->result) - 1);
    strncpy(ir->arg1, arg1, sizeof(ir->arg1) - 1);
    return ir;
}

IR* create_ir_binary(IROpcode opcode, const char *result, const char *arg1, const char *arg2) {
    IR *ir = create_ir(opcode);
    strncpy(ir->result, result, sizeof(ir->result) - 1);
    strncpy(ir->arg1, arg1, sizeof(ir->arg1) - 1);
    strncpy(ir->arg2, arg2, sizeof(ir->arg2) - 1);
    
    switch (opcode) {
        case IR_ADD: strcpy(ir->op, "+"); break;
        case IR_SUB: strcpy(ir->op, "-"); break;
        case IR_MUL: strcpy(ir->op, "*"); break;
        case IR_DIV: strcpy(ir->op, "/"); break;
        case IR_MOD: strcpy(ir->op, "%"); break;
        case IR_EQ:  strcpy(ir->op, "=="); break;
        case IR_NEQ: strcpy(ir->op, "!="); break;
        case IR_LT:  strcpy(ir->op, "<"); break;
        case IR_GT:  strcpy(ir->op, ">"); break;
        case IR_LEQ: strcpy(ir->op, "<="); break;
        case IR_GEQ: strcpy(ir->op, ">="); break;
        case IR_AND: strcpy(ir->op, "&&"); break;
        case IR_OR:  strcpy(ir->op, "||"); break;
        default: strcpy(ir->op, "?"); break;
    }
    
    return ir;
}

IR* create_ir_unary(IROpcode opcode, const char *result, const char *arg1) {
    IR *ir = create_ir(opcode);
    strncpy(ir->result, result, sizeof(ir->result) - 1);
    strncpy(ir->arg1, arg1, sizeof(ir->arg1) - 1);
    
    switch (opcode) {
        case IR_NOT: strcpy(ir->op, "!"); break;
        case IR_NEG: strcpy(ir->op, "-"); break;
        default: strcpy(ir->op, "?"); break;
    }
    
    return ir;
}

IR* create_ir_load(const char *result, const char *array, const char *index) {
    IR *ir = create_ir(IR_LOAD);
    strcpy(ir->op, "LOAD");
    strncpy(ir->result, result, sizeof(ir->result) - 1);
    strncpy(ir->arg1, array, sizeof(ir->arg1) - 1);
    strncpy(ir->arg2, index, sizeof(ir->arg2) - 1);
    return ir;
}

IR* create_ir_store(const char *array, const char *index, const char *value) {
    IR *ir = create_ir(IR_STORE);
    strcpy(ir->op, "STORE");
    strncpy(ir->result, array, sizeof(ir->result) - 1);
    strncpy(ir->arg1, index, sizeof(ir->arg1) - 1);
    strncpy(ir->arg2, value, sizeof(ir->arg2) - 1);
    return ir;
}

IR* create_ir_label(int label_num) {
    IR *ir = create_ir(IR_LABEL);
    strcpy(ir->op, "LABEL");
    ir->label_num = label_num;
    sprintf(ir->result, "L%d", label_num);
    return ir;
}

IR* create_ir_goto(int label_num) {
    IR *ir = create_ir(IR_GOTO);
    strcpy(ir->op, "GOTO");
    ir->label_num = label_num;
    sprintf(ir->result, "L%d", label_num);
    return ir;
}

IR* create_ir_if(IROpcode opcode, const char *condition, int label_num) {
    IR *ir = create_ir(opcode);
    strcpy(ir->op, opcode == IR_IF_FALSE ? "IF_FALSE" : "IF_TRUE");
    strncpy(ir->arg1, condition, sizeof(ir->arg1) - 1);
    ir->label_num = label_num;
    sprintf(ir->result, "L%d", label_num);
    return ir;
}

IR* create_ir_for_begin(const char *var, const char *start, const char *end) {
    IR *ir = create_ir(IR_FOR_BEGIN);
    strcpy(ir->op, "FOR");
    strncpy(ir->result, var, sizeof(ir->result) - 1);
    strncpy(ir->arg1, start, sizeof(ir->arg1) - 1);
    strncpy(ir->arg2, end, sizeof(ir->arg2) - 1);
    return ir;
}

IR* create_ir_for_end() {
    IR *ir = create_ir(IR_FOR_END);
    strcpy(ir->op, "END_FOR");
    return ir;
}

IR* create_ir_call(const char *result, const char *func_name, int num_args) {
    IR *ir = create_ir(IR_CALL);
    strcpy(ir->op, "CALL");
    if (result) {
        strncpy(ir->result, result, sizeof(ir->result) - 1);
    }
    strncpy(ir->arg1, func_name, sizeof(ir->arg1) - 1);
    sprintf(ir->arg2, "%d", num_args);
    return ir;
}

IR* create_ir_return(const char *value) {
    IR *ir = create_ir(IR_RETURN);
    strcpy(ir->op, "RETURN");
    if (value) {
        strncpy(ir->result, value, sizeof(ir->result) - 1);
    }
    return ir;
}

IR* create_ir_alloc(const char *result, const char *size_operand) {
    IR *ir = create_ir(IR_ALLOC);
    strcpy(ir->op, "ALLOC");
    strncpy(ir->result, result,       sizeof(ir->result) - 1);
    strncpy(ir->arg1,   size_operand, sizeof(ir->arg1)   - 1);
    return ir;
}

const char* ir_opcode_to_string(IROpcode opcode) {
    switch (opcode) {
        case IR_ASSIGN: return "ASSIGN";
        case IR_ADD: return "ADD";
        case IR_SUB: return "SUB";
        case IR_MUL: return "MUL";
        case IR_DIV: return "DIV";
        case IR_MOD: return "MOD";
        case IR_LOAD: return "LOAD";
        case IR_STORE: return "STORE";
        case IR_LABEL: return "LABEL";
        case IR_GOTO: return "GOTO";
        case IR_IF_FALSE: return "IF_FALSE";
        case IR_IF_TRUE: return "IF_TRUE";
        case IR_FOR_BEGIN: return "FOR_BEGIN";
        case IR_FOR_END: return "FOR_END";
        case IR_PARAM: return "PARAM";
        case IR_CALL: return "CALL";
        case IR_RETURN: return "RETURN";
        case IR_EQ: return "EQ";
        case IR_NEQ: return "NEQ";
        case IR_LT: return "LT";
        case IR_GT: return "GT";
        case IR_LEQ: return "LEQ";
        case IR_GEQ: return "GEQ";
        case IR_AND: return "AND";
        case IR_OR: return "OR";
        case IR_NOT: return "NOT";
        case IR_NEG:   return "NEG";
        case IR_ALLOC: return "ALLOC";
        default: return "UNKNOWN";
    }
}

void print_ir(IR *instruction) {
    if (!instruction) return;
    
    switch (instruction->opcode) {
        case IR_LABEL:
            printf("%s:\n", instruction->result);
            break;
            
        case IR_ASSIGN:
            printf("    %s = %s\n", instruction->result, instruction->arg1);
            break;
            
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
            printf("    %s = %s %s %s\n", instruction->result, 
                   instruction->arg1, instruction->op, instruction->arg2);
            break;
            
        case IR_NOT:
        case IR_NEG:
            printf("    %s = %s%s\n", instruction->result, 
                   instruction->op, instruction->arg1);
            break;
            
        case IR_LOAD:
            printf("    %s = LOAD %s[%s]\n", instruction->result, 
                   instruction->arg1, instruction->arg2);
            break;
            
        case IR_STORE:
            printf("    STORE %s[%s] = %s\n", instruction->result, 
                   instruction->arg1, instruction->arg2);
            break;
            
        case IR_FOR_BEGIN:
            printf("    FOR %s = %s TO %s\n", instruction->result, 
                   instruction->arg1, instruction->arg2);
            break;
            
        case IR_FOR_END:
            printf("    END_FOR\n");
            break;
            
        case IR_GOTO:
            printf("    GOTO %s\n", instruction->result);
            break;
            
        case IR_IF_FALSE:
            printf("    IF_FALSE %s GOTO %s\n", instruction->arg1, instruction->result);
            break;
            
        case IR_IF_TRUE:
            printf("    IF_TRUE %s GOTO %s\n", instruction->arg1, instruction->result);
            break;
            
        case IR_CALL:
            if (instruction->result[0]) {
                printf("    %s = CALL %s(%s args)\n", instruction->result, 
                       instruction->arg1, instruction->arg2);
            } else {
                printf("    CALL %s(%s args)\n", instruction->arg1, instruction->arg2);
            }
            break;
            
        case IR_RETURN:
            if (instruction->result[0]) {
                printf("    RETURN %s\n", instruction->result);
            } else {
                printf("    RETURN\n");
            }
            break;
            
        default:
            printf("    %s\n", ir_opcode_to_string(instruction->opcode));
            break;
    }
}


void print_ir_list(IRList *list) {
    if (!list) return;
    
    printf("\n===== Intermediate Representation (IR) =====\n");
    
    IR *current = list->head;
    while (current) {
        print_ir(current);
        current = current->next;
    }
    
    printf("============================================\n");
    printf("Total IR instructions: %d\n\n", list->count);
}
