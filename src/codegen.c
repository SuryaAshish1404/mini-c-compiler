#include "codegen.h"
#include <stdlib.h>
#include <string.h>

CodeGenContext* create_codegen_context(FILE* output) {
    CodeGenContext* ctx = (CodeGenContext*)malloc(sizeof(CodeGenContext));
    ctx->output = output;
    ctx->label_counter = 0;
    ctx->stack_offset = 0;
    ctx->current_function = NULL;
    return ctx;
}

void free_codegen_context(CodeGenContext* ctx) {
    if (ctx->current_function) free(ctx->current_function);
    free(ctx);
}

void generate_function_prologue(CodeGenContext* ctx, const char* func_name) {
    fprintf(ctx->output, "\n.globl %s\n", func_name);
    fprintf(ctx->output, "%s:\n", func_name);
    fprintf(ctx->output, "    pushq %%rbp\n");
    fprintf(ctx->output, "    movq %%rsp, %%rbp\n");
    ctx->stack_offset = 0;
}

void generate_function_epilogue(CodeGenContext* ctx) {
    fprintf(ctx->output, "    movq %%rbp, %%rsp\n");
    fprintf(ctx->output, "    popq %%rbp\n");
    fprintf(ctx->output, "    ret\n");
}

const char* get_register(int index) {
    static const char* regs[] = {"%rax", "%rbx", "%rcx", "%rdx", "%rsi", "%rdi"};
    if (index < 0 || index >= 6) return "%rax";
    return regs[index];
}

void generate_ir_instruction(CodeGenContext* ctx, IR* ir) {
    if (!ir) return;
    
    switch (ir->opcode) {
        case IR_ASSIGN:
            fprintf(ctx->output, "    # %s = %s\n", ir->result, ir->arg1);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    movq %%rax, %s\n", ir->result);
            break;
            
        case IR_ADD:
            fprintf(ctx->output, "    # %s = %s + %s\n", ir->result, ir->arg1, ir->arg2);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    addq %s, %%rax\n", ir->arg2);
            fprintf(ctx->output, "    movq %%rax, %s\n", ir->result);
            break;
            
        case IR_SUB:
            fprintf(ctx->output, "    # %s = %s - %s\n", ir->result, ir->arg1, ir->arg2);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    subq %s, %%rax\n", ir->arg2);
            fprintf(ctx->output, "    movq %%rax, %s\n", ir->result);
            break;
            
        case IR_MUL:
            fprintf(ctx->output, "    # %s = %s * %s\n", ir->result, ir->arg1, ir->arg2);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    imulq %s, %%rax\n", ir->arg2);
            fprintf(ctx->output, "    movq %%rax, %s\n", ir->result);
            break;
            
        case IR_DIV:
            fprintf(ctx->output, "    # %s = %s / %s\n", ir->result, ir->arg1, ir->arg2);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    cqto\n");
            fprintf(ctx->output, "    idivq %s\n", ir->arg2);
            fprintf(ctx->output, "    movq %%rax, %s\n", ir->result);
            break;
            
        case IR_MOD:
            fprintf(ctx->output, "    # %s = %s %% %s\n", ir->result, ir->arg1, ir->arg2);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    cqto\n");
            fprintf(ctx->output, "    idivq %s\n", ir->arg2);
            fprintf(ctx->output, "    movq %%rdx, %s\n", ir->result);
            break;
            
        case IR_EQ:
            fprintf(ctx->output, "    # %s = %s == %s\n", ir->result, ir->arg1, ir->arg2);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    cmpq %s, %%rax\n", ir->arg2);
            fprintf(ctx->output, "    sete %%al\n");
            fprintf(ctx->output, "    movzbq %%al, %%rax\n");
            fprintf(ctx->output, "    movq %%rax, %s\n", ir->result);
            break;
            
        case IR_NEQ:
            fprintf(ctx->output, "    # %s = %s != %s\n", ir->result, ir->arg1, ir->arg2);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    cmpq %s, %%rax\n", ir->arg2);
            fprintf(ctx->output, "    setne %%al\n");
            fprintf(ctx->output, "    movzbq %%al, %%rax\n");
            fprintf(ctx->output, "    movq %%rax, %s\n", ir->result);
            break;
            
        case IR_LT:
            fprintf(ctx->output, "    # %s = %s < %s\n", ir->result, ir->arg1, ir->arg2);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    cmpq %s, %%rax\n", ir->arg2);
            fprintf(ctx->output, "    setl %%al\n");
            fprintf(ctx->output, "    movzbq %%al, %%rax\n");
            fprintf(ctx->output, "    movq %%rax, %s\n", ir->result);
            break;
            
        case IR_GT:
            fprintf(ctx->output, "    # %s = %s > %s\n", ir->result, ir->arg1, ir->arg2);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    cmpq %s, %%rax\n", ir->arg2);
            fprintf(ctx->output, "    setg %%al\n");
            fprintf(ctx->output, "    movzbq %%al, %%rax\n");
            fprintf(ctx->output, "    movq %%rax, %s\n", ir->result);
            break;
            
        case IR_LEQ:
            fprintf(ctx->output, "    # %s = %s <= %s\n", ir->result, ir->arg1, ir->arg2);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    cmpq %s, %%rax\n", ir->arg2);
            fprintf(ctx->output, "    setle %%al\n");
            fprintf(ctx->output, "    movzbq %%al, %%rax\n");
            fprintf(ctx->output, "    movq %%rax, %s\n", ir->result);
            break;
            
        case IR_GEQ:
            fprintf(ctx->output, "    # %s = %s >= %s\n", ir->result, ir->arg1, ir->arg2);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    cmpq %s, %%rax\n", ir->arg2);
            fprintf(ctx->output, "    setge %%al\n");
            fprintf(ctx->output, "    movzbq %%al, %%rax\n");
            fprintf(ctx->output, "    movq %%rax, %s\n", ir->result);
            break;
            
        case IR_AND:
            fprintf(ctx->output, "    # %s = %s && %s\n", ir->result, ir->arg1, ir->arg2);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    andq %s, %%rax\n", ir->arg2);
            fprintf(ctx->output, "    movq %%rax, %s\n", ir->result);
            break;
            
        case IR_OR:
            fprintf(ctx->output, "    # %s = %s || %s\n", ir->result, ir->arg1, ir->arg2);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    orq %s, %%rax\n", ir->arg2);
            fprintf(ctx->output, "    movq %%rax, %s\n", ir->result);
            break;
            
        case IR_NOT:
            fprintf(ctx->output, "    # %s = !%s\n", ir->result, ir->arg1);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    testq %%rax, %%rax\n");
            fprintf(ctx->output, "    sete %%al\n");
            fprintf(ctx->output, "    movzbq %%al, %%rax\n");
            fprintf(ctx->output, "    movq %%rax, %s\n", ir->result);
            break;
            
        case IR_NEG:
            fprintf(ctx->output, "    # %s = -%s\n", ir->result, ir->arg1);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    negq %%rax\n");
            fprintf(ctx->output, "    movq %%rax, %s\n", ir->result);
            break;
            
        case IR_LABEL:
            fprintf(ctx->output, "%s:\n", ir->result);
            break;
            
        case IR_GOTO:
            fprintf(ctx->output, "    jmp %s\n", ir->result);
            break;
            
        case IR_IF_FALSE:
            fprintf(ctx->output, "    # if !%s goto %s\n", ir->arg1, ir->result);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    testq %%rax, %%rax\n");
            fprintf(ctx->output, "    je %s\n", ir->result);
            break;
            
        case IR_RETURN:
            if (ir->arg1[0] != '\0') {
                fprintf(ctx->output, "    # return %s\n", ir->arg1);
                fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            } else {
                fprintf(ctx->output, "    # return\n");
            }
            generate_function_epilogue(ctx);
            break;
            
        case IR_CALL:
            fprintf(ctx->output, "    # %s = call %s\n", ir->result, ir->arg1);
            fprintf(ctx->output, "    call %s\n", ir->arg1);
            fprintf(ctx->output, "    movq %%rax, %s\n", ir->result);
            break;
            
        case IR_PARAM:
            fprintf(ctx->output, "    # param %s\n", ir->arg1);
            fprintf(ctx->output, "    pushq %s\n", ir->arg1);
            break;
            
        case IR_LOAD:
            fprintf(ctx->output, "    # %s = load %s[%s]\n", ir->result, ir->arg1, ir->arg2);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg1);
            fprintf(ctx->output, "    movq (%s), %%rax\n", ir->arg2);
            fprintf(ctx->output, "    movq %%rax, %s\n", ir->result);
            break;
            
        case IR_STORE:
            fprintf(ctx->output, "    # store %s[%s] = %s\n", ir->result, ir->arg1, ir->arg2);
            fprintf(ctx->output, "    movq %s, %%rax\n", ir->arg2);
            fprintf(ctx->output, "    movq %%rax, (%s)\n", ir->arg1);
            break;
            
        default:
            fprintf(ctx->output, "    # unknown opcode %d\n", ir->opcode);
            break;
    }
}

void generate_assembly(IRList* ir_list, FILE* output) {
    if (!ir_list || !output) return;
    
    CodeGenContext* ctx = create_codegen_context(output);
    
    fprintf(output, "    .text\n");
    
    IR* current = ir_list->head;
    while (current) {
        generate_ir_instruction(ctx, current);
        current = current->next;
    }
    
    free_codegen_context(ctx);
}
