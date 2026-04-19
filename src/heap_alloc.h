#ifndef HEAP_ALLOC_H
#define HEAP_ALLOC_H

#include "ir.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── Runtime emission ───────────────────────────────────────────────────────
 *
 * Writes two allocator routines into the output assembly file:
 *
 *   __bump_alloc(rdi=size) → rax = pointer
 *     Simple bump-pointer over a 64 KB static heap.
 *     Size is rounded up to the next 8-byte boundary.
 *     No free.  Used for short-lived or arena-style allocation.
 *
 *   __freelist_alloc(rdi=size) → rax = pointer
 *     First-fit search over a free list built on the same heap.
 *     Each block carries a 24-byte header {size, flags, next_ptr}.
 *     Falls back to bump_alloc when no suitable free block exists.
 *
 *   __freelist_free(rdi=ptr)
 *     Marks the block free and coalesces adjacent free blocks.
 *
 * Call emit_heap_runtime() once per output file (before any functions).
 * ────────────────────────────────────────────────────────────────────────── */
void emit_heap_runtime(FILE *out);

/* ── IR lowering ────────────────────────────────────────────────────────────
 *
 * Scans ir_list for:
 *   IR_CALL  result = CALL alloc(1 arg)  ← generic alloc()
 *   IR_CALL  result = CALL bump_alloc(1 arg)
 *   IR_CALL  result = CALL freelist_alloc(1 arg)
 *   IR_CALL  result = CALL free(1 arg)
 *
 * and rewrites them to IR_ALLOC nodes (or a free-marker CALL node) that
 * the code generator knows how to lower to the correct System V AMD64
 * calling sequence (size in %rdi, result from %rax).
 *
 * Returns the number of nodes rewritten.
 * ────────────────────────────────────────────────────────────────────────── */
int lower_alloc_calls(IRList *ir_list);

/* Which runtime to use for a lowered IR_ALLOC node (stored in ir->arg2). */
#define ALLOC_BUMP     "bump"
#define ALLOC_FREELIST "freelist"

#ifdef __cplusplus
}
#endif

#endif /* HEAP_ALLOC_H */
