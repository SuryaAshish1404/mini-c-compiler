#ifndef CFG_H
#define CFG_H

#include "ir.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── limits ────────────────────────────────────────────────────────────── */
#define CFG_MAX_SUCCS 4   /* max out-edges per block (if/goto = 2 max) */

/* ── BasicBlock ─────────────────────────────────────────────────────────── */
typedef struct BasicBlock {
    int  id;
    char name[16];        /* "BB0", "BB1", … */

    /* instructions (owned by the IRList, not copied) */
    IR **instrs;
    int  instr_count;
    int  instr_cap;

    /* edges */
    int succs[CFG_MAX_SUCCS];
    int succ_count;

    int *preds;
    int  pred_count;
    int  pred_cap;

    /* dominator tree */
    int idom;             /* block id of immediate dominator; entry → self */
    int rpo_num;          /* reverse-post-order index (entry = 0) */

    /* loop analysis */
    int is_loop_header;   /* 1 if at least one back edge targets this block */
} BasicBlock;

/* ── CFGEdge ────────────────────────────────────────────────────────────── */
typedef struct CFGEdge {
    int src;
    int dst;
    int is_back_edge;
} CFGEdge;

/* ── CFG ────────────────────────────────────────────────────────────────── */
typedef struct CFG {
    BasicBlock *blocks;
    int block_count;
    int block_cap;

    CFGEdge *edges;
    int edge_count;
    int edge_cap;

    int entry;   /* always 0 */
} CFG;

/* ── public API ─────────────────────────────────────────────────────────── */
CFG  *build_cfg(IRList *ir_list);
void  free_cfg(CFG *cfg);

void  print_cfg(CFG *cfg);
void  print_dominator_tree(CFG *cfg);
void  print_loops(CFG *cfg);
void  print_cfg_dot(CFG *cfg, FILE *out);   /* Graphviz DOT output */

#ifdef __cplusplus
}
#endif

#endif /* CFG_H */
