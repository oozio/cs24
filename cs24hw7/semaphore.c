/*
 * General implementation of semaphores.
 *
 *--------------------------------------------------------------------
 * Adapted from code for CS24 by Jason Hickey.
 * Copyright (C) 2003-2010, Caltech.  All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "sthread.h"
#include "semaphore.h"
#include "queue.h"
/*
 * The semaphore data structure contains an int i, which represents
 * the number of threads that can still pass through the semaphore, and
 * a queue of blocked threads that tried to pass through it when i was 0.
 */

struct _semaphore {
    // the number of threads we can still run
    int i; 
    // queue of blocked threads that are holding this semaphore
    Queue *blocked_queue;
};

/************************************************************************
 * Top-level semaphore implementation.
 */

/*
 * Allocate a new semaphore.  The initial value of the semaphore is
 * specified by the argument.
 */
Semaphore *new_semaphore(int init) {
    Semaphore *semp = (Semaphore *) malloc(sizeof(Semaphore));
   
    // initialize blocked_queue
    Queue * q = (Queue *) malloc(sizeof(Queue));
    
    // check whether malloc succeeded
    if (semp == 0 || q == 0) {
        fprintf(stderr, "new_bounded_buffer: out of memory\n");
        exit(1);
    }

    // set values
    memset(q, 0, sizeof(Queue));
    semp->i = init;
    semp->blocked_queue = q;
    return semp;
}

/*
 * Decrement the semaphore.
 * This operation must be atomic, and it blocks iff the semaphore is zero.
 */
void semaphore_wait(Semaphore *semp) {
    // to make this function atomic, we want to lock at the starf t
    // and unlock at the end, while keeping in mind that
    // sthread_block unlocks   
    __sthread_lock();
    // if there are no runnable things, add the thread we're trying
    // but failing to run to this semaphore's blocked queue, and 
    // hold here until we run it

    while (semp->i == 0) {
        queue_append(semp->blocked_queue, sthread_current());
        sthread_block();
        __sthread_lock(); // we lock again since there's an unlock
                          // at the end of sthread_block
    }
    // decrement counter and unlock
    semp->i--;
    __sthread_unlock();

}

/*
 * Increment the semaphore.
 * This operation must be atomic.
 */
void semaphore_signal(Semaphore *semp) {
    // to make this function atomic, we want to lock at the start
    // and lock at the end, since sthread_unblock never unlocks
    
    __sthread_lock();
    // if we have no runnable things, unblock one of the threads
    // holding this semaphore

    if (!queue_empty(semp->blocked_queue)) {
       sthread_unblock(queue_take(semp->blocked_queue));
    }
    // increment semaphore
    semp->i++;
    __sthread_unlock();
}

