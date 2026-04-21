
.globl main
main:
    pushq %rbp
    movq  %rsp, %rbp
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    subq  $144, %rsp
    movq  $0, %rax
    movq  %rax, %r14
    movq  $0, %rax
    movq  %rax, %r13
L0:
    movq  %r13, %rax
    cmpq  $5, %rax
    setl  %al
    movzbq %al, %rax
    movq  %rax, %rbx
    movq  %rbx, %rax
    testq %rax, %rax
    je    L1
    movq  $0, %rax
    movq  %rax, %r12
L2:
    movq  %r12, %rax
    cmpq  $4, %rax
    setl  %al
    movzbq %al, %rax
    movq  %rax, %rbx
    movq  %rbx, %rax
    testq %rax, %rax
    je    L3
    movq  %r13, %rax
    addq  %r12, %rax
    movq  %rax, %rbx
    movq  %rbx, %rax
    cqto
    idivq $2
    movq  %rdx, %rbx
    movq  %rbx, %rax
    cmpq  $0, %rax
    sete  %al
    movzbq %al, %rax
    movq  %rax, %rbx
    movq  %rbx, %rax
    testq %rax, %rax
    je    L4
    movq  %r14, %rax
    addq  %r13, %rax
    movq  %rax, %rbx
    movq  %rbx, %rax
    addq  %r12, %rax
    movq  %rax, %r14
    movq  %r14, %rax
    movq  %rax, %r14
    jmp   L5
L4:
    movq  %r14, %rax
    subq  %r13, %rax
    movq  %rax, %r14
    movq  %r14, %rax
    movq  %rax, %r14
L5:
    movq  %r12, %rax
    addq  $1, %rax
    movq  %rax, %r12
    movq  %r12, %rax
    movq  %rax, %r12
    jmp   L2
L3:
    movq  %r14, %rax
    cmpq  $10, %rax
    setg  %al
    movzbq %al, %rax
    movq  %rax, %rbx
    movq  %rbx, %rax
    testq %rax, %rax
    je    L6
    movq  %r14, %rax
    subq  $3, %rax
    movq  %rax, %r14
    movq  %r14, %rax
    movq  %rax, %r14
    jmp   L7
L6:
    movq  %r14, %rax
    addq  $2, %rax
    movq  %rax, %r14
    movq  %r14, %rax
    movq  %rax, %r14
L7:
    movq  %r13, %rax
    addq  $1, %rax
    movq  %rax, %r13
    movq  %r13, %rax
    movq  %rax, %r13
    jmp   L0
L1:
    movq  %r14, %rax
    addq  $144, %rsp
    popq  %r14
    popq  %r13
    popq  %r12
    popq  %rbx
    popq  %rbp
    ret
