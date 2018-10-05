#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "ffunc.h"


/* This function takes an array of single-precision floating point values,
 * and computes a sum in the order of the inputs.  Very simple.
 */
float fsum(FloatArray *floats) {
    float sum = 0;
    int i;

    for (i = 0; i < floats->count; i++)
        sum += floats->values[i];

    return sum;
}


float my_fsum(FloatArray *floats) {
    float sum[floats->count/2+1];

    int n;

    int i,j;
    n = (floats->count) / 2; //halfway point

    //base case; if the array has only 1 or 2 elements, return the sole element or a single sum
    if (floats->count == 1){
        return floats->values[0];
    }    
    else if (floats->count == 2){
        return floats->values[0] + floats->values[1];
    }
    
    //iterate through the array, assigning sequential pairwise sums to sum; eg if the array is 1234, sum would have the elements 1+2 and 3+4
    for (i = j = 0; i < n; i++, j = j + 2){
        sum[i] = floats->values[j] + floats->values[j + 1];
    }
    
    //fake recursion; while the array is longer than the base case (n>2 implies size>2), continuously add neighboring terms in sum together, so that each sum from before is collected in another pairwise addition, until the array is short enough
    while (n > 2){
        for (i = j = 0; i < n / 2; i++, j = j + 2){
            sum[i] = sum[j] + sum[j + 1];
        }
        if (n & 1){ 
            sum[n / 2] = sum[n - 1];
        }
        n = (n + 1) / 2;
    } 
    
    return sum[0] + sum[1]; //sum[0] and sum[1] are the sums of the two original halves of sum, so we can add and return
}




int main() {
    FloatArray floats;
    float sum1, sum2, sum3, my_sum;
    load_floats(stdin, &floats);
    printf("Loaded %d floats from stdin.\n", floats.count);

    /* Compute a sum, in the order of input. */
    sum1 = fsum(&floats);

    /* Use my_fsum() to compute a sum of the values.  Ideally, your
     * summation function won't be affected by the order of the input floats.
     */
    

    /* Compute a sum, in order of increasing magnitude. */
    sort_incmag(&floats);
    sum2 = fsum(&floats);

    /* Compute a sum, in order of decreasing magnitude. */
    sort_decmag(&floats);
    sum3 = fsum(&floats);

    /* %e prints the floating-point value in full precision,
     * using scientific notation.
     */
    printf("Sum computed in order of input:  %e\n", sum1);
    printf("Sum computed in order of increasing magnitude:  %e\n", sum2);
    printf("Sum computed in order of decreasing magnitude:  %e\n", sum3);

    my_sum = my_fsum(&floats);
    printf("My sum:  %e\n", my_sum);
  
    return 0;
}

