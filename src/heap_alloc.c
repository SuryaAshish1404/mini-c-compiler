#include "heap_alloc.h"
#include <string.h>
#include <stdlib.h>

/* ═══════════════════════════════════════════════════════════════════════════
 *  emit_heap_runtime
 *
 *  Emits the complete allocator assembly into `out`.  Must be called once,
 *  before any function bodies, so the symbols resolve at link time.
 *
 *  Memory layout (in .data / .bss):
 *
 *   __ha_heap   64 KB static region (zero-initialised via .bss)
 *   __ha_off    8-byte quad: bytes consumed from heap (bump pointer offset)
 *   __ha_fl     8-byte quad: pointer to head of free list (0 = empty)
 *
 *  Free-list block header (24 bytes, immediately before user payload):
 *   +0  : size   (uint64) payload bytes, NOT counting the header
 *   +8  : flags  (uint64) bit 0 = 1 → block is free
 *   +16 : next   (uint64) absolute addr of next free-list header, 0 = none
 * ═══════════════════════════════════════════════════════════════════════════ */
void emit_heap_runtime(FILE *out) {
    /* ── BSS / data ─────────────────────────────────────────────────── */
    fprintf(out,
        "\n"
        "    .section .bss\n"
        "    .align 8\n"
        "__ha_heap: .space 65536\n"      /* 64 KB heap arena */
        "__ha_off:  .space 8\n"          /* bump pointer offset */
        "__ha_fl:   .space 8\n"          /* free-list head pointer */
        "\n"
        "    .text\n"
    );

    /* ── __bump_alloc ────────────────────────────────────────────────
     *  in:  rdi = requested bytes
     *  out: rax = aligned pointer into heap, or 0 on overflow
     *
     *  offset = __ha_off
     *  align requested size: size = (rdi + 7) & ~7
     *  if offset + size > 65536 → return 0
     *  rax = &__ha_heap + offset
     *  __ha_off += size
     * ──────────────────────────────────────────────────────────────── */
    fprintf(out,
        "\n"
        ".globl __bump_alloc\n"
        "__bump_alloc:\n"
        "    # rdi = size; returns rax = ptr or 0\n"
        "    addq  $7, %%rdi\n"
        "    andq  $-8, %%rdi\n"                  /* align to 8 */
        "    leaq  __ha_heap(%%rip), %%rax\n"
        "    movq  __ha_off(%%rip), %%rcx\n"
        "    movq  %%rcx, %%rdx\n"
        "    addq  %%rdi, %%rdx\n"               /* rdx = new offset */
        "    cmpq  $65536, %%rdx\n"
        "    ja    .bump_oom\n"
        "    addq  %%rcx, %%rax\n"               /* rax = base + old offset */
        "    movq  %%rdx, __ha_off(%%rip)\n"
        "    ret\n"
        ".bump_oom:\n"
        "    xorl  %%eax, %%eax\n"
        "    ret\n"
    );

    /* ── __freelist_alloc ─────────────────────────────────────────────
     *  in:  rdi = requested bytes
     *  out: rax = ptr (payload start), or 0
     *
     *  First-fit walk of the free list.
     *  If a suitable free block is found:
     *      mark it allocated, return payload pointer.
     *  Otherwise call __bump_alloc(size + 24) for a new block.
     *
     *  Register use:
     *    rbx = current block pointer (header address)
     *    r12 = aligned requested size
     *    r13 = link to previous block's "next" field (for relinking — unused
     *           in first-fit without splitting, but kept for clarity)
     * ──────────────────────────────────────────────────────────────── */
    fprintf(out,
        "\n"
        ".globl __freelist_alloc\n"
        "__freelist_alloc:\n"
        "    pushq %%rbx\n"
        "    pushq %%r12\n"
        "    pushq %%r13\n"
        "    # align requested size → r12\n"
        "    movq  %%rdi, %%r12\n"
        "    addq  $7, %%r12\n"
        "    andq  $-8, %%r12\n"
        "    # walk free list\n"
        "    movq  __ha_fl(%%rip), %%rbx\n"   /* rbx = head */
        "    testq %%rbx, %%rbx\n"
        "    jz    .fl_newblock\n"
        ".fl_walk:\n"
        "    # check flags (offset +8): bit 0 = free\n"
        "    movq  8(%%rbx), %%rax\n"
        "    testq $1, %%rax\n"
        "    jz    .fl_next\n"
        "    # check size (offset +0) >= r12\n"
        "    movq  0(%%rbx), %%rax\n"
        "    cmpq  %%r12, %%rax\n"
        "    jae   .fl_found\n"
        ".fl_next:\n"
        "    movq  16(%%rbx), %%rbx\n"       /* next */
        "    testq %%rbx, %%rbx\n"
        "    jnz   .fl_walk\n"
        ".fl_newblock:\n"
        "    # allocate from bump: size = r12 + 24 (header)\n"
        "    movq  %%r12, %%rdi\n"
        "    addq  $24, %%rdi\n"
        "    call  __bump_alloc\n"
        "    testq %%rax, %%rax\n"
        "    jz    .fl_oom\n"
        "    movq  %%rax, %%rbx\n"          /* rbx = new header */
        "    movq  %%r12, 0(%%rbx)\n"       /* header.size */
        "    movq  $0, 8(%%rbx)\n"          /* header.flags = allocated */
        "    # prepend to free list (mark next = old head)\n"
        "    movq  __ha_fl(%%rip), %%rax\n"
        "    movq  %%rax, 16(%%rbx)\n"
        "    movq  %%rbx, __ha_fl(%%rip)\n"
        "    jmp   .fl_ret\n"
        ".fl_found:\n"
        "    movq  $0, 8(%%rbx)\n"          /* mark allocated: flags &= ~1 */
        ".fl_ret:\n"
        "    leaq  24(%%rbx), %%rax\n"      /* payload = header + 24 */
        "    popq  %%r13\n"
        "    popq  %%r12\n"
        "    popq  %%rbx\n"
        "    ret\n"
        ".fl_oom:\n"
        "    xorl  %%eax, %%eax\n"
        "    popq  %%r13\n"
        "    popq  %%r12\n"
        "    popq  %%rbx\n"
        "    ret\n"
    );

    /* ── __freelist_free ──────────────────────────────────────────────
     *  in:  rdi = payload pointer (what the user holds)
     *
     *  1. Back up to header: rbx = rdi - 24
     *  2. Set header.flags |= 1 (mark free)
     *  3. Coalesce: walk entire list from __ha_fl; for any two adjacent
     *     free blocks (block B immediately follows block A in address space),
     *     merge B into A: A.size += 24 + B.size, A.next = B.next.
     * ──────────────────────────────────────────────────────────────── */
    fprintf(out,
        "\n"
        ".globl __freelist_free\n"
        "__freelist_free:\n"
        "    pushq %%rbx\n"
        "    pushq %%r12\n"
        "    # rbx = header = rdi - 24\n"
        "    leaq  -24(%%rdi), %%rbx\n"
        "    orq   $1, 8(%%rbx)\n"          /* flags |= free */
        "    # coalesce pass: walk list, merge adjacent free blocks\n"
        ".coal_restart:\n"
        "    movq  __ha_fl(%%rip), %%rbx\n"
        "    testq %%rbx, %%rbx\n"
        "    jz    .coal_done\n"
        ".coal_walk:\n"
        "    # is this block free?\n"
        "    movq  8(%%rbx), %%rax\n"
        "    testq $1, %%rax\n"
        "    jz    .coal_advance\n"
        "    # compute address of block immediately after rbx in memory\n"
        "    movq  0(%%rbx), %%r12\n"       /* r12 = rbx.size */
        "    leaq  24(%%rbx, %%r12, 1), %%rax\n"  /* rax = rbx + 24 + size */
        "    # check if rax == next free block in list (naïve: check next ptr)\n"
        "    movq  16(%%rbx), %%rcx\n"      /* rcx = rbx.next */
        "    cmpq  %%rcx, %%rax\n"
        "    jne   .coal_advance\n"
        "    testq %%rcx, %%rcx\n"
        "    jz    .coal_advance\n"
        "    # merge: check both free\n"
        "    movq  8(%%rcx), %%rax\n"
        "    testq $1, %%rax\n"
        "    jz    .coal_advance\n"
        "    # merge rcx into rbx\n"
        "    movq  0(%%rcx), %%rax\n"       /* rax = rcx.size */
        "    addq  $24, %%rax\n"
        "    addq  %%rax, 0(%%rbx)\n"       /* rbx.size += 24 + rcx.size */
        "    movq  16(%%rcx), %%rax\n"      /* rax = rcx.next */
        "    movq  %%rax, 16(%%rbx)\n"      /* rbx.next = rcx.next */
        "    jmp   .coal_restart\n"         /* restart since list changed */
        ".coal_advance:\n"
        "    movq  16(%%rbx), %%rbx\n"
        "    testq %%rbx, %%rbx\n"
        "    jnz   .coal_walk\n"
        ".coal_done:\n"
        "    popq  %%r12\n"
        "    popq  %%rbx\n"
        "    ret\n"
    );
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  lower_alloc_calls
 *
 *  Walk ir_list; for each IR_CALL whose func name is "alloc",
 *  "bump_alloc", or "freelist_alloc", look for the preceding IR_PARAM
 *  (which carries the size argument) and rewrite the pair as a single
 *  IR_ALLOC node:
 *
 *    PARAM  size_operand          ← removed
 *    CALL   result = alloc(1)     ← replaced with IR_ALLOC result, size_operand
 *
 *  The IR_ALLOC node stores:
 *    arg1   = size operand (string)
 *    arg2   = "bump" or "freelist"
 *    result = destination virtual register
 *
 *  Also handles "free(ptr)" → an IR_CALL to "__freelist_free" with rdi=ptr.
 * ═══════════════════════════════════════════════════════════════════════════ */
int lower_alloc_calls(IRList *ir_list) {
    if (!ir_list) return 0;
    int rewrites = 0;

    IR *prev = NULL;
    IR *cur  = ir_list->head;

    while (cur) {
        IR *next = cur->next;

        if (cur->opcode == IR_CALL) {
            const char *fn = cur->arg1;
            int is_bump      = (strcmp(fn, "alloc")        == 0 ||
                                 strcmp(fn, "bump_alloc")   == 0);
            int is_freelist  = (strcmp(fn, "freelist_alloc") == 0);
            int is_free      = (strcmp(fn, "free")          == 0);

            if ((is_bump || is_freelist) && prev && prev->opcode == IR_PARAM) {
                /* Consume the preceding PARAM node as the size argument. */
                char size_op[64];
                strncpy(size_op, prev->arg1, sizeof(size_op) - 1);

                /* Rewrite CALL → IR_ALLOC in place. */
                cur->opcode = IR_ALLOC;
                strcpy(cur->op, "ALLOC");
                strncpy(cur->arg1, size_op, sizeof(cur->arg1) - 1);
                strncpy(cur->arg2, is_freelist ? ALLOC_FREELIST : ALLOC_BUMP,
                        sizeof(cur->arg2) - 1);

                /* Unlink the PARAM node. */
                if (prev == ir_list->head) {
                    ir_list->head = cur;
                } else {
                    /* find node before prev */
                    IR *pp = ir_list->head;
                    while (pp && pp->next != prev) pp = pp->next;
                    if (pp) pp->next = cur;
                }
                free(prev);
                ir_list->count--;
                prev = cur;
                rewrites++;

            } else if (is_free && prev && prev->opcode == IR_PARAM) {
                /* Rewrite free(ptr) → call __freelist_free with ptr in rdi. */
                strncpy(cur->arg1, "__freelist_free", sizeof(cur->arg1) - 1);
                /* arg2 already holds "1" (num args) */
                rewrites++;
            }
        }

        prev = cur;
        cur  = next;
    }

    return rewrites;
}
