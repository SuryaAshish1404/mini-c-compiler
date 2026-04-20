#include "regalloc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* ===========================================================================
 *  Physical register table
 * =========================================================================== */

static const char *phys_names[RA_K] = {
    /* callee-saved 0-4 */
    "%rbx", "%r12", "%r13", "%r14", "%r15",
    /* caller-saved 5-12 */
    "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9", "%r10", "%r11"
};

/* Bitmask bit (SF_REG_*) for callee-saved colours 0-4. */
static const int callee_bits[RA_K] = {
    1<<0, 1<<1, 1<<2, 1<<3, 1<<4,  /* callee-saved */
    0,0,0,0,0,0,0,0                 /* caller-saved  */
};

const char *ra_phys_reg(int colour) {
    if (colour < 0 || colour >= RA_K) return "%rax";
    return phys_names[colour];
}

int ra_callee_mask(int colour) {
    if (colour < 0 || colour >= RA_K) return 0;
    return callee_bits[colour];
}

/* ===========================================================================
 *  VarMap
 * =========================================================================== */

int varmap_intern(VarMap *vm, const char *name) {
    int existing = varmap_find(vm, name);
    if (existing >= 0) return existing;
    if (vm->count >= RA_MAX_VARS) return -1;
    strncpy(vm->name[vm->count], name, 63);
    return vm->count++;
}

int varmap_find(const VarMap *vm, const char *name) {
    for (int i = 0; i < vm->count; i++)
        if (strcmp(vm->name[i], name) == 0) return i;
    return -1;
}

/* ===========================================================================
 *  Interference graph
 * =========================================================================== */

void ig_add_edge(IGraph *g, int u, int v) {
    if (u == v) return;
    if (!BS_TEST(g->adj[u], v)) { BS_SET(g->adj[u], v); g->degree[u]++; }
    if (!BS_TEST(g->adj[v], u)) { BS_SET(g->adj[v], u); g->degree[v]++; }
}

/* ===========================================================================
 *  Union-Find coalescing map
 * =========================================================================== */

static void cm_init(CoalesceMap *cm, int n) {
    for (int i = 0; i < n; i++) cm->parent[i] = i;
}

int cm_find(CoalesceMap *cm, int x) {
    while (cm->parent[x] != x) {
        cm->parent[x] = cm->parent[cm->parent[x]]; /* path compression */
        x = cm->parent[x];
    }
    return x;
}

void cm_union(CoalesceMap *cm, int x, int y) {
    x = cm_find(cm, x);
    y = cm_find(cm, y);
    if (x != y) cm->parent[y] = x;
}

/* ===========================================================================
 *  Helpers: classify an IR operand
 * =========================================================================== */

static int is_virtual(const char *s) {
    if (!s || !s[0]) return 0;
    if (s[0] == '%') return 0;                          /* physical reg */
    if (s[0] == 'L' && isdigit((unsigned char)s[1])) return 0; /* label */
    if (isdigit((unsigned char)s[0]) || s[0] == '-') return 0; /* const */
    return 1;
}

/* ===========================================================================
 *  Phase 1 — Build VarMap + use/def sets per basic block
 * =========================================================================== */

static void build_use_def(RAResult *r, CFG *cfg) {
    for (int b = 0; b < cfg->block_count; b++) {
        BasicBlock *bb = &cfg->blocks[b];
        BlockLive  *bl = &r->live[b];
        BS_ZERO(bl->use); BS_ZERO(bl->def);
        BS_ZERO(bl->live_in); BS_ZERO(bl->live_out);

        for (int i = 0; i < bb->instr_count; i++) {
            IR *ir = bb->instrs[i];

            /* operands used before being defined in this block */
            const char *uses[2] = { ir->arg1, ir->arg2 };
            for (int u = 0; u < 2; u++) {
                if (!is_virtual(uses[u])) continue;
                int idx = varmap_intern(&r->vars, uses[u]);
                if (idx < 0) continue;
                if (!BS_TEST(r->live[b].def, idx)) /* not yet def'd in block */
                    BS_SET(r->live[b].use, idx);
            }

            /* result is defined */
            if (is_virtual(ir->result)) {
                int idx = varmap_intern(&r->vars, ir->result);
                if (idx >= 0) BS_SET(r->live[b].def, idx);
            }
        }
    }
}

/* ===========================================================================
 *  Phase 2 — Backward liveness dataflow
 *
 *  live_out[B] = ∪ live_in[S]   for each successor S of B
 *  live_in[B]  = use[B]  ∪  (live_out[B] − def[B])
 *
 *  Iterate until no live_in changes (guaranteed to terminate).
 * =========================================================================== */

static void compute_liveness(RAResult *r, CFG *cfg) {
    int changed = 1;
    while (changed) {
        changed = 0;
        /* iterate in reverse RPO (approximate post-order) */
        for (int b = cfg->block_count - 1; b >= 0; b--) {
            BasicBlock *bb  = &cfg->blocks[b];
            BlockLive  *bl  = &r->live[b];

            /* live_out[b] = ∪ live_in[succ] */
            BS new_out;
            BS_ZERO(new_out);
            for (int s = 0; s < bb->succ_count; s++) {
                int sid = bb->succs[s];
                BS_OR(new_out, r->live[sid].live_in);
            }

            /* live_in[b] = use[b] ∪ (live_out[b] − def[b]) */
            BS new_in = new_out;
            BS_ANDNOT(new_in, bl->def);
            BS_OR(new_in, bl->use);

            if (!BS_EQ(new_in, bl->live_in) || !BS_EQ(new_out, bl->live_out)) {
                bl->live_in  = new_in;
                bl->live_out = new_out;
                changed = 1;
            }
        }
    }
}

/* ===========================================================================
 *  Phase 3 — Build interference graph
 *
 *  For each instruction that defines variable d:
 *    all variables live simultaneously with d at that point interfere with d.
 *  We simulate the live set backward through each block.
 * =========================================================================== */

static void build_interference(RAResult *r, CFG *cfg) {
    for (int b = 0; b < cfg->block_count; b++) {
        BasicBlock *bb = &cfg->blocks[b];

        /* Start from live_out of this block and walk backwards. */
        BS live = r->live[b].live_out;

        for (int i = bb->instr_count - 1; i >= 0; i--) {
            IR *ir = bb->instrs[i];

            /* Defined variable */
            int d = -1;
            if (is_virtual(ir->result)) {
                d = varmap_find(&r->vars, ir->result);
                if (d >= 0) {
                    /* d interferes with everything live at this point */
                    for (int v = 0; v < r->vars.count; v++)
                        if (BS_TEST(live, v) && v != d)
                            ig_add_edge(&r->ig, d, v);
                    /* remove d from live (it is being defined here) */
                    BS_CLR(live, d);
                }
            }

            /* Add uses to live */
            const char *uses[2] = { ir->arg1, ir->arg2 };
            for (int u = 0; u < 2; u++) {
                if (!is_virtual(uses[u])) continue;
                int idx = varmap_find(&r->vars, uses[u]);
                if (idx >= 0) BS_SET(live, idx);
            }
        }
    }
}

/* ===========================================================================
 *  Phase 4 — Register coalescing (conservative Briggs criterion)
 *
 *  For each IR_ASSIGN "a = b" where a and b are both virtual:
 *    Merge a and b into one equivalence class unless coalescing would cause
 *    the merged node to have ≥ K high-degree neighbours (Briggs test).
 *    After merging, remove the copy instruction by marking it as a no-op.
 * =========================================================================== */

static void coalesce(RAResult *r, CFG *cfg) {
    cm_init(&r->cm, r->vars.count);

    for (int b = 0; b < cfg->block_count; b++) {
        BasicBlock *bb = &cfg->blocks[b];
        for (int i = 0; i < bb->instr_count; i++) {
            IR *ir = bb->instrs[i];
            if (ir->opcode != IR_ASSIGN) continue;
            if (!is_virtual(ir->result) || !is_virtual(ir->arg1)) continue;

            int d = varmap_find(&r->vars, ir->result);
            int s = varmap_find(&r->vars, ir->arg1);
            if (d < 0 || s < 0) continue;

            int rd = cm_find(&r->cm, d);
            int rs = cm_find(&r->cm, s);
            if (rd == rs) continue; /* already same class */

            /* They must not interfere directly */
            if (BS_TEST(r->ig.adj[rd], rs)) continue;

            /* Briggs criterion: count neighbours of (rd ∪ rs) with degree ≥ K */
            BS combined = r->ig.adj[rd];
            BS_OR(combined, r->ig.adj[rs]);
            int high = 0;
            for (int v = 0; v < r->vars.count; v++)
                if (BS_TEST(combined, v) && r->ig.degree[v] >= RA_K) high++;

            if (high < RA_K) {
                cm_union(&r->cm, rd, rs);
                r->n_coalesced++;
            }
        }
    }
}

/* ===========================================================================
 *  Phase 5 — Chaitin-Briggs graph colouring
 *
 *  Simplify phase: repeatedly pick a node with degree < K and push it onto
 *  a stack.  If no such node exists, spill the highest-degree node.
 *
 *  Select phase: pop nodes from the stack, assign the lowest available colour
 *  not used by any already-coloured neighbour.
 * =========================================================================== */

static void colour_graph(RAResult *r, int next_spill_offset) {
    int n = r->vars.count;
    if (n == 0) return;

    /* Work on the canonical (post-coalesce) representatives.
     * Build a working degree array (may be modified during simplify). */
    int *wdeg    = (int *)malloc(n * sizeof(int));
    int *removed = (int *)calloc(n, sizeof(int)); /* 1 = on stack */
    int *stack   = (int *)malloc(n * sizeof(int));
    int  sp      = 0;

    for (int i = 0; i < n; i++) {
        r->colour[i]     = -1;
        r->spill_slot[i] = 0;
        wdeg[i] = r->ig.degree[i];
    }

    int remaining = n;
    while (remaining > 0) {
        /* Find a node with wdeg < K (prefer nodes not yet removed) */
        int chosen = -1;
        for (int i = 0; i < n; i++) {
            if (removed[i]) continue;
            if (wdeg[i] < RA_K) { chosen = i; break; }
        }

        /* No node with degree < K: pick spill candidate (highest degree). */
        if (chosen < 0) {
            int max_deg = -1;
            for (int i = 0; i < n; i++) {
                if (removed[i]) continue;
                if (wdeg[i] > max_deg) { max_deg = wdeg[i]; chosen = i; }
            }
        }

        /* Push onto stack and remove from graph. */
        stack[sp++] = chosen;
        removed[chosen] = 1;
        remaining--;

        /* Update neighbours' working degrees. */
        for (int v = 0; v < n; v++) {
            if (!removed[v] && BS_TEST(r->ig.adj[chosen], v))
                wdeg[v]--;
        }
    }

    /* Select phase: pop, assign colour. */
    int spill_off = next_spill_offset;
    while (sp > 0) {
        int u = stack[--sp];
        /* Find colours used by already-coloured neighbours. */
        int used[RA_K];
        memset(used, 0, sizeof(used));
        for (int v = 0; v < n; v++) {
            if (BS_TEST(r->ig.adj[u], v) && r->colour[v] >= 0)
                used[r->colour[v]] = 1;
        }
        /* Assign first available colour. */
        int c = -1;
        for (int k = 0; k < RA_K; k++) {
            if (!used[k]) { c = k; break; }
        }
        if (c >= 0) {
            r->colour[u] = c;
            r->callee_mask |= ra_callee_mask(c);
        } else {
            /* Spill. */
            r->colour[u]     = -1;
            r->spill_slot[u] = spill_off;
            spill_off -= 8;
            r->n_spills++;
        }
    }

    free(wdeg); free(removed); free(stack);
}

/* ===========================================================================
 *  regalloc_run  — public entry point
 * =========================================================================== */
RAResult *regalloc_run(CFG *cfg, int next_spill_offset) {
    RAResult *r = (RAResult *)calloc(1, sizeof(RAResult));

    build_use_def(r, cfg);
    compute_liveness(r, cfg);
    build_interference(r, cfg);
    coalesce(r, cfg);
    colour_graph(r, next_spill_offset);

    return r;
}

void regalloc_free(RAResult *r) { free(r); }

/* ===========================================================================
 *  ra_operand  — translate virtual name → asm operand string
 * =========================================================================== */
const char *ra_operand(const RAResult *r, const char *name, char *buf) {
    if (!is_virtual(name)) {
        /* Already a constant or physical reg — return as-is. */
        strncpy(buf, name, 23); buf[23] = '\0';
        return buf;
    }
    int idx = varmap_find(&r->vars, name);
    if (idx < 0) {
        strncpy(buf, name, 23); buf[23] = '\0';
        return buf;
    }
    /* Follow coalesce map to canonical representative */
    int root = cm_find((CoalesceMap *)&r->cm, idx);
    int col  = r->colour[root];
    if (col >= 0) {
        strncpy(buf, ra_phys_reg(col), 23);
    } else {
        int off = r->spill_slot[root];
        snprintf(buf, 24, "%d(%%rbp)", off);
    }
    return buf;
}

/* ===========================================================================
 *  Print helpers
 * =========================================================================== */
void print_liveness(const RAResult *r) {
    printf("\n+==================================================+\n");
    printf("|              Liveness Analysis                   |\n");
    printf("+==================================================+\n");
    /* (Block count not stored in RAResult; caller can print per-block.) */
    printf("  Variables discovered: %d\n", r->vars.count);
    for (int i = 0; i < r->vars.count; i++)
        printf("    v%-3d = %s\n", i, r->vars.name[i]);
}

void print_interference(const RAResult *r) {
    printf("\n+==================================================+\n");
    printf("|            Interference Graph                    |\n");
    printf("+==================================================+\n");
    for (int i = 0; i < r->vars.count; i++) {
        printf("  v%-3d (%s) deg=%-3d  interferes: ",
               i, r->vars.name[i], r->ig.degree[i]);
        for (int j = 0; j < r->vars.count; j++)
            if (BS_TEST(r->ig.adj[i], j)) printf("v%d ", j);
        printf("\n");
    }
}

void print_colouring(const RAResult *r) {
    printf("\n+==================================================+\n");
    printf("|          Chaitin-Briggs Colouring                |\n");
    printf("+==================================================+\n");
    printf("  K = %d colours,  coalesced = %d,  spills = %d\n",
           RA_K, r->n_coalesced, r->n_spills);
    for (int i = 0; i < r->vars.count; i++) {
        int root = cm_find((CoalesceMap *)&r->cm, i);
        int col  = r->colour[root];
        if (col >= 0)
            printf("  %-20s → %s\n", r->vars.name[i], ra_phys_reg(col));
        else
            printf("  %-20s → SPILL %d(%%rbp)\n",
                   r->vars.name[i], r->spill_slot[root]);
    }
}
