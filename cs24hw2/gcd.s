#=============================================================================
# The gcd(bigger,smaller) function recursively computes the greatest common 
# denominator of two integers with Euclid's method. 
.globl gcd
gcd:                            # bigger in %rdi, smaller in %rsi

        xor     %rdx, %rdx      # make rdx 0
        movq    %rdi, %rax      # put bigger in rax   
        movq    %rsi, %rbx      # put smaller in rbx

        testq   %rbx, %rbx      # check whether the smaller argument is 0
        jz      gcd_return      # if so, return the bigger number, since 0 is 
                                # divisible by everything, and gcd(0, 0) = 0

        jmp     gcd_continue    # go to loop start

gcd_continue:
        xor     %rdx, %rdx      # make rdx 0
        idivq   %rbx            # divide rax by rbx, with remainder in rdx and quotient in rax
        testq   %rdx, %rdx      # see whether the remainder was 0 - if so, zero flag is 1
        movq    %rbx, %rax      # otherwise, put 2nd number in 1st number
        movq    %rdx, %rbx      # and put the remainder in %rbx
        jz      gcd_return      # if the remainder was 0, return

        call    gcd_continue    # otherwise do the loop again

gcd_return:
        ret     # All done
        
