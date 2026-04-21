    movq  0(%rbp), %rax
    incq  %rax
    movq  %rax, 0(%rbp)
    jmp   .Lloop_-1
.Lloop_end_-1:
    movq  8(%rbp), %rax
    incq  %rax
    movq  %rax, 8(%rbp)
    jmp   .Lloop_-1
.Lloop_end_-1:
    movq  16(%rbp), %rax
    incq  %rax
    movq  %rax, 16(%rbp)
    jmp   .Lloop_-1
.Lloop_end_-1:
    movq  24(%rbp), %rax
    incq  %rax
    movq  %rax, 24(%rbp)
    jmp   .Lloop_-1
.Lloop_end_-1:
