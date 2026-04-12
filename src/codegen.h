#ifndef CODEGEN_H
#define CODEGEN_H

#include "ir.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    FILE* output;
    int label_counter;
    int stack_offset;
    char* current_function;
} CodeGenContext;

CodeGenContext* create_codegen_context(FILE* output);
void free_codegen_context(CodeGenContext* ctx);
void generate_assembly(IRList* ir_list, FILE* output);
void generate_function_prologue(CodeGenContext* ctx, const char* func_name);
void generate_function_epilogue(CodeGenContext* ctx);

#ifdef __cplusplus
}
#endif

#endif
