    .text
    # x = 10
    movq 10, %rax
    movq %rax, x
    # y = 20
    movq 20, %rax
    movq %rax, y
    # param x
    pushq x
    # param y
    pushq y
    #  = 
    movq , %rax
    movq %rax, 
    #  = 
    movq , %rax
    movq %rax, 
