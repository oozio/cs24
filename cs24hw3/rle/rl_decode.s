.globl rl_decode

#============================================================================
# rl_decode:  decode RLE-encoded input into a malloc'd buffer
#
# Author:  Ben Bitdiddle (those guys are lucky I said I'd do this before
#          going on my vacation.  they never appreciate my work ethic!)
#
# Arguments to rl_decode are in these registers (following the System V AMD64
# ABI calling convention):
#
#      %rdi = pointer to buffer containing run-length encoded input data
#
#      %esi = int length of the run-length-encoded data in the buffer
#
#      %rdx = OUTPUT pointer to where the length of the decoded result
#             should be stored
#
# Return-value in %rax is the pointer to the malloc'd buffer containing
# the decoded data.
#
rl_decode:
        # No need for a frame pointer - we don't need a stack frame.

        # Save rbx since it is callee-save, and we definitely use it a lot.
        push    %rbx
        push    %rdx

        # First, figure out how much space is required to decode the data.
        # We do this by summing up the counts, which are in the odd memory
        # locations.

        mov     $0, %ecx                  # %ecx = loop variable (int)
        mov     $0, %ebx                  # %ebx = size required (also int)
      

        # Find-space while-loop starts here... 
        cmp     %esi, %ecx               # if ecx >= esi, finish
        jge     find_space_done

find_space_loop:
        xor     %eax, %eax
        add     (%rdi, %rcx), %eax         # Add in the count, then move # this adds the data to the counter and 
                                             #   sets eax to it
        and     $255, %eax               
        add     %eax, %ebx      
                                             
        add     $2, %rcx                  # forward to the next count! # changed 1 to 2 in this line

        cmp     %esi, %ecx                # if ecx < esi, do again
        jl      find_space_loop

find_space_done:

        # Allocate memory for the decoded data using malloc.
        # Number of bytes required goes in rdi, which we were using before.
        # Pointer to allocated memory will be returned in %rax.

        push    %rcx
        push    %rsi    
        push    %rdi                # save the data
        mov     %rbx, %rdi         # Number of bytes to allocate...
        call    malloc
        pop     %rdi
        pop     %rcx
        pop     %rsi
        
        pop     %rdx
        
        # Write the length of the decoded output to the output-variable
        mov     %ebx, (%rdx)      # store computed size into this location


        # Now, decode the data from the input buffer into the output buffer.
        mov     $0, %ecx          # Loop variable again (int)
        xor     %r10, %r10        # Index in output buffer

        # First comparison of decode while-loop here...
        cmp     %esi, %ecx          # while i < input length
        jge     decode_done

decode_loop:
        # Pull out the next [count][value] pair from the encoded data.
        mov     (%rdi, %rcx), %bh       # bh is the count of repetitions
        mov     1(%rdi, %rcx), %bl       # bl is the value to repeat

write_loop:

        mov     %bl, (%rax, %r10)
        
        add     $1, %r10
        dec     %bh
          
        jnz     write_loop

        add     $2, %ecx
        
        cmp     %esi, %ecx
        jl      decode_loop

decode_done:
        # Restore callee-save registers.
        pop %rbx

        # No stack frame to clean up.
        ret

