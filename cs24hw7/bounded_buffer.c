/*
 * Define a bounded buffer containing records that describe the
 * results in a producer thread.
 *
 *--------------------------------------------------------------------
 * Adapted from code for CS24 by Jason Hickey.
 * Copyright (C) 2003-2010, Caltech.  All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

#include "semaphore.h"
#include "sthread.h"
#include "bounded_buffer.h"

/*
 * The bounded buffer data.
 */
struct _bounded_buffer {
    /* The maximum number of elements in the buffer */
    int length;

    /* The index of the first element in the buffer */
    int first;

    /* The number of elements currently stored in the buffer */
    int count;

    /* The values in the buffer */
    BufferElem *buffer;

    // semaphore for this buffer's contents
    Semaphore * p_semp;
    Semaphore * c_semp;
    Semaphore * mutex;
};


#define EMPTY -1


/*
 * Allocate a new bounded buffer.
 */
BoundedBuffer *new_bounded_buffer(int length) {
    BoundedBuffer *bufp;
    BufferElem *buffer;
    // 1st semaphore keeps track of the empty slots available in the buffer for
    // produced numbers to inhabit. it starts at the max length of the buffer
    // and is decreased when a thread consumes a slot.
    // p is for "produce-able slots"
    Semaphore * p = new_semaphore(length);

    // 2nd semaphore keeps track of the occupied slots available for 
    // consumption in the buffer. it starts at 0 and grows when a producer 
    // adds something to the buffer.
    // c is for "consumable slots"
    Semaphore * c = new_semaphore(0);

    // 3rd semaphore is to protect the contents of the buffer. we only want
    // one thread to be adding to/consuming from the buffer at a time, so 
    // this is a binary semaphore/mutex that acts as a lock on the two
    // buffer functions.
    Semaphore * m = new_semaphore(1);
    int i;

    /* Allocate the buffer */
    buffer = (BufferElem *) malloc(length * sizeof(BufferElem));
    bufp = (BoundedBuffer *) malloc(sizeof(BoundedBuffer));
    if (buffer == 0 || bufp == 0) {
        fprintf(stderr, "new_bounded_buffer: out of memory\n");
        exit(1);
    }

    /* Initialize */

    memset(bufp, 0, sizeof(BoundedBuffer));

    for (i = 0; i != length; i++) {
        buffer[i].id = EMPTY;
        buffer[i].arg = EMPTY;
        buffer[i].val = EMPTY;
    }

    bufp->length = length;
    bufp->buffer = buffer;

    // set buffer semaphores
    bufp->p_semp = p;
    bufp->c_semp = c;
    bufp->mutex = m;
    return bufp;
}

/*
 * Add an integer to the buffer.  Block thread and
 * wait if the buffer is full.
 */
void bounded_buffer_add(BoundedBuffer *bufp, const BufferElem *elem) {

    /* Wait until the buffer has space and then decrease the number of 
       spaces available to produce*/
    semaphore_wait(bufp->p_semp);
    // add mutex to protect this function
    semaphore_wait(bufp->mutex);

    /* Now the buffer has space.  Copy the element data over. */
    int idx = (bufp->first + bufp->count) % bufp->length;
    bufp->buffer[idx].id  = elem->id;
    bufp->buffer[idx].arg = elem->arg;
    bufp->buffer[idx].val = elem->val;

    bufp->count = bufp->count + 1;
    // release mutex; increment the # of things available to consume
    semaphore_signal(bufp->mutex);
    semaphore_signal(bufp->c_semp);
}


/*
 * Get an integer from the buffer.  Block this 
 * thread and wait if the buffer is empty.
 */
void bounded_buffer_take(BoundedBuffer *bufp, BufferElem *elem) {
    /* Wait until the buffer has a value to retrieve and then decrease 
       the number of available things to consume*/
    semaphore_wait(bufp->c_semp);
    // add mutex to protect this function
    semaphore_wait(bufp->mutex);

    /* Copy the element from the buffer, and clear the record */
    elem->id  = bufp->buffer[bufp->first].id;
    elem->arg = bufp->buffer[bufp->first].arg;
    elem->val = bufp->buffer[bufp->first].val;

    bufp->buffer[bufp->first].id  = -1;
    bufp->buffer[bufp->first].arg = -1;
    bufp->buffer[bufp->first].val = -1;

    bufp->count = bufp->count - 1;
    bufp->first = (bufp->first + 1) % bufp->length;

    // release mutex; increase the # of slots available for producers
    semaphore_signal(bufp->mutex);
    semaphore_signal(bufp->p_semp);
}


