#ifndef IR_H
#define IR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    IR_ASSIGN,
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_MOD,
    IR_LOAD,
    IR_STORE,
    IR_LABEL,
    IR_GOTO,
    IR_IF_FALSE,
    IR_IF_TRUE,
    IR_FOR_BEGIN,
    IR_FOR_END,
    IR_PARAM,
    IR_CALL,
    IR_RETURN,
    IR_EQ,
    IR_NEQ,
    IR_LT,
    IR_GT,
    IR_LEQ,
    IR_GEQ,
    IR_AND,
    IR_OR,
    IR_NOT,
    IR_NEG
} IROpcode;

typedef struct IR {
    IROpcode opcode;
    char op[16];           
    char arg1[64];         
    char arg2[64];         
    char result[64];       
    int label_num;         
    struct IR *next;       
} IR;

typedef struct IRList {
    IR *head;
    IR *tail;
    int count;
} IRList;


IRList* create_ir_list();
void append_ir(IRList *list, IR *instruction);
void free_ir_list(IRList *list);


IR* create_ir(IROpcode opcode);
IR* create_ir_assign(const char *result, const char *arg1);
IR* create_ir_binary(IROpcode opcode, const char *result, const char *arg1, const char *arg2);
IR* create_ir_unary(IROpcode opcode, const char *result, const char *arg1);
IR* create_ir_load(const char *result, const char *array, const char *index);
IR* create_ir_store(const char *array, const char *index, const char *value);
IR* create_ir_label(int label_num);
IR* create_ir_goto(int label_num);
IR* create_ir_if(IROpcode opcode, const char *condition, int label_num);
IR* create_ir_for_begin(const char *var, const char *start, const char *end);
IR* create_ir_for_end();
IR* create_ir_call(const char *result, const char *func_name, int num_args);
IR* create_ir_return(const char *value);


void print_ir(IR *instruction);
void print_ir_list(IRList *list);


const char* ir_opcode_to_string(IROpcode opcode);

#ifdef __cplusplus
}
#endif

#endif 
