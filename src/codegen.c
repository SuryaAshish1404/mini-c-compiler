#include "codegen.h"
#include "heap_alloc.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ═══════════════════════════════════════════════════════════════════════════
 *  CodeGenContext
 * ═══════════════════════════════════════════════════════════════════════════ */
CodeGenContext *create_codegen_context(FILE *output) {
    CodeGenContext *ctx = (CodeGenContext *)calloc(1, sizeof(CodeGenContext));
    ctx->output = output;
    return ctx;
}

void free_codegen_context(CodeGenContext *ctx) {
    if (!ctx) return;
    free(ctx->current_function);
    regalloc_free(ctx->ra);
    free_frame_layout(ctx->frame);
    free(ctx);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  cg_operand  — resolve an IR operand to an assembly string
 *
 *  If register allocation has run, virtual registers get replaced with their
 *  physical register or spill-slot string.  Constants and physical regs pass
 *  through unchanged (with a '$' prefix for integer constants).
 * ═══════════════════════════════════════════════════════════════════════════ */
const char *cg_operand(CodeGenContext *ctx, const char *op, char *buf) {
    if (!op || !op[0]) { buf[0] = '\0'; return buf; }

    /* Already a physical register — use as-is. */
    if (op[0] == '%') { strncpy(buf, op, 23); buf[23] = '\0'; return buf; }

    /* Integer constant — add '$' prefix for AT&T syntax. */
    if (isdigit((unsigned char)op[0]) || op[0] == '-') {
        snprintf(buf, 24, "$%s", op);
        return buf;
    }

    /* Label reference — use as-is (for jumps). */
    if (op[0] == 'L' && isdigit((unsigned char)op[1])) {
        strncpy(buf, op, 23); buf[23] = '\0'; return buf;
    }

    /* Virtual register — ask register allocator. */
    if (ctx->ra) return ra_operand(ctx->ra, op, buf);

    /* Fallback: look up in frame layout. */
    if (ctx->frame) {
        int off = frame_slot_of(ctx->frame, op);
        if (off) { frame_slot_str(off, buf); return buf; }
    }

    /* Unknown — emit as-is (will not assemble correctly, but useful for debug). */
    strncpy(buf, op, 23); buf[23] = '\0';
    return buf;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  Prologue / epilogue delegated to stack_frame
 * ═══════════════════════════════════════════════════════════════════════════ */
void generate_function_prologue(CodeGenContext *ctx, const char *func_name) {
    if (ctx->frame)
        emit_prologue(ctx->output, func_name, ctx->frame);
    else {
        /* Fallback minimal prologue. */
        fprintf(ctx->output, "\n.globl %s\n%s:\n", func_name, func_name);
        fprintf(ctx->output, "    pushq %%rbp\n");
        fprintf(ctx->output, "    movq  %%rsp, %%rbp\n");
    }
}

void generate_function_epilogue(CodeGenContext *ctx) {
    if (ctx->frame) emit_epilogue(ctx->output, ctx->frame);
    else {
        fprintf(ctx->output, "    movq  %%rbp, %%rsp\n");
        fprintf(ctx->output, "    popq  %%rbp\n");
        fprintf(ctx->output, "    ret\n");
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  Instruction emitter
 * ═══════════════════════════════════════════════════════════════════════════ */
static void emit_instr(CodeGenContext *ctx, IR *ir) {
    FILE *out = ctx->output;
    char a1[24], a2[24], res[24];

    cg_operand(ctx, ir->arg1,   a1);
    cg_operand(ctx, ir->arg2,   a2);
    cg_operand(ctx, ir->result, res);

    switch (ir->opcode) {

    case IR_ASSIGN:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    movq  %%rax, %s\n", res);
        break;

    case IR_ADD:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    addq  %s, %%rax\n", a2);
        fprintf(out, "    movq  %%rax, %s\n", res);
        break;

    case IR_SUB:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    subq  %s, %%rax\n", a2);
        fprintf(out, "    movq  %%rax, %s\n", res);
        break;

    case IR_MUL:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    imulq %s, %%rax\n", a2);
        fprintf(out, "    movq  %%rax, %s\n", res);
        break;

    case IR_DIV:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    cqto\n");
        fprintf(out, "    idivq %s\n", a2);
        fprintf(out, "    movq  %%rax, %s\n", res);
        break;

    case IR_MOD:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    cqto\n");
        fprintf(out, "    idivq %s\n", a2);
        fprintf(out, "    movq  %%rdx, %s\n", res);
        break;

    case IR_EQ:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    cmpq  %s, %%rax\n", a2);
        fprintf(out, "    sete  %%al\n");
        fprintf(out, "    movzbq %%al, %%rax\n");
        fprintf(out, "    movq  %%rax, %s\n", res);
        break;

    case IR_NEQ:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    cmpq  %s, %%rax\n", a2);
        fprintf(out, "    setne %%al\n");
        fprintf(out, "    movzbq %%al, %%rax\n");
        fprintf(out, "    movq  %%rax, %s\n", res);
        break;

    case IR_LT:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    cmpq  %s, %%rax\n", a2);
        fprintf(out, "    setl  %%al\n");
        fprintf(out, "    movzbq %%al, %%rax\n");
        fprintf(out, "    movq  %%rax, %s\n", res);
        break;

    case IR_GT:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    cmpq  %s, %%rax\n", a2);
        fprintf(out, "    setg  %%al\n");
        fprintf(out, "    movzbq %%al, %%rax\n");
        fprintf(out, "    movq  %%rax, %s\n", res);
        break;

    case IR_LEQ:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    cmpq  %s, %%rax\n", a2);
        fprintf(out, "    setle %%al\n");
        fprintf(out, "    movzbq %%al, %%rax\n");
        fprintf(out, "    movq  %%rax, %s\n", res);
        break;

    case IR_GEQ:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    cmpq  %s, %%rax\n", a2);
        fprintf(out, "    setge %%al\n");
        fprintf(out, "    movzbq %%al, %%rax\n");
        fprintf(out, "    movq  %%rax, %s\n", res);
        break;

    case IR_AND:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    andq  %s, %%rax\n", a2);
        fprintf(out, "    movq  %%rax, %s\n", res);
        break;

    case IR_OR:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    orq   %s, %%rax\n", a2);
        fprintf(out, "    movq  %%rax, %s\n", res);
        break;

    case IR_NOT:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    testq %%rax, %%rax\n");
        fprintf(out, "    sete  %%al\n");
        fprintf(out, "    movzbq %%al, %%rax\n");
        fprintf(out, "    movq  %%rax, %s\n", res);
        break;

    case IR_NEG:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    negq  %%rax\n");
        fprintf(out, "    movq  %%rax, %s\n", res);
        break;

    case IR_LABEL:
        fprintf(out, "%s:\n", ir->result);
        break;

    case IR_GOTO:
        fprintf(out, "    jmp   %s\n", ir->result);
        break;

    case IR_IF_FALSE:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    testq %%rax, %%rax\n");
        fprintf(out, "    je    %s\n", ir->result);
        break;

    case IR_IF_TRUE:
        fprintf(out, "    movq  %s, %%rax\n", a1);
        fprintf(out, "    testq %%rax, %%rax\n");
        fprintf(out, "    jne   %s\n", ir->result);
        break;

    case IR_RETURN:
        /* Move return value into %rax, then epilogue. */
        if (ir->result[0]) {
            char rbuf[24];
            cg_operand(ctx, ir->result, rbuf);
            fprintf(out, "    movq  %s, %%rax\n", rbuf);
        }
        generate_function_epilogue(ctx);
        break;

    case IR_PARAM:
        /* Collected by CALL handler — nothing to emit standalone. */
        break;

    case IR_CALL: {
        /* The PARAM instructions immediately preceding this CALL already
         * carry the arguments; we batch them here.
         * Simple approach: the calling convention is already handled by
         * emit_call_args in a pre-pass; here we just emit the call and
         * store the result.  For the codegen we use the naive approach:
         * push each argument in the PARAMs, call, store rax → result. */
        fprintf(out, "    call  %s\n", ir->arg1);
        if (ir->result[0])
            fprintf(out, "    movq  %%rax, %s\n", res);
        break;
    }

    case IR_ALLOC: {
        /* IR_ALLOC result = alloc(arg1 bytes)
         * Lower to: mov arg1→rdi, call __bump_alloc or __freelist_alloc */
        const char *runtime = (strcmp(ir->arg2, ALLOC_FREELIST) == 0)
                              ? "__freelist_alloc" : "__bump_alloc";
        fprintf(out, "    movq  %s, %%rdi\n", a1);
        fprintf(out, "    call  %s\n", runtime);
        if (ir->result[0])
            fprintf(out, "    movq  %%rax, %s\n", res);
        break;
    }

    case IR_LOAD:
        fprintf(out, "    movq  %s, %%rax\n", a1);  /* base addr */
        fprintf(out, "    movq  (%%rax), %%rax\n");
        fprintf(out, "    movq  %%rax, %s\n", res);
        break;

    case IR_STORE:
        /* STORE array[index] = value: result=array, arg1=index, arg2=value */
        fprintf(out, "    movq  %s, %%rax\n", res);  /* array ptr */
        fprintf(out, "    movq  %s, %%rcx\n", a2);   /* value */
        fprintf(out, "    movq  %%rcx, (%%rax)\n");
        break;

    case IR_FOR_BEGIN: {
        int loop_id = ctx->loop_counter++;
        ctx->stack_offset -= 8;
        fprintf(out, "    movq  %s, %d(%%rbp)\n", a1, ctx->stack_offset);
        fprintf(out, ".Lloop_%d:\n", loop_id);
        fprintf(out, "    movq  %d(%%rbp), %%rax\n", ctx->stack_offset);
        fprintf(out, "    cmpq  %s, %%rax\n", a2);
        fprintf(out, "    jge   .Lloop_end_%d\n", loop_id);
        break;
    }

    case IR_FOR_END: {
        int loop_id = ctx->loop_counter - 1;
        fprintf(out, "    movq  %d(%%rbp), %%rax\n", ctx->stack_offset);
        fprintf(out, "    incq  %%rax\n");
        fprintf(out, "    movq  %%rax, %d(%%rbp)\n", ctx->stack_offset);
        fprintf(out, "    jmp   .Lloop_%d\n", loop_id);
        fprintf(out, ".Lloop_end_%d:\n", loop_id);
        ctx->stack_offset += 8;
        break;
    }

    default:
        fprintf(out, "    # unhandled opcode %d\n", (int)ir->opcode);
        break;
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  generate_assembly  — top-level driver
 *
 *  Pipeline:
 *    1. Lower alloc() calls → IR_ALLOC
 *    2. Emit runtime heap support code
 *    3. Build CFG
 *    4. Run register allocation (liveness → interference → Chaitin-Briggs)
 *    5. Build frame layout (using spill slots from regalloc)
 *    6. Detect function boundaries, emit prologue/epilogue around bodies
 *    7. Emit each instruction with operands resolved via cg_operand()
 * ═══════════════════════════════════════════════════════════════════════════ */
void generate_assembly(IRList *ir_list, FILE *output) {
    if (!ir_list || !output) return;

    /* 1. Lower alloc() calls. */
    lower_alloc_calls(ir_list);

    /* 2. Emit runtime support (heap allocators). */
    fprintf(output, "    .text\n");
    emit_heap_runtime(output);
    fprintf(output, "\n    .text\n");

    /* 3. Build CFG. */
    CFG *cfg = build_cfg(ir_list);

    /* 4. Register allocation.
     *    Use offset -8 as the starting spill offset so spill slots don't
     *    overlap with the callee-saved saves. The frame builder adjusts. */
    RAResult *ra = cfg ? regalloc_run(cfg, -8) : NULL;

    /* 5. Frame layout.
     *    Pass the callee mask from regalloc so the prologue pushes the right
     *    registers; pass n_spills as extra slots. */
    int callee_mask  = ra ? ra->callee_mask  : 0;
    int n_spills     = ra ? ra->n_spills     : 0;
    FrameLayout *fl  = build_frame_layout(ir_list, n_spills, callee_mask);

    /* Wire up context. */
    CodeGenContext *ctx = create_codegen_context(output);
    ctx->ra    = ra;
    ctx->frame = fl;

    /* 6 + 7. Walk IR, detect function entry/exit, emit instructions. */
    int in_function = 0;
    IR *ir = ir_list->head;
    while (ir) {
        if (ir->opcode == IR_LABEL) {
            /* Heuristic: a label that begins a new function (no current function
             * open, or preceded by a RETURN) starts a new function body.
             * We rely on the convention that function-entry labels are top-level
             * (not loop labels). */
            if (!in_function) {
                /* Treat the first label as the function name. */
                free(ctx->current_function);
                ctx->current_function = strdup(ir->result);
                generate_function_prologue(ctx, ir->result);
                in_function = 1;
                ir = ir->next;
                continue;
            }
        }

        if (ir->opcode == IR_RETURN) {
            emit_instr(ctx, ir);
            in_function = 0;
            ir = ir->next;
            continue;
        }

        emit_instr(ctx, ir);
        ir = ir->next;
    }

    /* Close any unclosed function. */
    if (in_function) generate_function_epilogue(ctx);

    /* Print regalloc diagnostics to stdout. */
    if (ra) {
        print_liveness(ra);
        print_interference(ra);
        print_colouring(ra);
    }

    if (cfg) free_cfg(cfg);
    free_codegen_context(ctx);
}
