#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* main wrapper for gcd.s, to calculate gcd of two positive integers */
extern int gcd(int small, int big);

int main(int argc, char *argv[]) {

    // If the input is of an unexpected format, tell the user what to put in.
    if (argc != 3) {
        printf("Usage: ./factmain <positive integer> <positive integer>");
        return 1;
    }

    if (argv[1] <= 0) {
        printf("Usage: ./factmain <positive integer> <positive integer>");
        return 1;
    }
            
    // Read numbers from commandline
    int bigger = atoi(argv[1]);
    int smaller = atoi(argv[2]);
    
    // If the order of the two integers isn't (bigger, smaller), rearrange them.
    if (bigger < smaller) {
        int temp = bigger;
        bigger = smaller;
        smaller = temp;
    }
    
        
   // Print their gcd.
   printf("GCD  of %d and %d is %d \n", smaller, bigger, gcd(bigger,smaller) ); 
    
    
   return 0;
}
