#include "cfg.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ═══════════════════════════════════════════════════════════════════════════
 *  Internal constants
 * ═══════════════════════════════════════════════════════════════════════════ */
#define INIT_INSTR_CAP 16
#define INIT_PRED_CAP   4
#define INIT_BLOCK_CAP 32
#define INIT_EDGE_CAP  64

/* ═══════════════════════════════════════════════════════════════════════════
 *  Helpers
 * ═══════════════════════════════════════════════════════════════════════════ */

static int is_conditional(IROpcode op) {
    return op == IR_IF_FALSE || op == IR_IF_TRUE;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  Block / edge allocation
 * ═══════════════════════════════════════════════════════════════════════════ */

static void bb_push_instr(BasicBlock *bb, IR *ir) {
    if (bb->instr_count == bb->instr_cap) {
        bb->instr_cap *= 2;
        bb->instrs = (IR **)realloc(bb->instrs, bb->instr_cap * sizeof(IR *));
    }
    bb->instrs[bb->instr_count++] = ir;
}

static void bb_push_pred(BasicBlock *bb, int pred) {
    /* de-duplicate */
    for (int i = 0; i < bb->pred_count; i++)
        if (bb->preds[i] == pred) return;
    if (bb->pred_count == bb->pred_cap) {
        bb->pred_cap *= 2;
        bb->preds = (int *)realloc(bb->preds, bb->pred_cap * sizeof(int));
    }
    bb->preds[bb->pred_count++] = pred;
}

static BasicBlock *cfg_new_block(CFG *cfg) {
    if (cfg->block_count == cfg->block_cap) {
        cfg->block_cap *= 2;
        cfg->blocks = (BasicBlock *)realloc(cfg->blocks,
                                            cfg->block_cap * sizeof(BasicBlock));
    }
    BasicBlock *bb = &cfg->blocks[cfg->block_count];
    memset(bb, 0, sizeof(BasicBlock));
    bb->id       = cfg->block_count++;
    snprintf(bb->name, sizeof(bb->name), "BB%d", bb->id);
    bb->instrs   = (IR **)malloc(INIT_INSTR_CAP * sizeof(IR *));
    bb->instr_cap = INIT_INSTR_CAP;
    bb->preds    = (int *)malloc(INIT_PRED_CAP * sizeof(int));
    bb->pred_cap  = INIT_PRED_CAP;
    bb->idom     = -1;
    bb->rpo_num  = -1;
    return bb;
}

/* Add directed edge src → dst (de-duplicates successor list). */
static void cfg_add_edge(CFG *cfg, int src, int dst) {
    BasicBlock *s = &cfg->blocks[src];

    /* de-duplicate successor */
    for (int i = 0; i < s->succ_count; i++)
        if (s->succs[i] == dst) return;

    if (s->succ_count < CFG_MAX_SUCCS)
        s->succs[s->succ_count++] = dst;

    bb_push_pred(&cfg->blocks[dst], src);

    /* record in edge list */
    if (cfg->edge_count == cfg->edge_cap) {
        cfg->edge_cap *= 2;
        cfg->edges = (CFGEdge *)realloc(cfg->edges,
                                        cfg->edge_cap * sizeof(CFGEdge));
    }
    cfg->edges[cfg->edge_count].src        = src;
    cfg->edges[cfg->edge_count].dst        = dst;
    cfg->edges[cfg->edge_count].is_back_edge = 0;
    cfg->edge_count++;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  Step 1 – leader identification
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *  An instruction at index i is a leader if:
 *    (a) i == 0  (first instruction)
 *    (b) it is the target label of any branch
 *    (c) it immediately follows a branch or return
 */
static void find_leaders(IR **instr, int n, int *leader) {
    leader[0] = 1;

    for (int i = 0; i < n; i++) {
        IROpcode op = instr[i]->opcode;

        if (op == IR_GOTO || op == IR_IF_FALSE || op == IR_IF_TRUE) {
            /* instruction after the branch is a leader */
            if (i + 1 < n) leader[i + 1] = 1;

            /* target of the branch is a leader */
            int target_label = instr[i]->label_num;
            for (int j = 0; j < n; j++) {
                if (instr[j]->opcode == IR_LABEL &&
                    instr[j]->label_num == target_label) {
                    leader[j] = 1;
                    break;
                }
            }
        } else if (op == IR_RETURN) {
            if (i + 1 < n) leader[i + 1] = 1;
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  Step 2 – RPO (Reverse Post-Order) via iterative DFS
 *
 *  Returns the count of reachable blocks and fills rpo_order[].
 * ═══════════════════════════════════════════════════════════════════════════ */

/* Iterative DFS post-order to avoid stack overflow on large CFGs. */
static int compute_rpo(CFG *cfg, int *rpo_order) {
    int n = cfg->block_count;
    int *visited   = (int *)calloc(n, sizeof(int));
    int *post      = (int *)malloc(n * sizeof(int));
    int *stack     = (int *)malloc(n * sizeof(int));
    int *child_idx = (int *)calloc(n, sizeof(int)); /* next successor to visit */
    int  post_cnt  = 0;
    int  sp        = 0;

    stack[sp++] = cfg->entry;
    visited[cfg->entry] = 1;

    while (sp > 0) {
        int cur = stack[sp - 1];
        BasicBlock *bb = &cfg->blocks[cur];

        int pushed = 0;
        while (child_idx[cur] < bb->succ_count) {
            int s = bb->succs[child_idx[cur]++];
            if (!visited[s]) {
                visited[s] = 1;
                stack[sp++] = s;
                pushed = 1;
                break;
            }
        }
        if (!pushed) {
            post[post_cnt++] = cur;
            sp--;
        }
    }

    /* RPO = reverse of post-order */
    for (int i = 0; i < post_cnt; i++) {
        int bid = post[post_cnt - 1 - i];
        rpo_order[i] = bid;
        cfg->blocks[bid].rpo_num = i;
    }
    /* unreachable blocks get rpo_num = n (∞) */
    for (int i = 0; i < n; i++)
        if (!visited[i]) cfg->blocks[i].rpo_num = n;

    free(visited);
    free(post);
    free(stack);
    free(child_idx);
    return post_cnt; /* reachable block count */
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  Step 3 – Dominator tree  (Cooper, Harvey, Kennedy 2001)
 *
 *  idom_rpo[i]  = RPO index of the immediate dominator of the block whose
 *                 RPO index is i.
 * ═══════════════════════════════════════════════════════════════════════════ */

/* Walk up the dominator tree until both fingers meet. */
static int intersect(const int *idom_rpo, int a, int b) {
    while (a != b) {
        while (a > b) a = idom_rpo[a];
        while (b > a) b = idom_rpo[b];
    }
    return a;
}

static void compute_dominators(CFG *cfg, const int *rpo_order, int rpo_count) {
    /* idom_rpo[i]: RPO number of idom for the i-th RPO block; -1 = undefined */
    int *idom_rpo = (int *)malloc(rpo_count * sizeof(int));
    for (int i = 0; i < rpo_count; i++) idom_rpo[i] = -1;
    idom_rpo[0] = 0;  /* entry dominates itself */

    int changed = 1;
    while (changed) {
        changed = 0;
        /* process in RPO order, skip entry (index 0) */
        for (int i = 1; i < rpo_count; i++) {
            int b   = rpo_order[i];
            BasicBlock *bb = &cfg->blocks[b];
            int new_idom = -1;

            for (int p = 0; p < bb->pred_count; p++) {
                int pred     = bb->preds[p];
                int pred_rpo = cfg->blocks[pred].rpo_num;
                if (pred_rpo >= rpo_count) continue;   /* unreachable pred */
                if (idom_rpo[pred_rpo] == -1)  continue;   /* not yet computed */

                new_idom = (new_idom == -1) ? pred_rpo
                                            : intersect(idom_rpo, new_idom, pred_rpo);
            }

            if (new_idom != -1 && idom_rpo[i] != new_idom) {
                idom_rpo[i] = new_idom;
                changed = 1;
            }
        }
    }

    /* Write idom back as block IDs */
    for (int i = 0; i < rpo_count; i++) {
        int b = rpo_order[i];
        if (i == 0) {
            cfg->blocks[b].idom = b;          /* entry: idom = self */
        } else {
            cfg->blocks[b].idom = (idom_rpo[i] >= 0)
                                  ? rpo_order[idom_rpo[i]]
                                  : -1;
        }
    }

    free(idom_rpo);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  Step 4 – Back-edge detection
 *
 *  An edge u → v is a back edge iff v dominates u in the dominator tree.
 *  Such a v is marked as a loop header.
 * ═══════════════════════════════════════════════════════════════════════════ */

/* Returns 1 if `dom` dominates `b` (walks up idom chain). */
static int dominates(CFG *cfg, int dom, int b) {
    int cur = b;
    while (1) {
        if (cur == dom) return 1;
        int up = cfg->blocks[cur].idom;
        if (up < 0 || up == cur) break;   /* reached root or unresolved */
        cur = up;
    }
    return 0;
}

static void detect_back_edges(CFG *cfg) {
    for (int i = 0; i < cfg->edge_count; i++) {
        int s = cfg->edges[i].src;
        int d = cfg->edges[i].dst;
        if (dominates(cfg, d, s)) {
            cfg->edges[i].is_back_edge = 1;
            cfg->blocks[d].is_loop_header = 1;
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  build_cfg  – public entry point
 * ═══════════════════════════════════════════════════════════════════════════ */
CFG *build_cfg(IRList *ir_list) {
    if (!ir_list || !ir_list->head || ir_list->count == 0) return NULL;

    /* ── flatten linked list → random-access array ── */
    int n = ir_list->count;
    IR **instr = (IR **)malloc(n * sizeof(IR *));
    {
        IR *cur = ir_list->head;
        for (int i = 0; i < n; i++) {
            if (!cur) {
                free(instr);
                return NULL;
            }
            instr[i] = cur;
            cur = cur->next;
        }
    }

    /* ── Step 1: leaders ── */
    int *leader = (int *)calloc(n, sizeof(int));
    find_leaders(instr, n, leader);

    /* ── Step 2: create basic blocks ── */
    CFG *cfg = (CFG *)malloc(sizeof(CFG));
    memset(cfg, 0, sizeof(CFG));
    cfg->block_cap = INIT_BLOCK_CAP;
    cfg->blocks    = (BasicBlock *)malloc(cfg->block_cap * sizeof(BasicBlock));
    cfg->edge_cap  = INIT_EDGE_CAP;
    cfg->edges     = (CFGEdge *)malloc(cfg->edge_cap * sizeof(CFGEdge));
    cfg->entry     = 0;

    /* instr_to_block[i] = id of block containing instruction i */
    int *instr_to_block = (int *)malloc(n * sizeof(int));

    int cur_bb_id = -1;
    for (int i = 0; i < n; i++) {
        if (leader[i]) {
            BasicBlock *bb = cfg_new_block(cfg);
            cur_bb_id = bb->id;
        }
        instr_to_block[i] = cur_bb_id;
        if (cur_bb_id >= 0) bb_push_instr(&cfg->blocks[cur_bb_id], instr[i]);
    }

    /* ── Build label_num → block_id map ── */
    int max_label = 0;
    for (int i = 0; i < n; i++)
        if (instr[i]->label_num > max_label) max_label = instr[i]->label_num;

    int *label_to_block = (int *)malloc((max_label + 2) * sizeof(int));
    memset(label_to_block, -1, (max_label + 2) * sizeof(int));

    for (int b = 0; b < cfg->block_count; b++) {
        BasicBlock *bb = &cfg->blocks[b];
        if (bb->instr_count > 0 && bb->instrs[0]->opcode == IR_LABEL) {
            int lnum = bb->instrs[0]->label_num;
            if (lnum >= 0 && lnum <= max_label)
                label_to_block[lnum] = b;
        }
    }

    /* ── Step 3: connect edges ── */
    for (int b = 0; b < cfg->block_count; b++) {
        BasicBlock *bb = &cfg->blocks[b];
        if (bb->instr_count == 0) continue;

        IR *last = bb->instrs[bb->instr_count - 1];

        if (last->opcode == IR_GOTO) {
            int lnum = last->label_num;
            int tgt  = (lnum >= 0 && lnum <= max_label) ? label_to_block[lnum] : -1;
            if (tgt >= 0) cfg_add_edge(cfg, b, tgt);

        } else if (is_conditional(last->opcode)) {
            /* taken branch */
            int lnum = last->label_num;
            int tgt  = (lnum >= 0 && lnum <= max_label) ? label_to_block[lnum] : -1;
            if (tgt >= 0) cfg_add_edge(cfg, b, tgt);
            /* fall-through */
            if (b + 1 < cfg->block_count) cfg_add_edge(cfg, b, b + 1);

        } else if (last->opcode == IR_RETURN) {
            /* terminal – no outgoing edge */

        } else {
            /* sequential fall-through */
            if (b + 1 < cfg->block_count) cfg_add_edge(cfg, b, b + 1);
        }
    }

    /* ── Step 4: RPO + dominator tree + back edges ── */
    int *rpo_order = (int *)malloc(cfg->block_count * sizeof(int));
    int  rpo_count = compute_rpo(cfg, rpo_order);
    compute_dominators(cfg, rpo_order, rpo_count);
    detect_back_edges(cfg);

    free(instr);
    free(leader);
    free(instr_to_block);
    free(label_to_block);
    free(rpo_order);

    return cfg;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  free_cfg
 * ═══════════════════════════════════════════════════════════════════════════ */
void free_cfg(CFG *cfg) {
    if (!cfg) return;
    for (int i = 0; i < cfg->block_count; i++) {
        free(cfg->blocks[i].instrs);
        free(cfg->blocks[i].preds);
    }
    free(cfg->blocks);
    free(cfg->edges);
    free(cfg);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  print_cfg  – dump every block with its instructions and edges
 * ═══════════════════════════════════════════════════════════════════════════ */
void print_cfg(CFG *cfg) {
    if (!cfg) return;
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║               Control Flow Graph  (%d blocks)                ║\n",
           cfg->block_count);
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    for (int b = 0; b < cfg->block_count; b++) {
        BasicBlock *bb = &cfg->blocks[b];

        printf("\n┌─ %s", bb->name);
        if (bb->is_loop_header) printf("  [LOOP HEADER]");
        printf("  (RPO=%d  idom=%s)\n",
               bb->rpo_num,
               bb->idom >= 0 ? cfg->blocks[bb->idom].name : "none");

        /* predecessors */
        printf("│  preds: ");
        if (bb->pred_count == 0) printf("(none)");
        for (int p = 0; p < bb->pred_count; p++)
            printf("%s%s", cfg->blocks[bb->preds[p]].name,
                   p < bb->pred_count - 1 ? ", " : "");
        printf("\n");

        /* instructions */
        printf("│  instructions:\n");
        for (int i = 0; i < bb->instr_count; i++) {
            printf("│    [%2d] ", i);
            print_ir(bb->instrs[i]);
        }

        /* successors */
        printf("│  succs: ");
        if (bb->succ_count == 0) printf("(none)");
        for (int s = 0; s < bb->succ_count; s++) {
            /* find edge to determine if it's a back edge */
            int back = 0;
            for (int e = 0; e < cfg->edge_count; e++)
                if (cfg->edges[e].src == b && cfg->edges[e].dst == bb->succs[s])
                    back = cfg->edges[e].is_back_edge;
            printf("%s%s%s",
                   cfg->blocks[bb->succs[s]].name,
                   back ? "(back)" : "",
                   s < bb->succ_count - 1 ? ", " : "");
        }
        printf("\n└─────────────────────────────────────\n");
    }

    printf("\nEdge summary  (%d edges):\n", cfg->edge_count);
    for (int e = 0; e < cfg->edge_count; e++) {
        printf("  %s → %s%s\n",
               cfg->blocks[cfg->edges[e].src].name,
               cfg->blocks[cfg->edges[e].dst].name,
               cfg->edges[e].is_back_edge ? "  ← BACK EDGE" : "");
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  print_dominator_tree  – print the idom relationship and tree structure
 * ═══════════════════════════════════════════════════════════════════════════ */

/* Recursively print dominator tree rooted at `root`, indented by `depth`. */
static void print_dom_node(CFG *cfg, int root, int depth) {
    for (int i = 0; i < depth; i++) printf("    ");
    printf("%s%s\n",
           cfg->blocks[root].name,
           cfg->blocks[root].is_loop_header ? "  [LOOP HEADER]" : "");

    /* find all blocks whose idom == root (children in dom tree) */
    for (int b = 0; b < cfg->block_count; b++) {
        if (b != root && cfg->blocks[b].idom == root)
            print_dom_node(cfg, b, depth + 1);
    }
}

void print_dominator_tree(CFG *cfg) {
    if (!cfg) return;
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                      Dominator Tree                         ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
    print_dom_node(cfg, cfg->entry, 0);

    printf("\nImmediate dominator table:\n");
    printf("  %-10s  %-10s\n", "Block", "idom");
    printf("  %-10s  %-10s\n", "─────", "────");
    for (int b = 0; b < cfg->block_count; b++) {
        printf("  %-10s  %s\n",
               cfg->blocks[b].name,
               cfg->blocks[b].idom >= 0
                   ? cfg->blocks[cfg->blocks[b].idom].name
                   : "(unreachable)");
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  print_loops  – identify natural loops via back edges
 *
 *  For a back edge (n → h), the natural loop body is:
 *    { h } ∪ { all blocks that can reach n going backwards, stopping at h }
 * ═══════════════════════════════════════════════════════════════════════════ */

static void collect_loop_body(CFG *cfg, int header, int tail,
                              int *in_loop, int *worklist) {
    int wl_top = 0;
    if (!in_loop[tail]) {
        in_loop[tail] = 1;
        worklist[wl_top++] = tail;
    }
    while (wl_top > 0) {
        int b = worklist[--wl_top];
        BasicBlock *bb = &cfg->blocks[b];
        for (int p = 0; p < bb->pred_count; p++) {
            int pred = bb->preds[p];
            if (pred != header && !in_loop[pred]) {
                in_loop[pred] = 1;
                worklist[wl_top++] = pred;
            }
        }
    }
}

void print_loops(CFG *cfg) {
    if (!cfg) return;
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                     Loop Identification                      ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    int loop_count = 0;
    int *in_loop  = (int *)malloc(cfg->block_count * sizeof(int));
    int *worklist = (int *)malloc(cfg->block_count * sizeof(int));

    for (int e = 0; e < cfg->edge_count; e++) {
        if (!cfg->edges[e].is_back_edge) continue;

        int tail   = cfg->edges[e].src;
        int header = cfg->edges[e].dst;
        loop_count++;

        printf("\nLoop #%d\n", loop_count);
        printf("  Back edge   : %s → %s\n",
               cfg->blocks[tail].name, cfg->blocks[header].name);
        printf("  Loop header : %s\n", cfg->blocks[header].name);

        /* collect body */
        memset(in_loop, 0, cfg->block_count * sizeof(int));
        in_loop[header] = 1;
        collect_loop_body(cfg, header, tail, in_loop, worklist);

        printf("  Body blocks :");
        for (int b = 0; b < cfg->block_count; b++)
            if (in_loop[b]) printf(" %s", cfg->blocks[b].name);
        printf("\n");
    }

    if (loop_count == 0)
        printf("\n  (no loops detected)\n");

    free(in_loop);
    free(worklist);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  print_cfg_dot  – Graphviz DOT output
 * ═══════════════════════════════════════════════════════════════════════════ */
void print_cfg_dot(CFG *cfg, FILE *out) {
    if (!cfg || !out) return;
    fprintf(out, "digraph CFG {\n");
    fprintf(out, "    node [shape=box fontname=\"Courier\"];\n");
    fprintf(out, "    rankdir=TB;\n\n");

    for (int b = 0; b < cfg->block_count; b++) {
        BasicBlock *bb = &cfg->blocks[b];
        /* node label = block name + first/last instr summary */
        fprintf(out, "    %s [label=\"%s", bb->name, bb->name);
        if (bb->is_loop_header) fprintf(out, "\\n[loop header]");
        if (bb->instr_count > 0) {
            /* print first instruction opcode */
            fprintf(out, "\\n%s", ir_opcode_to_string(bb->instrs[0]->opcode));
            if (bb->instr_count > 1)
                fprintf(out, "\\n...");
        }
        fprintf(out, "\"");
        if (b == cfg->entry)        fprintf(out, " style=filled fillcolor=lightblue");
        if (bb->is_loop_header)     fprintf(out, " style=filled fillcolor=lightyellow");
        fprintf(out, "];\n");
    }

    fprintf(out, "\n");
    for (int e = 0; e < cfg->edge_count; e++) {
        fprintf(out, "    %s -> %s",
                cfg->blocks[cfg->edges[e].src].name,
                cfg->blocks[cfg->edges[e].dst].name);
        if (cfg->edges[e].is_back_edge)
            fprintf(out, " [color=red label=\"back\"]");
        fprintf(out, ";\n");
    }

    fprintf(out, "}\n");
}
