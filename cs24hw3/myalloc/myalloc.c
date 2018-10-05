/*! \file
 * Implementation of a simple memory allocator.  The allocator manages a small
 * pool of memory, provides memory chunks on request, and reintegrates freed
 * memory back into the pool.
 *
 * Adapted from Andre DeHon's CS24 2004, 2006 material.
 * Copyright (C) California Institute of Technology, 2004-2010.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>

#include "myalloc.h"


// THIS DOESN'T WORK, I'M SORRY 


/*!
 * These variables are used to specify the size and address of the memory pool
 * that the simple allocator works against.  The memory pool is allocated within
 * init_myalloc(), and then myalloc() and free() work against this pool of
 * memory that mem points to.
 */
int MEMORY_SIZE;
unsigned char *mem;


// doubly linked header to keep track of free blocks
struct header {
 /* Negative size means the block is allocated, */
 /* positive size means the block is available. */
 int size;
 struct header *nexth;
 struct header *prevh;
} *start;

/* TODO:  The unacceptable allocator uses an external "free-pointer" to track
 *        where free memory starts.  If your allocator doesn't use this
 *        variable, get rid of it.
 *
 *        You can declare data types, constants, and statically declared
 *        variables for managing your memory pool in this section too.
 */
//static unsigned char *freeptr;

//set properties of a given header
void set_prop(struct header * node, int amount, struct header * next,
    struct header * prev) {
    node -> size = amount + sizeof(node);
    node -> nexth = next;
    node -> prevh = prev;
}


/*!
 * This function initializes both the allocator state, and the memory pool.  It
 * must be called before myalloc() or myfree() will work at all.
 *
 * Note that we allocate the entire memory pool using malloc().  This is so we
 * can create different memory-pool sizes for testing.  Obviously, in a real
 * allocator, this memory pool would either be a fixed memory region, or the
 * allocator would request a memory region from the operating system (see the
 * C standard function sbrk(), for example).
 */
void init_myalloc() {

    /*
     * Allocate the entire memory pool, from which our simple allocator will
     * serve allocation requests.
     */

    mem = (unsigned char *) malloc(MEMORY_SIZE);
    if (mem == 0) {
        fprintf(stderr,
                "init_myalloc: could not get %d bytes from the system\n",
		MEMORY_SIZE);
        abort();
    }

    /* TODO:  You can initialize the initial state of your memory pool here. */
    //put the first header at mem; connect it to the next header location
    start = (struct header *) mem;
    struct header * next = ( (void *)start + start->size);
    set_prop(start, MEMORY_SIZE, next, NULL);
}

//splits a block "original" into "original" and "new", if original
// had more space than is being currently allocated; removes
// original from free list
// amount is the amount of memory to be allocated
struct header * split_block(struct header * original, int amount) {
  
    // new block at next memory location?
    struct header * new;
    new = (struct header *) ((void *)original + original->size);

    // original's size should be the amount of memory to be alloc.
    original -> size = amount * -1;
    // the new block should have the remaining memory, and
    // inherit original's connections in the free list
    new -> size = original -> size - amount;
    new -> nexth = original -> nexth;
    new -> prevh = original -> prevh;
    
    // replace original in the linked list with new
    original -> nexth -> prevh = new;
    original -> prevh -> nexth = new;
    
    // remove the original block from the free list
    original -> nexth = NULL;
    original -> prevh = NULL;
 
    return new;   
}


// merge free block h1 with next free block h2
void merge_block_forward(struct header * h1, struct header * h2) {
   
    // collect h2 into h1
    h1 -> size += h2 -> size;
    h1 -> size -= sizeof(h2);
    h1 -> nexth = h2 -> nexth;
    
    // replace h2 in free list with h1
    h2 -> size = 0;
    h2 -> nexth -> prevh = h1;
    h2 -> prevh -> nexth = h1;

    // remove h2
    h2 -> nexth = NULL;
    h2 -> prevh = NULL;

}

// merge free block h2 with previous free block h1
void merge_block_backward(struct header * h1, struct header * h2) {
   
    // collect h1 into h2
    h2 -> size += h1 -> size;
    h2 -> size -= sizeof(h1);
    
    h2 -> prevh = h1 -> prevh;
    
    // replace h1 in free list with h2
    h1 -> size = 0;
    h1 -> nexth -> prevh = h2;
    h1 -> prevh -> nexth = h2;

    // remove h2
    h1 -> nexth = NULL;
    h1 -> prevh = NULL;
}



/*!
 * Attempt to allocate a chunk of memory of "size" bytes.  Return 0 if
 * allocation fails.
 */
unsigned char *myalloc(int size) {

    /* TODO:  The unacceptable allocator simply checks to see if there are at
     *        least "size" bytes left in the pool, and if so, the caller gets
     *        the current "free-pointer" value, and then freeptr is incremented
     *        by size bytes.
     *
     *        Your allocator will be more sophisticated!
     */

    // iterate through the free list, starting from the beginning
    struct header * temp;
    temp = start;
    int success = -1;
    while (temp != NULL) {
        // if a free block has enough space, use it for allocation
        if (temp -> size >= size) {
            // if it has more than enough space, split it first
            if (temp -> size > size) {
                break;            
            
            }
                start = temp;
                success = 1;
                break;    
            
        }
        
        temp = temp -> nexth;
    }
    
    
    if (success < 0) {
        fprintf(stderr, "myalloc: cannot service request of size %d with"
                    " %lx bytes allocated\n", size, ((unsigned char *)start - mem));
        return (unsigned char *) 0;
    }
    else {
        return (unsigned char *) start;
    }    
    
}

/*!
 * Free a previously allocated pointer.  oldptr should be an address returned by
 * myalloc().
 */
void myfree(unsigned char *oldptr) {
 
     // an error here: after myfree, temp -> nexth is temp itself
 
     // reverse allocation status of current block
     struct header * temp;
     temp = (struct header *) oldptr;
     temp -> size *= -1;
     
     
     //check if there is a free block immediately before or after current block
     struct header * next = ( (void *) temp + temp->size);

     struct header * prev = ( (void *) temp - temp->size);

     //if so, merge appropriately
     if (temp -> nexth == next) {
        merge_block_forward(temp, temp -> nexth);
     }
     else if (temp -> prevh == prev) {
        merge_block_backward(temp, temp -> prevh);
     }
     
     start = temp;
     
}
     

/*!
 * Clean up the allocator state.
 * All this really has to do is free the user memory pool. This function mostly
 * ensures that the test program doesn't leak memory, so it's easy to check
 * if the allocator does.
 */
void close_myalloc() {
    free(mem);
}
