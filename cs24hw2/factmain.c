#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// main wrapper for fact.s, which finds the factorial of a positive number. 

extern int fact(int n);

int main(int argc, char *argv[]) {

    // If the input is of an unexpected format, tell the user what to put in.
    if (argc > 2) {
        printf("Usage: ./factmain <positive integer>");
        return 0;
    }
    if (argv[1] <= 0) {
        printf("Usage: ./factmain <positive integer>");
        return 0;
    }
    
    // Read number from commandline
    int n = atoi(argv[1]);
    
    // Print factorial of number.
    printf("Factorial of %d is %d \n", n, fact(n)); 
    
    return 0;
}
