#============================================================================
# Keep a pointer to the main scheduler context.  This variable should be
# initialized to %rsp, which is done in the __sthread_start routine.
#
        .data
        .align 8
scheduler_context:      .quad   0


#============================================================================
# __sthread_switch is the main entry point for the thread scheduler.
# It has three parts:
#
#    1. Save the context of the current thread on the stack.
#       The context includes all of the integer registers and RFLAGS.
#
#    2. Call __sthread_scheduler (the C scheduler function), passing the
#       context as an argument.  The scheduler stack *must* be restored by
#       setting %rsp to the scheduler_context before __sthread_scheduler is
#       called.
#
#    3. __sthread_scheduler will return the context of a new thread.
#       Restore the context, and return to the thread.
#
        .text
        .align 8
        .globl __sthread_switch
__sthread_switch:

        # Save the process state onto its stack; we want to stick with using
        # quadwords so that our alignment is correct
        
        # save base pointer     
        pushq %rbp

        # save integer registers
        pushq %r15
        pushq %r14
        pushq %r13
        pushq %r12
        pushq %r11
        pushq %r10
        pushq %r9
        pushq %r8

        pushq %rdi
        pushq %rsi
        pushq %rdx
        pushq %rcx
        pushq %rbx
        pushq %rax

        # save flags
        pushfq

        # Call the high-level scheduler with the current context as an argument
        movq    %rsp, %rdi
        movq    scheduler_context, %rsp
        call    __sthread_scheduler


        # The scheduler will return a context to start.
        # Restore the context to resume the thread.
__sthread_restore:
        
        # since the context to start is returned in rax, we want to 
        # move to that stack
        movq    %rax, %rsp

        # restore registers in reverse order from earlier, since pop and push
        # go in opposite directions

        # restore flags
        popfq

        # restore integer registers
        popq %rax
        popq %rbx
        popq %rcx
        popq %rdx
        popq %rsi
        popq %rdi

        popq %r8
        popq %r9
        popq %r10
        popq %r11
        popq %r12
        popq %r13
        popq %r14
        popq %r15

        # restore program counter and stack pointers 
        popq %rbp

        ret


#============================================================================
# Initialize a process context, given:
#    1. the stack for the process
#    2. the function to start
#    3. its argument
# The context should be consistent with that saved in the __sthread_switch
# routine.
#
# A pointer to the newly initialized context is returned to the caller.
# (This is the thread's stack-pointer after its context has been set up.)
#
# This function is described in more detail in sthread.c.
#
#   This implements
#    __sthread_initialize_context(new_stack - DEFAULT_STACKSIZE, f, arg);
#     
        .align 8
        .globl __sthread_initialize_context
__sthread_initialize_context:

        # eventually, after popping all the registers in __sthread_restore, 
        # we want to have f at the top of the stack and its arg in rdi,
        # so that we execute f

        # afterwards we want $__sthread_finish to be at the top of the stack

        # since the end of the stack is stored in %rdi when we call this, 
        # we want the stack to ultimately look like this
  #       ------------------------
  #      | $__s_thread_finish     |
  #      |           f            |
  #      |    (registers, where)  |
  #      |   arg is in %rdi       |
  #      |                        | 
  #       ------------------------   <- end of stack, which is at first in rdi

        # stack grows downwards
        # move $__sthread_finish into the nearest slot to the end of the stack
        movq   $__sthread_finish, (%rdi)

        # since we're using quadwords, each adjacent slot is spaced 8 apart 
        movq   %rsi, -8(%rdi) #f is stored in %rsi; move into stack

        # move the registers onto the stack in the opposite order as
        # in __sthread_restore

        # since we're initializing the context, set things we don't know
        # or don't need to 0  
        movq    $0, -16(%rdi) #set rbp
        movq    $0, -24(%rdi) #set r15
        movq    $0, -32(%rdi) #set r14
        movq    $0, -40(%rdi) #set r13
        movq    $0, -48(%rdi) #set r12
        movq    $0, -56(%rdi) #set r11
        movq    $0, -64(%rdi) #set r10
        movq    $0, -72(%rdi) #set r9
        movq    $0, -80(%rdi) #set r8

        # we want arg to be in %rdi when we call f, so we want it to be popped
        # into rdi in __sthread_restore
        movq    %rdx, -88(%rdi) #set rdi
        movq    $0, -96(%rdi) #set rsi
        movq    $0, -104(%rdi) #set rdx
        movq    $0, -112(%rdi) #set rcx
        movq    $0, -120(%rdi) #set rbx
        movq    $0, -128(%rdi) #set rax
        movq    $0, -136(%rdi) #set flags

        # set return value to address of context 
        lea    -136(%rdi), %rax
        ret
 

#============================================================================
# The start routine initializes the scheduler_context variable, and calls
# the __sthread_scheduler with a NULL context.
#
# The scheduler will return a context to resume.
#
        .align 8
        .globl __sthread_start
__sthread_start:
        # Remember the context
        movq    %rsp, scheduler_context

        # Call the scheduler with no context
        movl    $0, %edi  # Also clears upper 4 bytes of %rdi
        call    __sthread_scheduler

        # Restore the context returned by the scheduler
        jmp     __sthread_restore

