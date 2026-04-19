#ifndef STACK_FRAME_H
#define STACK_FRAME_H

#include "ir.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── System V AMD64 ABI register sets ──────────────────────────────────────
 *
 *  Argument registers (caller places args here before a call):
 *    %rdi  %rsi  %rdx  %rcx  %r8  %r9   (integer/pointer, args 1-6)
 *    %xmm0-7  (floating point — not handled here)
 *
 *  Return value: %rax (integer/pointer)
 *
 *  Callee-saved (function must preserve across calls):
 *    %rbx  %r12  %r13  %r14  %r15  %rbp
 *
 *  Caller-saved (function may freely clobber):
 *    %rax  %rcx  %rdx  %rsi  %rdi  %r8  %r9  %r10  %r11
 *
 *  Stack alignment: %rsp must be 16-byte aligned immediately before a CALL
 *  (the CALL itself pushes an 8-byte return address, so at function entry
 *  %rsp is 8 mod 16).
 *
 *  Frame layout (addresses grow down):
 *
 *    higher ────────────────────────────────────
 *           arg8, arg7  (extra args pushed by caller, if any)
 *           return address        ← %rsp at function entry
 *    %rbp → saved %rbp            ← after "push %rbp; mov %rsp,%rbp"
 *           saved callee regs     (each 8 bytes)
 *           local / spill slots   (each 8 bytes, rbp_offset = -8, -16, …)
 *    %rsp → ─── (frame bottom, aligned to 16)
 *    lower ────────────────────────────────────
 * ────────────────────────────────────────────────────────────────────────── */

#define SF_MAX_LOCALS 256

/* Bitmask indices for callee-saved registers */
#define SF_REG_RBX  0
#define SF_REG_R12  1
#define SF_REG_R13  2
#define SF_REG_R14  3
#define SF_REG_R15  4
#define SF_NUM_CALLEE_SAVED 5

typedef struct {
    char name[64];    /* variable / temp name from IR */
    int  rbp_offset;  /* e.g. -8, -16, … (always negative) */
} FrameSlot;

typedef struct {
    FrameSlot *slots;
    int        slot_count;
    int        slot_cap;

    int  frame_size;       /* total bytes to sub from %rsp (16-byte aligned) */
    int  callee_mask;      /* OR of (1<<SF_REG_*) for regs the function uses */
    int  next_offset;      /* next free slot (starts at -8, decrements by 8)  */
} FrameLayout;

/* ── public API ─────────────────────────────────────────────────────────── */

/* Scan ir_list for one function, build a FrameLayout.
 * `extra_slots` pre-allocates that many additional 8-byte slots (for spills).
 * `callee_mask` should come from the register allocator; pass 0 if unknown. */
FrameLayout *build_frame_layout(IRList *ir_list, int extra_slots, int callee_mask);

/* Reserve a new 8-byte slot and return its rbp_offset. */
int  frame_alloc_slot(FrameLayout *fl);

/* Look up an existing variable's slot.  Returns 0 (no slot) if not found. */
int  frame_slot_of(FrameLayout *fl, const char *name);

/* Format an operand string for a stack slot: "-8(%rbp)" etc.
 * `buf` must hold at least 24 bytes. */
void frame_slot_str(int rbp_offset, char *buf);

/* Emit function prologue: .globl, label, push rbp, mov rsp→rbp,
 * push callee-saved, sub rsp frame_size. */
void emit_prologue(FILE *out, const char *func_name, FrameLayout *fl);

/* Emit function epilogue: add rsp frame_size, pop callee-saved (reversed),
 * pop rbp, ret. */
void emit_epilogue(FILE *out, FrameLayout *fl);

/* Emit the System V AMD64 argument-passing sequence for a CALL instruction.
 * `params` is an array of `n_params` operand strings (already resolved to
 * physical regs or stack addresses).  Up to 6 go into rdi/rsi/rdx/rcx/r8/r9;
 * extras are pushed right-to-left. */
void emit_call_args(FILE *out, const char **params, int n_params);

void free_frame_layout(FrameLayout *fl);

#ifdef __cplusplus
}
#endif

#endif /* STACK_FRAME_H */
