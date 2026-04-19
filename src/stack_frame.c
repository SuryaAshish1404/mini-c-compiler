#include "stack_frame.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* ═══════════════════════════════════════════════════════════════════════════
 *  Helpers
 * ═══════════════════════════════════════════════════════════════════════════ */

/* Returns 1 if `s` looks like a variable/temp name (not a constant or label).
 * Heuristic: starts with a letter or underscore, not with 'L' followed by
 * a digit (those are IR labels). */
static int is_var_name(const char *s) {
    if (!s || !s[0]) return 0;
    if (s[0] == 'L' && isdigit((unsigned char)s[1])) return 0; /* label */
    if (isdigit((unsigned char)s[0]) || s[0] == '-') return 0; /* constant */
    if (s[0] == '%') return 0; /* already a physical register */
    return 1;
}

static const char *callee_reg_name[SF_NUM_CALLEE_SAVED] = {
    "%rbx", "%r12", "%r13", "%r14", "%r15"
};

/* ═══════════════════════════════════════════════════════════════════════════
 *  FrameLayout management
 * ═══════════════════════════════════════════════════════════════════════════ */

static FrameLayout *fl_new(void) {
    FrameLayout *fl = (FrameLayout *)calloc(1, sizeof(FrameLayout));
    fl->slot_cap    = 16;
    fl->slots       = (FrameSlot *)malloc(fl->slot_cap * sizeof(FrameSlot));
    fl->next_offset = 0; /* will be initialised after counting callee saves */
    return fl;
}

/* Intern a variable name → assign a slot if it doesn't have one yet.
 * Returns the (negative) rbp_offset assigned. */
static int fl_intern(FrameLayout *fl, const char *name) {
    /* check existing */
    for (int i = 0; i < fl->slot_count; i++)
        if (strcmp(fl->slots[i].name, name) == 0)
            return fl->slots[i].rbp_offset;

    /* grow if needed */
    if (fl->slot_count == fl->slot_cap) {
        fl->slot_cap *= 2;
        fl->slots = (FrameSlot *)realloc(fl->slots,
                                         fl->slot_cap * sizeof(FrameSlot));
    }

    fl->next_offset -= 8;
    FrameSlot *s = &fl->slots[fl->slot_count++];
    strncpy(s->name, name, sizeof(s->name) - 1);
    s->rbp_offset = fl->next_offset;
    return s->rbp_offset;
}

/* ── public: reserve an anonymous spill slot ── */
int frame_alloc_slot(FrameLayout *fl) {
    fl->next_offset -= 8;
    if (fl->slot_count == fl->slot_cap) {
        fl->slot_cap *= 2;
        fl->slots = (FrameSlot *)realloc(fl->slots,
                                         fl->slot_cap * sizeof(FrameSlot));
    }
    FrameSlot *s = &fl->slots[fl->slot_count++];
    s->name[0]    = '\0';
    s->rbp_offset = fl->next_offset;
    return s->rbp_offset;
}

int frame_slot_of(FrameLayout *fl, const char *name) {
    for (int i = 0; i < fl->slot_count; i++)
        if (strcmp(fl->slots[i].name, name) == 0)
            return fl->slots[i].rbp_offset;
    return 0;
}

void frame_slot_str(int rbp_offset, char *buf) {
    sprintf(buf, "%d(%%rbp)", rbp_offset);
}

void free_frame_layout(FrameLayout *fl) {
    if (!fl) return;
    free(fl->slots);
    free(fl);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  build_frame_layout
 *
 *  1. Scan ir_list for all variable names appearing as result, arg1, arg2.
 *  2. Assign each a stack slot (rbp-relative, 8 bytes each).
 *  3. Reserve `extra_slots` additional anonymous slots.
 *  4. Account for the callee-saved pushes so that %rsp stays 16-aligned.
 * ═══════════════════════════════════════════════════════════════════════════ */
FrameLayout *build_frame_layout(IRList *ir_list, int extra_slots, int callee_mask) {
    FrameLayout *fl = fl_new();
    fl->callee_mask = callee_mask;

    /* Callee-saved pushes come before locals.  Each push is 8 bytes.
     * We account for them by offsetting next_offset. */
    int n_callee = 0;
    for (int i = 0; i < SF_NUM_CALLEE_SAVED; i++)
        if (callee_mask & (1 << i)) n_callee++;

    /* At function entry %rsp is 8 mod 16 (pushed return addr).
     * After "push %rbp": %rsp is 0 mod 16 → good, %rbp set here.
     * After n_callee pushes: %rsp is (n_callee * 8) mod 16 below %rbp.
     * We need total frame = callee_saves + locals to be ≡ 0 mod 16.
     * We track locals in next_offset; we'll pad at the end. */

    /* Collect variable names */
    IR *ir = ir_list ? ir_list->head : NULL;
    while (ir) {
        if (is_var_name(ir->result)) fl_intern(fl, ir->result);
        if (is_var_name(ir->arg1))   fl_intern(fl, ir->arg1);
        if (is_var_name(ir->arg2))   fl_intern(fl, ir->arg2);
        ir = ir->next;
    }

    /* Reserve anonymous spill slots */
    for (int i = 0; i < extra_slots; i++) frame_alloc_slot(fl);

    /* Compute frame_size: bytes to sub from %rsp.
     * Locals occupy |next_offset| bytes.
     * We need (n_callee * 8 + frame_size) to be 16-byte aligned. */
    int local_bytes = -fl->next_offset; /* positive */
    int total       = n_callee * 8 + local_bytes;
    int pad         = (total % 16 != 0) ? 16 - (total % 16) : 0;
    fl->frame_size  = local_bytes + pad;

    return fl;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  Prologue / epilogue emission
 * ═══════════════════════════════════════════════════════════════════════════ */
void emit_prologue(FILE *out, const char *func_name, FrameLayout *fl) {
    fprintf(out, "\n.globl %s\n%s:\n", func_name, func_name);
    fprintf(out, "    pushq %%rbp\n");
    fprintf(out, "    movq  %%rsp, %%rbp\n");

    /* Push callee-saved registers (in index order). */
    for (int i = 0; i < SF_NUM_CALLEE_SAVED; i++)
        if (fl->callee_mask & (1 << i))
            fprintf(out, "    pushq %s\n", callee_reg_name[i]);

    /* Allocate locals. */
    if (fl->frame_size > 0)
        fprintf(out, "    subq  $%d, %%rsp\n", fl->frame_size);
}

void emit_epilogue(FILE *out, FrameLayout *fl) {
    /* Deallocate locals. */
    if (fl->frame_size > 0)
        fprintf(out, "    addq  $%d, %%rsp\n", fl->frame_size);

    /* Pop callee-saved registers in reverse order. */
    for (int i = SF_NUM_CALLEE_SAVED - 1; i >= 0; i--)
        if (fl->callee_mask & (1 << i))
            fprintf(out, "    popq  %s\n", callee_reg_name[i]);

    fprintf(out, "    popq  %%rbp\n");
    fprintf(out, "    ret\n");
}

/* ── Argument-register table (System V AMD64 integer arguments 1-6) ── */
static const char *arg_regs[6] = {
    "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"
};

void emit_call_args(FILE *out, const char **params, int n_params) {
    /* Push extra args right-to-left (args 7+). */
    for (int i = n_params - 1; i >= 6; i--)
        fprintf(out, "    pushq %s\n", params[i]);

    /* Move first 6 args into registers. */
    int regs = (n_params < 6) ? n_params : 6;
    for (int i = 0; i < regs; i++)
        fprintf(out, "    movq  %s, %s\n", params[i], arg_regs[i]);
}
