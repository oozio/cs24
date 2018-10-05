/* This file contains x86-64 assembly-language implementations of three
 * basic, very common math operations.
 
 The common theme of these three implementations is that they all 
 avoid jumps, which takes more code and might be slower, since branch
 predictors can't always be correct.
 */

        .text

/*====================================================================
 * int f1(int x, int y). f1 finds the minimum of x and y.
 */
.globl f1
f1:
        movl    %edi, %edx // copy what's in %edi to %edx; call it x
        movl    %esi, %eax // copy what's in %esi to %eax; call it y
        cmpl    %edx, %eax // compare x and y
        cmovg   %edx, %eax // if y>x, copy x to y 
        ret                // return y


/*====================================================================
 * int f2(int x). f2 finds the absolute value of x. 
 */
.globl f2
f2:
        movl    %edi, %eax // copy what's in %edi to %eax; call it x
        movl    %eax, %edx // copy x to %edx; let's call  it x'
        sarl    $31, %edx  // arithmetic shift x' right 31 times, 
                           // but preserving the sign; if x'=x were
                           // positive, then this is 0, and if it 
                           // were negative, then this is all 1
        xorl    %edx, %eax // compares x's bits with x''s bits; sets 
                           // bits in x to 0 if they are and to 1 if 
                           // they're not- finds 2's complement of x
        subl    %edx, %eax // x = x - x'; recovers original if x' were
                           // originally positive, inverts sign if 
                           // it were originally negative
        ret                // return x


/*====================================================================
 * int f3(int x). f3 tells you sign of x 
 */
.globl f3
f3:
        movl    %edi, %edx // copy what's in %edi to %edx; call it x
        movl    %edx, %eax // copy x to %eax; call it x'
        sarl    $31, %eax  // arithmetic shift x' right 31 times, 
                           // but preserving the sign; if x'=x were
                           // positive, then this is 0, and if it 
                           // were negative, then this is all 1 (-1)
        testl   %edx, %edx // sets flag to 0 if x is 0, and 1 if
                           // otherwise
        movl    $1, %edx   // copy the value 1 to x
        cmovg   %edx, %eax // if x was not 0, then copy x to x'
        ret                // return x'

