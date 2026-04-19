#ifndef CODEGEN_H
#define CODEGEN_H

#include "ir.h"
#include "cfg.h"
#include "stack_frame.h"
#include "regalloc.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    FILE         *output;
    int           label_counter;
    int           loop_counter;
    int           stack_offset;   /* legacy: used by FOR_BEGIN/END */
    char         *current_function;

    /* register allocation + frame (populated per function) */
    RAResult     *ra;
    FrameLayout  *frame;
} CodeGenContext;

CodeGenContext *create_codegen_context(FILE *output);
void            free_codegen_context(CodeGenContext *ctx);

/* Full pipeline: lower allocs, build CFG, run regalloc, emit assembly. */
void generate_assembly(IRList *ir_list, FILE *output);

/* Lower-level helpers (used internally; exposed for testing). */
void generate_function_prologue(CodeGenContext *ctx, const char *func_name);
void generate_function_epilogue(CodeGenContext *ctx);

/* Resolve an IR operand to an asm string (physical reg or stack slot).
 * `buf` must hold ≥ 24 bytes. */
const char *cg_operand(CodeGenContext *ctx, const char *operand, char *buf);

#ifdef __cplusplus
}
#endif

#endif
