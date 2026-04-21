

    .section .bss
    .align 8
__ha_heap: .space 65536
__ha_off:  .space 8
__ha_fl:   .space 8

    .text

.globl __bump_alloc
__bump_alloc:
    # rdi = size; returns rax = ptr or 0
    addq  $7, %rdi
    andq  $-8, %rdi
    leaq  __ha_heap(%rip), %rax
    movq  __ha_off(%rip), %rcx
    movq  %rcx, %rdx
    addq  %rdi, %rdx
    cmpq  $65536, %rdx
    ja    .bump_oom
    addq  %rcx, %rax
    movq  %rdx, __ha_off(%rip)
    ret
.bump_oom:
    xorl  %eax, %eax
    ret

.globl __freelist_alloc
__freelist_alloc:
    pushq %rbx
    pushq %r12
    pushq %r13
    # align requested size → r12
    movq  %rdi, %r12
    addq  $7, %r12
    andq  $-8, %r12
    # walk free list
    movq  __ha_fl(%rip), %rbx
    testq %rbx, %rbx
    jz    .fl_newblock
.fl_walk:
    # check flags (offset +8): bit 0 = free
    movq  8(%rbx), %rax
    testq $1, %rax
    jz    .fl_next
    # check size (offset +0) >= r12
    movq  0(%rbx), %rax
    cmpq  %r12, %rax
    jae   .fl_found
.fl_next:
    movq  16(%rbx), %rbx
    testq %rbx, %rbx
    jnz   .fl_walk
.fl_newblock:
    # allocate from bump: size = r12 + 24 (header)
    movq  %r12, %rdi
    addq  $24, %rdi
    call  __bump_alloc
    testq %rax, %rax
    jz    .fl_oom
    movq  %rax, %rbx
    movq  %r12, 0(%rbx)
    movq  $0, 8(%rbx)
    # prepend to free list (mark next = old head)
    movq  __ha_fl(%rip), %rax
    movq  %rax, 16(%rbx)
    movq  %rbx, __ha_fl(%rip)
    jmp   .fl_ret
.fl_found:
    movq  $0, 8(%rbx)
.fl_ret:
    leaq  24(%rbx), %rax
    popq  %r13
    popq  %r12
    popq  %rbx
    ret
.fl_oom:
    xorl  %eax, %eax
    popq  %r13
    popq  %r12
    popq  %rbx
    ret

.globl __freelist_free
__freelist_free:
    pushq %rbx
    pushq %r12
    # rbx = header = rdi - 24
    leaq  -24(%rdi), %rbx
    orq   $1, 8(%rbx)
    # coalesce pass: walk list, merge adjacent free blocks
.coal_restart:
    movq  __ha_fl(%rip), %rbx
    testq %rbx, %rbx
    jz    .coal_done
.coal_walk:
    # is this block free?
    movq  8(%rbx), %rax
    testq $1, %rax
    jz    .coal_advance
    # compute address of block immediately after rbx in memory
    movq  0(%rbx), %r12
    leaq  24(%rbx, %r12, 1), %rax
    # check if rax == next free block in list (naïve: check next ptr)
    movq  16(%rbx), %rcx
    cmpq  %rcx, %rax
    jne   .coal_advance
    testq %rcx, %rcx
    jz    .coal_advance
    # merge: check both free
    movq  8(%rcx), %rax
    testq $1, %rax
    jz    .coal_advance
    # merge rcx into rbx
    movq  0(%rcx), %rax
    addq  $24, %rax
    addq  %rax, 0(%rbx)
    movq  16(%rcx), %rax
    movq  %rax, 16(%rbx)
    jmp   .coal_restart
.coal_advance:
    movq  16(%rbx), %rbx
    testq %rbx, %rbx
    jnz   .coal_walk
.coal_done:
    popq  %r12
    popq  %rbx
    ret

    .text
    movq  $5, %rax
    movq  %rax, %r12
    movq  $10, %rax
    movq  %rax, %rbx
