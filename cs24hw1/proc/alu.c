/*! \file
 *
 * This file contains definitions for an Arithmetic/Logic Unit of an
 * emulated processor.
 */


#include <stdio.h>
#include <stdlib.h>   /* malloc(), free() */
#include <string.h>   /* memset() */

#include "alu.h"
#include "instruction.h"


/*!
 * This function dynamically allocates and initializes the state for a new ALU
 * instance.  If allocation fails, the program is terminated.
 */
ALU * build_alu() {
    /* Try to allocate the ALU struct.  If this fails, report error then exit. */
    ALU *alu = malloc(sizeof(ALU));
    if (!alu) {
        fprintf(stderr, "Out of memory building an ALU!\n");
        exit(11);
    }

    /* Initialize all values in the ALU struct to 0. */
    memset(alu, 0, sizeof(ALU));
    return alu;
}


/*! This function frees the dynamically allocated ALU instance. */
void free_alu(ALU *alu) {
    free(alu);
}


/*!
 * This function implements the logic of the ALU.  It reads the inputs and
 * opcode, then sets the output accordingly.  Note that if the ALU does not
 * recognize the opcode, it should simply produce a zero result.
 */
void alu_eval(ALU *alu) {
    uint32_t A, B, aluop;
    uint32_t result;

    A = pin_read(alu->in1);
    B = pin_read(alu->in2);
    aluop = pin_read(alu->op);

    result = 0;

    switch (aluop) {
    
    case ALUOP_ADD:  //adds the two inputs
        result = A + B;
        break;

    case ALUOP_INV: //inverts every bit of the first input
        result = ~A;
        break;
    
    case ALUOP_SUB: //subtracts the second input from the first
        result = A - B;
        break;
        
    case ALUOP_AND: //takes bitwise and of two inputs
        result = A & B;
        break;
    
    case ALUOP_OR: //bitwise or of two inputs
        result = A | B;
        break;
    
    case ALUOP_XOR: //exclusive bitwise or of two inputs
        result = A ^ B;
        break;
     
    case ALUOP_INCR: //increments the first input by 1 
        result = A + 1;
        break;
       
    case ALUOP_SRA: //right arithmetic shift by 1; preserves sign
        if ((1 << 31) & A ){ //if the first bit is 1, the number is negative
             result = A >> 1 | (1 << 31); //then we shift and stick the sign back on afterwards
         }   
        else{//otherwise the first bit is 0, and the number is positive
            result = A >> 1 | (0 << 31); //we shift and stick the sign on afterwards
        }
        break;
    
    case ALUOP_SRL: //right logical shift, doesn't care about sign
        result = A >> 1;
        break;
    
    case ALUOP_SLA: //left arithmetic shift; the sign doesn't get eaten, so it's always preserved
        result = A << 1;
        break;
        
    case ALUOP_SLL: //left logical shift
        result = A << 1;
        break;
    
     
    }

    pin_set(alu->out, result);
}

