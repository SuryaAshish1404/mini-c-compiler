#ifndef DAG_H
#define DAG_H

#include "ast.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── DAGNode ────────────────────────────────────────────────────────────────
 *
 * One node in the value-number DAG.  Expression nodes (BINARY_OP,
 * UNARY_OP, NUMBER, FLOAT_NUMBER, IDENTIFIER, STRING, tensor ops) are
 * deduplicated via a hash map keyed on (type, op, child_ids, leaf_value).
 * Statement nodes are always unique (side effects).
 *
 * Child slots use -1 to mean "absent".
 * ─────────────────────────────────────────────────────────────────────────── */
typedef struct DAGNode {
    int         id;
    ASTNodeType type;
    OperatorType op;

    /* ── named child slots ── */
    int left_id;
    int right_id;
    int cond_id;
    int then_id;
    int else_id;
    int init_id;
    int update_id;
    int body_id;

    /* ── variable-arity children (list nodes) ── */
    int *child_ids;
    int  child_count;
    int  child_cap;

    /* ── leaf payload ── */
    char   name[64];
    int    int_value;
    double float_value;
    char   string_value[256];
    char   data_type[32];

    /* ── DAG analysis metadata ── */
    int ref_count;   /* in-degree: #other DAGNodes whose child slot names this */
    int is_root;     /* 1 = top-level node; immune to dead-node pruning        */
    int is_folded;   /* 1 = produced by constant folding (value was computed)  */
    int is_dead;     /* 1 = ref_count==0 && !is_root  (prunable)               */
} DAGNode;

/* ── DAG ─────────────────────────────────────────────────────────────────── */
typedef struct DAG {
    DAGNode *pool;   /* flat array; indexed by DAGNode.id                      */
    int      count;
    int      cap;

    int root_id;     /* id of the top-level node (AST_PROGRAM or function)    */

    /* ── pass statistics ── */
    int fold_count;   /* binary/unary ops collapsed to a constant              */
    int dedup_count;  /* hash-map hits (shared subexpressions)                 */
    int dead_count;   /* nodes with in-degree 0 that are not the root          */
} DAG;

/* ── public API ─────────────────────────────────────────────────────────── */

/* Build a DAG from an existing AST (the AST is not modified). */
DAG  *build_dag(ASTNode *root);

/* Release all memory owned by the DAG (does NOT free the source AST). */
void  free_dag(DAG *dag);

/* Human-readable dump: one section per live node, dead nodes summarised. */
void  print_dag(DAG *dag);

/* One-line statistics banner. */
void  print_dag_stats(DAG *dag);

/* Graphviz DOT: shared edges are shown as fan-in, dead nodes are red/dashed,
 * folded constants are yellow, shared (ref>1) nodes have a bold border. */
void  print_dag_dot(DAG *dag, FILE *out);

#ifdef __cplusplus
}
#endif

#endif /* DAG_H */
