#include "dag.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ═══════════════════════════════════════════════════════════════════════════
 *  Canonical key dimensions
 * ═══════════════════════════════════════════════════════════════════════════ */
#define KEY_MAX  320   /* longest possible key string (string literals)        */
#define HT_CAP  4096   /* must be a power of 2; load factor stays < 0.75      */
#define HT_MASK (HT_CAP - 1)

/* ═══════════════════════════════════════════════════════════════════════════
 *  Hash map  (open-addressing, linear probing, FNV-1a)
 *
 *  Keys and values are heap-allocated arrays; the map lives on the heap so
 *  it never blows the stack.
 * ═══════════════════════════════════════════════════════════════════════════ */
typedef struct {
    char (*keys)[KEY_MAX]; /* [HT_CAP][KEY_MAX] – one slot per entry          */
    int  *vals;            /* [HT_CAP]          – DAGNode id, or -1 (empty)   */
    int   used;
} HashMap;

static HashMap *hm_new(void) {
    HashMap *hm  = (HashMap *)malloc(sizeof(HashMap));
    hm->keys     = (char (*)[KEY_MAX])calloc(HT_CAP, KEY_MAX);
    hm->vals     = (int *)malloc(HT_CAP * sizeof(int));
    hm->used     = 0;
    for (int i = 0; i < HT_CAP; i++) hm->vals[i] = -1;
    return hm;
}

static void hm_free(HashMap *hm) {
    free(hm->keys);
    free(hm->vals);
    free(hm);
}

static unsigned int fnv1a(const char *s) {
    unsigned int h = 2166136261u;
    while (*s) h = (h ^ (unsigned char)*s++) * 16777619u;
    return h;
}

static int hm_get(HashMap *hm, const char *key) {
    unsigned int slot = fnv1a(key) & HT_MASK;
    while (hm->vals[slot] != -1) {
        if (strcmp(hm->keys[slot], key) == 0) return hm->vals[slot];
        slot = (slot + 1) & HT_MASK;
    }
    return -1;
}

static void hm_set(HashMap *hm, const char *key, int val) {
    unsigned int slot = fnv1a(key) & HT_MASK;
    while (hm->vals[slot] != -1 && strcmp(hm->keys[slot], key) != 0)
        slot = (slot + 1) & HT_MASK;
    if (hm->vals[slot] == -1) hm->used++;
    strncpy(hm->keys[slot], key, KEY_MAX - 1);
    hm->vals[slot] = val;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  Build context (threaded through recursive conversion)
 * ═══════════════════════════════════════════════════════════════════════════ */
typedef struct { DAG *dag; HashMap *hm; } Ctx;

/* ═══════════════════════════════════════════════════════════════════════════
 *  Pool management
 * ═══════════════════════════════════════════════════════════════════════════ */
static DAGNode *pool_new(DAG *dag) {
    if (dag->count == dag->cap) {
        dag->cap *= 2;
        dag->pool = (DAGNode *)realloc(dag->pool, dag->cap * sizeof(DAGNode));
    }
    DAGNode *n  = &dag->pool[dag->count];
    memset(n, 0, sizeof(DAGNode));
    n->id       = dag->count++;
    n->left_id  = n->right_id = n->cond_id  = -1;
    n->then_id  = n->else_id  = n->init_id  = -1;
    n->update_id = n->body_id = -1;
    return n;
}

static void push_child(DAGNode *n, int cid) {
    if (n->child_count == n->child_cap) {
        n->child_cap = n->child_cap ? n->child_cap * 2 : 4;
        n->child_ids = (int *)realloc(n->child_ids, n->child_cap * sizeof(int));
    }
    n->child_ids[n->child_count++] = cid;
}

/* Increment the in-degree of node `id` (no-op for -1). */
static void arc(DAG *dag, int id) {
    if (id >= 0) dag->pool[id].ref_count++;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  Canonical-key helpers
 * ═══════════════════════════════════════════════════════════════════════════ */
static const char *op_sym(OperatorType op) {
    switch (op) {
        case OP_ADD: return "+";  case OP_SUB: return "-";
        case OP_MUL: return "*";  case OP_DIV: return "/";
        case OP_MOD: return "%";  case OP_EQ:  return "==";
        case OP_NEQ: return "!="; case OP_LT:  return "<";
        case OP_GT:  return ">";  case OP_LEQ: return "<=";
        case OP_GEQ: return ">="; case OP_AND: return "&&";
        case OP_OR:  return "||"; case OP_NOT: return "!";
        case OP_NEG: return "~";  case OP_ASSIGN: return "=";
        default: return "?";
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  Constant-folding helpers  (integer arithmetic only)
 * ═══════════════════════════════════════════════════════════════════════════ */
static int is_int_const(DAG *dag, int id) {
    return id >= 0 && dag->pool[id].type == AST_NUMBER;
}

static int fold_binary(OperatorType op, int l, int r, int *out) {
    switch (op) {
        case OP_ADD: *out = l + r;                      return 1;
        case OP_SUB: *out = l - r;                      return 1;
        case OP_MUL: *out = l * r;                      return 1;
        case OP_DIV: if (!r) return 0; *out = l / r;   return 1;
        case OP_MOD: if (!r) return 0; *out = l % r;   return 1;
        case OP_EQ:  *out = l == r;                     return 1;
        case OP_NEQ: *out = l != r;                     return 1;
        case OP_LT:  *out = l < r;                      return 1;
        case OP_GT:  *out = l > r;                      return 1;
        case OP_LEQ: *out = l <= r;                     return 1;
        case OP_GEQ: *out = l >= r;                     return 1;
        case OP_AND: *out = l && r;                     return 1;
        case OP_OR:  *out = l || r;                     return 1;
        default: return 0;
    }
}

static int fold_unary(OperatorType op, int v, int *out) {
    switch (op) {
        case OP_NEG: *out = -v; return 1;
        case OP_NOT: *out = !v; return 1;
        default: return 0;
    }
}

/* Intern (lookup-or-create) an integer constant node.
 * Children of folded nodes are NOT referenced so they can become dead. */
static int intern_int(Ctx *ctx, int val, int folded) {
    char key[KEY_MAX];
    snprintf(key, sizeof(key), "N:%d", val);
    int ex = hm_get(ctx->hm, key);
    if (ex >= 0) {
        if (folded) ctx->dag->fold_count++;
        return ex;
    }
    DAGNode *n   = pool_new(ctx->dag);
    n->type      = AST_NUMBER;
    n->int_value = val;
    n->is_folded = folded;
    hm_set(ctx->hm, key, n->id);
    return n->id;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  Core recursive conversion:  ASTNode  →  DAGNode id
 *
 *  Expression nodes: deduplicated via hash map, constant-folded bottom-up.
 *  Statement nodes:  always freshly allocated (side effects prevent sharing).
 *
 *  ref-count rule: when we CREATE a new DAGNode that uses child C, we call
 *  arc(dag, C) to bump C's in-degree.  When we DEDUPLICATE (return existing
 *  id without creating a new node), we do NOT call arc on children — their
 *  counts were already bumped the first time the node was created.  The
 *  RETURNED id's ref-count is bumped by the caller (its own parent).
 * ═══════════════════════════════════════════════════════════════════════════ */
static int cvt(Ctx *ctx, ASTNode *ast);

static int cvt(Ctx *ctx, ASTNode *ast) {
    if (!ast) return -1;
    DAG *dag = ctx->dag;

    switch (ast->type) {

    /* ── integer literal ───────────────────────────────────────────────── */
    case AST_NUMBER: {
        char key[KEY_MAX];
        snprintf(key, sizeof(key), "N:%d", ast->int_value);
        int ex = hm_get(ctx->hm, key);
        if (ex >= 0) { dag->dedup_count++; return ex; }
        DAGNode *n   = pool_new(dag);
        n->type      = AST_NUMBER;
        n->int_value = ast->int_value;
        hm_set(ctx->hm, key, n->id);
        return n->id;
    }

    /* ── float literal ─────────────────────────────────────────────────── */
    case AST_FLOAT_NUMBER: {
        char key[KEY_MAX];
        /* %a gives an exact hex representation, avoiding rounding issues. */
        snprintf(key, sizeof(key), "F:%a", ast->float_value);
        int ex = hm_get(ctx->hm, key);
        if (ex >= 0) { dag->dedup_count++; return ex; }
        DAGNode *n     = pool_new(dag);
        n->type        = AST_FLOAT_NUMBER;
        n->float_value = ast->float_value;
        hm_set(ctx->hm, key, n->id);
        return n->id;
    }

    /* ── identifier ────────────────────────────────────────────────────── */
    case AST_IDENTIFIER: {
        char key[KEY_MAX];
        snprintf(key, sizeof(key), "I:%.60s", ast->name);
        int ex = hm_get(ctx->hm, key);
        if (ex >= 0) { dag->dedup_count++; return ex; }
        DAGNode *n = pool_new(dag);
        n->type    = AST_IDENTIFIER;
        strncpy(n->name, ast->name, sizeof(n->name) - 1);
        hm_set(ctx->hm, key, n->id);
        return n->id;
    }

    /* ── string literal ────────────────────────────────────────────────── */
    case AST_STRING: {
        char key[KEY_MAX];
        snprintf(key, sizeof(key), "S:%.250s", ast->string_value);
        int ex = hm_get(ctx->hm, key);
        if (ex >= 0) { dag->dedup_count++; return ex; }
        DAGNode *n = pool_new(dag);
        n->type    = AST_STRING;
        strncpy(n->string_value, ast->string_value, sizeof(n->string_value) - 1);
        hm_set(ctx->hm, key, n->id);
        return n->id;
    }

    /* ── binary expression ─────────────────────────────────────────────── */
    case AST_BINARY_OP: {
        int lid = cvt(ctx, ast->left);
        int rid = cvt(ctx, ast->right);

        /* Bottom-up constant folding: both operands are integer constants. */
        if (is_int_const(dag, lid) && is_int_const(dag, rid)) {
            int folded_val;
            if (fold_binary(ast->op,
                            dag->pool[lid].int_value,
                            dag->pool[rid].int_value,
                            &folded_val)) {
                dag->fold_count++;
                /* lid and rid are NOT arc'd — they may become dead nodes. */
                return intern_int(ctx, folded_val, 0);
            }
        }

        /* Deduplication via value-number key: (op, left_id, right_id). */
        char key[KEY_MAX];
        snprintf(key, sizeof(key), "B:%s:%d:%d", op_sym(ast->op), lid, rid);
        int ex = hm_get(ctx->hm, key);
        if (ex >= 0) { dag->dedup_count++; return ex; }

        DAGNode *n  = pool_new(dag);
        n->type     = AST_BINARY_OP;
        n->op       = ast->op;
        n->left_id  = lid;
        n->right_id = rid;
        arc(dag, lid);
        arc(dag, rid);
        hm_set(ctx->hm, key, n->id);
        return n->id;
    }

    /* ── unary expression ──────────────────────────────────────────────── */
    case AST_UNARY_OP: {
        int lid = cvt(ctx, ast->left);

        if (is_int_const(dag, lid)) {
            int folded_val;
            if (fold_unary(ast->op, dag->pool[lid].int_value, &folded_val)) {
                dag->fold_count++;
                return intern_int(ctx, folded_val, 0);
            }
        }

        char key[KEY_MAX];
        snprintf(key, sizeof(key), "U:%s:%d", op_sym(ast->op), lid);
        int ex = hm_get(ctx->hm, key);
        if (ex >= 0) { dag->dedup_count++; return ex; }

        DAGNode *n = pool_new(dag);
        n->type    = AST_UNARY_OP;
        n->op      = ast->op;
        n->left_id = lid;
        arc(dag, lid);
        hm_set(ctx->hm, key, n->id);
        return n->id;
    }

    /* ── tensor binary ops (pure, dedup like expression nodes) ─────────── */
    case AST_TENSOR_ADD:
    case AST_TENSOR_SUB:
    case AST_TENSOR_MUL: {
        int lid = cvt(ctx, ast->left);
        int rid = cvt(ctx, ast->right);
        char key[KEY_MAX];
        snprintf(key, sizeof(key), "TP%d:%d:%d", (int)ast->type, lid, rid);
        int ex = hm_get(ctx->hm, key);
        if (ex >= 0) { dag->dedup_count++; return ex; }
        DAGNode *n  = pool_new(dag);
        n->type     = ast->type;
        n->left_id  = lid;
        n->right_id = rid;
        arc(dag, lid);
        arc(dag, rid);
        hm_set(ctx->hm, key, n->id);
        return n->id;
    }

    /* ── tensor access (pure read, dedup on name + index) ──────────────── */
    case AST_TENSOR_ACCESS: {
        int lid = cvt(ctx, ast->left);
        int rid = cvt(ctx, ast->right);
        char key[KEY_MAX];
        snprintf(key, sizeof(key), "TA:%.60s:%d:%d", ast->name, lid, rid);
        int ex = hm_get(ctx->hm, key);
        if (ex >= 0) { dag->dedup_count++; return ex; }
        DAGNode *n  = pool_new(dag);
        n->type     = AST_TENSOR_ACCESS;
        n->left_id  = lid;
        n->right_id = rid;
        strncpy(n->name, ast->name, sizeof(n->name) - 1);
        arc(dag, lid);
        arc(dag, rid);
        hm_set(ctx->hm, key, n->id);
        return n->id;
    }

    /* ════════════════════════════════════════════════════════════════════
     *  Statement / declaration nodes: always fresh (side effects).
     *  Children are converted (expression parts get deduplicated),
     *  then arc'd to record the parent→child edge.
     * ════════════════════════════════════════════════════════════════════ */

    case AST_ASSIGNMENT: {
        int lid = cvt(ctx, ast->left);
        int rid = cvt(ctx, ast->right);
        DAGNode *n  = pool_new(dag);
        n->type     = AST_ASSIGNMENT;
        n->left_id  = lid;
        n->right_id = rid;
        arc(dag, lid); arc(dag, rid);
        return n->id;
    }

    case AST_VARIABLE_DECL: {
        int init_id = cvt(ctx, ast->right);
        DAGNode *n  = pool_new(dag);
        n->type     = AST_VARIABLE_DECL;
        n->right_id = init_id;
        strncpy(n->name,      ast->name,      sizeof(n->name)      - 1);
        strncpy(n->data_type, ast->data_type, sizeof(n->data_type) - 1);
        arc(dag, init_id);
        return n->id;
    }

    case AST_FUNCTION_DECL: {
        int params_id = cvt(ctx, ast->left);
        int body_id   = cvt(ctx, ast->body);
        DAGNode *n  = pool_new(dag);
        n->type     = AST_FUNCTION_DECL;
        n->left_id  = params_id;
        n->body_id  = body_id;
        strncpy(n->name,      ast->name,      sizeof(n->name)      - 1);
        strncpy(n->data_type, ast->data_type, sizeof(n->data_type) - 1);
        arc(dag, params_id); arc(dag, body_id);
        return n->id;
    }

    case AST_TENSOR_DECL: {
        DAGNode *n = pool_new(dag);
        n->type    = AST_TENSOR_DECL;
        strncpy(n->name, ast->name, sizeof(n->name) - 1);
        return n->id;
    }

    case AST_IF_STMT: {
        int cond_id = cvt(ctx, ast->condition);
        int then_id = cvt(ctx, ast->then_branch);
        int else_id = cvt(ctx, ast->else_branch);
        DAGNode *n  = pool_new(dag);
        n->type     = AST_IF_STMT;
        n->cond_id  = cond_id;
        n->then_id  = then_id;
        n->else_id  = else_id;
        arc(dag, cond_id); arc(dag, then_id); arc(dag, else_id);
        return n->id;
    }

    case AST_WHILE_STMT: {
        int cond_id = cvt(ctx, ast->condition);
        int body_id = cvt(ctx, ast->body);
        DAGNode *n  = pool_new(dag);
        n->type     = AST_WHILE_STMT;
        n->cond_id  = cond_id;
        n->body_id  = body_id;
        arc(dag, cond_id); arc(dag, body_id);
        return n->id;
    }

    case AST_FOR_STMT: {
        int init_id   = cvt(ctx, ast->init);
        int cond_id   = cvt(ctx, ast->condition);
        int update_id = cvt(ctx, ast->update);
        int body_id   = cvt(ctx, ast->body);
        DAGNode *n    = pool_new(dag);
        n->type       = AST_FOR_STMT;
        n->init_id    = init_id;
        n->cond_id    = cond_id;
        n->update_id  = update_id;
        n->body_id    = body_id;
        arc(dag, init_id); arc(dag, cond_id);
        arc(dag, update_id); arc(dag, body_id);
        return n->id;
    }

    case AST_RETURN_STMT: {
        int lid    = cvt(ctx, ast->left);
        DAGNode *n = pool_new(dag);
        n->type    = AST_RETURN_STMT;
        n->left_id = lid;
        arc(dag, lid);
        return n->id;
    }

    case AST_EXPR_STMT: {
        int lid    = cvt(ctx, ast->left);
        DAGNode *n = pool_new(dag);
        n->type    = AST_EXPR_STMT;
        n->left_id = lid;
        arc(dag, lid);
        return n->id;
    }

    /* Function calls are never deduplicated (observable side effects). */
    case AST_FUNCTION_CALL: {
        int args_id = cvt(ctx, ast->left);
        DAGNode *n  = pool_new(dag);
        n->type     = AST_FUNCTION_CALL;
        n->left_id  = args_id;
        strncpy(n->name, ast->name, sizeof(n->name) - 1);
        arc(dag, args_id);
        return n->id;
    }

    /* ── list / container nodes ─────────────────────────────────────────── */
    case AST_PROGRAM:
    case AST_DECLARATION:
    case AST_COMPOUND_STMT:
    case AST_PARAM_LIST:
    case AST_ARG_LIST:
    case AST_DECLARATION_LIST:
    case AST_STATEMENT_LIST: {
        DAGNode *n = pool_new(dag);
        n->type    = ast->type;
        /* some list nodes also use left/right */
        n->left_id  = cvt(ctx, ast->left);   arc(dag, n->left_id);
        n->right_id = cvt(ctx, ast->right);  arc(dag, n->right_id);
        for (int i = 0; i < ast->num_children; i++) {
            int cid = cvt(ctx, ast->children[i]);
            push_child(n, cid);
            arc(dag, cid);
        }
        return n->id;
    }

    default: {
        DAGNode *n = pool_new(dag);
        n->type    = ast->type;
        return n->id;
    }
    } /* switch */
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  build_dag — public entry point
 * ═══════════════════════════════════════════════════════════════════════════ */
DAG *build_dag(ASTNode *root) {
    if (!root) return NULL;

    DAG *dag   = (DAG *)calloc(1, sizeof(DAG));
    dag->cap   = 128;
    dag->pool  = (DAGNode *)malloc(dag->cap * sizeof(DAGNode));
    dag->root_id = -1;

    Ctx ctx;
    ctx.dag = dag;
    ctx.hm  = hm_new();

    int rid      = cvt(&ctx, root);
    dag->root_id = rid;
    if (rid >= 0) dag->pool[rid].is_root = 1;

    /* ── dead-node pass ──
     * A node with in-degree 0 that isn't the root is unreachable from any
     * parent.  This happens when:
     *   (a) a constant literal's only parent was a folded binary/unary op
     *       (the folded op never became a DAGNode so never called arc())
     *   (b) identical subexpressions: all duplicates after the first
     *       are handled by returning an existing id — the duplicate subtrees'
     *       leaf nodes were never arc'd by any new parent node.
     */
    for (int i = 0; i < dag->count; i++) {
        DAGNode *n = &dag->pool[i];
        if (n->ref_count == 0 && !n->is_root) {
            n->is_dead = 1;
            dag->dead_count++;
        }
    }

    hm_free(ctx.hm);
    return dag;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  free_dag
 * ═══════════════════════════════════════════════════════════════════════════ */
void free_dag(DAG *dag) {
    if (!dag) return;
    for (int i = 0; i < dag->count; i++)
        free(dag->pool[i].child_ids);
    free(dag->pool);
    free(dag);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  Printing helpers
 * ═══════════════════════════════════════════════════════════════════════════ */
static const char *node_label(ASTNodeType t) {
    switch (t) {
        case AST_PROGRAM:          return "Program";
        case AST_DECLARATION:      return "Declaration";
        case AST_VARIABLE_DECL:    return "VarDecl";
        case AST_FUNCTION_DECL:    return "FuncDecl";
        case AST_TENSOR_DECL:      return "TensorDecl";
        case AST_ASSIGNMENT:       return "Assign";
        case AST_BINARY_OP:        return "BinOp";
        case AST_UNARY_OP:         return "UnaryOp";
        case AST_IDENTIFIER:       return "Id";
        case AST_NUMBER:           return "Int";
        case AST_FLOAT_NUMBER:     return "Float";
        case AST_STRING:           return "Str";
        case AST_COMPOUND_STMT:    return "Compound";
        case AST_IF_STMT:          return "If";
        case AST_WHILE_STMT:       return "While";
        case AST_FOR_STMT:         return "For";
        case AST_RETURN_STMT:      return "Return";
        case AST_EXPR_STMT:        return "ExprStmt";
        case AST_FUNCTION_CALL:    return "Call";
        case AST_TENSOR_ACCESS:    return "TensorAccess";
        case AST_TENSOR_ADD:       return "TensorAdd";
        case AST_TENSOR_SUB:       return "TensorSub";
        case AST_TENSOR_MUL:       return "TensorMul";
        case AST_PARAM_LIST:       return "Params";
        case AST_ARG_LIST:         return "Args";
        case AST_DECLARATION_LIST: return "DeclList";
        case AST_STATEMENT_LIST:   return "StmtList";
        default:                   return "?";
    }
}

static void node_value_str(DAGNode *n, char *buf, int len) {
    switch (n->type) {
        case AST_NUMBER:
            snprintf(buf, len, "%d", n->int_value); break;
        case AST_FLOAT_NUMBER:
            snprintf(buf, len, "%g", n->float_value); break;
        case AST_IDENTIFIER:
        case AST_VARIABLE_DECL:
        case AST_FUNCTION_DECL:
        case AST_FUNCTION_CALL:
        case AST_TENSOR_DECL:
        case AST_TENSOR_ACCESS:
            snprintf(buf, len, "'%s'", n->name); break;
        case AST_STRING:
            snprintf(buf, len, "\"%.*s\"", len - 4, n->string_value); break;
        case AST_BINARY_OP:
        case AST_UNARY_OP:
            snprintf(buf, len, "%s", op_sym(n->op)); break;
        default:
            buf[0] = '\0'; break;
    }
}

/* Emit one line per non-absent edge from node n. */
static void print_edge(const char *label, int child_id, DAG *dag) {
    if (child_id < 0) return;
    DAGNode *c = &dag->pool[child_id];
    char val[64];
    node_value_str(c, val, sizeof(val));
    printf("│  %-8s → #%-4d [%s %s]%s\n",
           label, child_id, node_label(c->type), val,
           c->is_dead ? " DEAD" : "");
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  print_dag_stats
 * ═══════════════════════════════════════════════════════════════════════════ */
void print_dag_stats(DAG *dag) {
    if (!dag) return;
    int live = dag->count - dag->dead_count;
    printf("\n┌─ DAG statistics ───────────────────────────────────────────┐\n");
    printf("│  Total nodes in pool : %-6d                              │\n", dag->count);
    printf("│  Live nodes          : %-6d                              │\n", live);
    printf("│  Dead nodes (pruned) : %-6d  (zero in-degree, not root)  │\n", dag->dead_count);
    printf("│  Constant folds      : %-6d                              │\n", dag->fold_count);
    printf("│  Dedup hits (shared) : %-6d                              │\n", dag->dedup_count);
    printf("└────────────────────────────────────────────────────────────┘\n");
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  print_dag
 * ═══════════════════════════════════════════════════════════════════════════ */
void print_dag(DAG *dag) {
    if (!dag) return;

    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║           AST → DAG  (value-number + constant folding)       ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    /* Print live nodes */
    for (int i = 0; i < dag->count; i++) {
        DAGNode *n = &dag->pool[i];
        if (n->is_dead) continue;

        char val[128];
        node_value_str(n, val, sizeof(val));

        printf("\n┌─ #%-4d  %-14s  %s", n->id, node_label(n->type), val);
        if (n->is_root)   printf("  [ROOT]");
        if (n->is_folded) printf("  [FOLDED]");
        if (n->ref_count > 1) printf("  [shared×%d]", n->ref_count);
        printf("\n");
        printf("│  ref_count=%d\n", n->ref_count);

        print_edge("left",   n->left_id,   dag);
        print_edge("right",  n->right_id,  dag);
        print_edge("cond",   n->cond_id,   dag);
        print_edge("then",   n->then_id,   dag);
        print_edge("else",   n->else_id,   dag);
        print_edge("init",   n->init_id,   dag);
        print_edge("update", n->update_id, dag);
        print_edge("body",   n->body_id,   dag);
        for (int c = 0; c < n->child_count; c++) {
            char lbl[16];
            snprintf(lbl, sizeof(lbl), "[%d]", c);
            print_edge(lbl, n->child_ids[c], dag);
        }
        printf("└─────────────────────────────────────────────────────────\n");
    }

    /* Summarise dead nodes */
    if (dag->dead_count > 0) {
        printf("\nDead nodes (in-degree 0, not root) — prunable:\n");
        for (int i = 0; i < dag->count; i++) {
            DAGNode *n = &dag->pool[i];
            if (!n->is_dead) continue;
            char val[64];
            node_value_str(n, val, sizeof(val));
            printf("  #%-4d  %-14s  %s\n", n->id, node_label(n->type), val);
        }
    }

    print_dag_stats(dag);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  print_dag_dot  — Graphviz DOT output
 *
 *  Visual encoding:
 *    lightblue  = root node
 *    lightyellow = constant-folded result
 *    lightgreen  = shared (ref_count > 1), deduplicated
 *    red/dashed  = dead node (in-degree 0, not root)
 *    bold border = ref_count > 1 (shared expression)
 * ═══════════════════════════════════════════════════════════════════════════ */
void print_dag_dot(DAG *dag, FILE *out) {
    if (!dag || !out) return;

    fprintf(out, "digraph DAG {\n");
    fprintf(out, "    node [shape=box fontname=\"Courier\" fontsize=10];\n");
    fprintf(out, "    rankdir=TB;\n\n");

    /* nodes */
    for (int i = 0; i < dag->count; i++) {
        DAGNode *n = &dag->pool[i];
        char val[80];
        node_value_str(n, val, sizeof(val));

        fprintf(out, "    N%d [label=\"#%d %s", n->id, n->id, node_label(n->type));
        if (val[0]) fprintf(out, "\\n%s", val);
        if (n->ref_count > 1) fprintf(out, "\\nref×%d", n->ref_count);
        fprintf(out, "\"");

        if (n->is_dead) {
            fprintf(out, " style=\"dashed\" color=red fontcolor=red");
        } else if (n->is_root) {
            fprintf(out, " style=filled fillcolor=lightblue");
        } else if (n->is_folded) {
            fprintf(out, " style=filled fillcolor=lightyellow");
        } else if (n->ref_count > 1) {
            fprintf(out, " style=\"filled,bold\" fillcolor=lightgreen");
        }
        fprintf(out, "];\n");
    }

    fprintf(out, "\n");

    /* helper macro-like lambda via a local function pattern */
    #define EMIT_EDGE(src_id, child_id, lbl) \
        if ((child_id) >= 0) \
            fprintf(out, "    N%d -> N%d [label=\"%s\"%s];\n", \
                    (src_id), (child_id), (lbl), \
                    dag->pool[(child_id)].is_dead ? " style=dashed color=red" : "")

    for (int i = 0; i < dag->count; i++) {
        DAGNode *n = &dag->pool[i];
        EMIT_EDGE(n->id, n->left_id,   "left");
        EMIT_EDGE(n->id, n->right_id,  "right");
        EMIT_EDGE(n->id, n->cond_id,   "cond");
        EMIT_EDGE(n->id, n->then_id,   "then");
        EMIT_EDGE(n->id, n->else_id,   "else");
        EMIT_EDGE(n->id, n->init_id,   "init");
        EMIT_EDGE(n->id, n->update_id, "update");
        EMIT_EDGE(n->id, n->body_id,   "body");
        for (int c = 0; c < n->child_count; c++) {
            char lbl[16];
            snprintf(lbl, sizeof(lbl), "[%d]", c);
            EMIT_EDGE(n->id, n->child_ids[c], lbl);
        }
    }

    #undef EMIT_EDGE

    fprintf(out, "}\n");
}
