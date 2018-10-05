# Definitions for the functions given as
#   int my_setjmp(my_jmp_buf buf);    and
#   void my_longjmp(my_jmp_buf buf, int ret);

.globl my_setjmp
my_setjmp: #buf passed in rdi, is an int array with JB_LENGTH=8 elements 

    # save all the callee-save registers in the buffer
    mov    %rbp, (%rdi)
    mov    %rbx, 8(%rdi)
    mov    %r12, 16(%rdi)
    mov    %r13, 24(%rdi)
    mov    %r14, 32(%rdi)
    mov    %r15, 40(%rdi)

    # save stack pointer and return address in the buffer
    mov    %rsp, 48(%rdi)
    mov    (%rsp), %rcx
    mov    %rcx, 56(%rdi)

    # returns 0 
    xor    %rax, %rax
    ret    

.globl my_longjmp
my_longjmp: #rdi is buf, rsi is ret
 
    # read values from the buffer in the same order that they 
    # were saved into corresponding registers
    mov    (%rdi), %rbp
    mov    8(%rdi), %rbx
    mov    16(%rdi), %r12
    mov    24(%rdi), %r13
    mov    32(%rdi), %r14
    mov    40(%rdi), %r15
    mov    48(%rdi), %rsp
    mov    56(%rdi), %rdx
    mov    %rdx, (%rsp)

    # set return value to ret
    mov    %rsi, %rax

    # check if ret is 0
    cmp    $0, %rsi
    # if not, finish and return
    jnz    lj_done

    # otherwise, return 1
    mov    $1, %rax

    lj_done:
        ret     
        