#ifndef REGALLOC_H
#define REGALLOC_H

#include "ir.h"
#include "cfg.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ═══════════════════════════════════════════════════════════════════════════
 *  Physical register set (K colours for graph colouring)
 *
 *  We allocate into:
 *    Callee-saved: %rbx %r12 %r13 %r14 %r15              (indices 0-4)
 *    Caller-saved: %rdi %rsi %rdx %rcx %r8 %r9 %r10 %r11 (indices 5-12)
 *
 *  %rax is reserved as scratch for spill loads and arithmetic sequences.
 *  %rsp and %rbp are never allocated.
 *  Total K = 13 colours.
 * ═══════════════════════════════════════════════════════════════════════════ */
#define RA_K            13
#define RA_MAX_VARS    128    /* max virtual registers per function */
#define RA_MAX_BLOCKS  256    /* max CFG blocks */

/* colour index → physical register name */
const char *ra_phys_reg(int colour);

/* callee-saved bitmask bit for colour index c (for stack_frame integration) */
int ra_callee_mask(int colour);

/* ── Variable map ───────────────────────────────────────────────────────── */
typedef struct {
    char name[RA_MAX_VARS][64];
    int  count;
} VarMap;

int  varmap_intern(VarMap *vm, const char *name);  /* -1 on overflow */
int  varmap_find(const VarMap *vm, const char *name); /* -1 if absent */

/* ── 128-bit bitset (enough for RA_MAX_VARS = 128) ─────────────────────── */
typedef struct { unsigned long long w[2]; } BS;

#define BS_SET(b,i)   ((b).w[(i)>>6] |=  (1ULL<<((i)&63)))
#define BS_CLR(b,i)   ((b).w[(i)>>6] &= ~(1ULL<<((i)&63)))
#define BS_TEST(b,i)  (!!((b).w[(i)>>6] &  (1ULL<<((i)&63))))
#define BS_ZERO(b)    do{(b).w[0]=0;(b).w[1]=0;}while(0)
#define BS_OR(a,b)    do{(a).w[0]|=(b).w[0];(a).w[1]|=(b).w[1];}while(0)
#define BS_ANDNOT(a,b) do{(a).w[0]&=~(b).w[0];(a).w[1]&=~(b).w[1];}while(0)
#define BS_EQ(a,b)    ((a).w[0]==(b).w[0]&&(a).w[1]==(b).w[1])

/* ── Per-block liveness ─────────────────────────────────────────────────── */
typedef struct {
    BS use;       /* vars used before any def in block  */
    BS def;       /* vars defined in block              */
    BS live_in;   /* live at block entry                */
    BS live_out;  /* live at block exit                 */
} BlockLive;

/* ── Interference graph ─────────────────────────────────────────────────── */
typedef struct {
    BS  adj[RA_MAX_VARS];    /* adj[u] is the set of vars that interfere w/ u */
    int degree[RA_MAX_VARS];
} IGraph;

void ig_add_edge(IGraph *g, int u, int v); /* symmetric, no self-loops */

/* ── Coalescing map ─────────────────────────────────────────────────────── */
typedef struct {
    int parent[RA_MAX_VARS];  /* union-find: parent[i]=i means root */
} CoalesceMap;

int  cm_find(CoalesceMap *cm, int x);          /* path-compressed find */
void cm_union(CoalesceMap *cm, int x, int y);  /* union by rank (simple) */

/* ── Register allocation result ─────────────────────────────────────────── */
typedef struct {
    VarMap     vars;
    IGraph     ig;
    BlockLive  live[RA_MAX_BLOCKS];

    int        colour[RA_MAX_VARS];     /* physical reg index; -1 = spilled  */
    int        spill_slot[RA_MAX_VARS]; /* rbp offset when spilled, else 0   */
    int        callee_mask;             /* OR of ra_callee_mask() for colours */
    int        n_spills;
    int        n_coalesced;

    CoalesceMap cm;
} RAResult;

/* ── public API ─────────────────────────────────────────────────────────── */

/* Full pipeline: liveness → interference → coalescing → Chaitin-Briggs.
 * `next_spill_offset` is the first available negative rbp offset for spills
 * (typically -(frame_size_for_locals + 8)). */
RAResult *regalloc_run(CFG *cfg, int next_spill_offset);

/* Translate a virtual-register name to the operand string the codegen
 * should emit: either "%rbx" (physical reg) or "-8(%rbp)" (spill slot).
 * Returns `buf` (caller-provided, ≥ 24 bytes). */
const char *ra_operand(const RAResult *r, const char *name, char *buf);

void  print_liveness(const RAResult *r);
void  print_interference(const RAResult *r);
void  print_colouring(const RAResult *r);
void  regalloc_free(RAResult *r);

#ifdef __cplusplus
}
#endif

#endif /* REGALLOC_H */
