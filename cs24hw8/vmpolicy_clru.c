/*============================================================================
 * Implementation of the RANDOM page replacement policy.
 *
 * We don't mind if paging policies use malloc() and free(), just because it
 * keeps things simpler.  In real life, the pager would use the kernel memory
 * allocator to manage data structures, and it would endeavor to allocate and
 * release as little memory as possible to avoid making the kernel slow and
 * prone to memory fragmentation issues.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "virtualmem.h"
#include "vmpolicy.h"

#include <signal.h>
#include <memory.h>
#include <sys/time.h>

/*============================================================================
 * "Loaded Pages" Data Structure
 *
 * This data structure records all pages that are currently loaded in the
 * virtual memory, so that we can choose a random page to evict very easily.
 */

typedef struct loaded_pages_t {
    /* The maximum number of pages that can be resident in memory at once. */
    int max_resident;
    
    /* The number of pages that are currently loaded.  This can initially be
     * less than max_resident.
     */
    int num_loaded;
    
} loaded_pages_t;

typedef struct _queuenode {
    page_t page;
    struct _queuenode *prev;
    struct _queuenode *next;
} QueueNode;

typedef struct _queue {
    QueueNode *head;  /*!< The first page in the queue. */
    QueueNode *tail;  /*!< The last page in the queue. */
} Queue;

/*============================================================================
 * Policy Implementation
 */


/* The list of pages that are currently resident. */
static loaded_pages_t *loaded;
static Queue *queuep;

/*!
 * Add the page to the queue.  If the queue is empty, add the
 * singleton element.  Otherwise, add the element as the tail.
 */
void queue_append(Queue *queuep, page_t page) {
    QueueNode *nodep = (QueueNode *) malloc(sizeof(QueueNode));
    if (nodep == NULL) {
        fprintf(stderr, "Couldn't allocate QueueNode\n");
        abort();
    }

    nodep->page = page;

    if (queuep->head == NULL) {
        nodep->prev = NULL;
        nodep->next = NULL;
        queuep->head = nodep;
        queuep->tail = nodep;
    }
    else {
        queuep->tail->next = nodep;
        nodep->prev = queuep->tail;
        nodep->next = NULL;
        queuep->tail = nodep;
    }
}


/*!
 * Get the first page from the queue.  Returns NULL if the queue is empty.
 */
page_t queue_take(Queue *queuep) {
    QueueNode *nodep;
    page_t page;

    assert(queuep != NULL);
    
    /* Return NULL if the queue is empty */
    if(queuep->head == NULL)
        return 0;

    /* Go to the final element */
    nodep = queuep->head;
    if(nodep == queuep->tail) {
        queuep->head = NULL;
        queuep->tail = NULL;
    }
    else {
        nodep->next->prev = NULL;
        queuep->head = nodep->next;
    }

    page = nodep->page;
    free(nodep);
    return page;
}
/*!
 * Remove a page from a queue.
 *
 * Returns 1 if the page was found in the queue, or 0 if the page was not
 * found in the queue.
 *
 */
int queue_remove(Queue *queuep, page_t page) {
    QueueNode *nodep;

    assert(queuep != NULL);

    /* Find the node in the queue with the specified thread. */
    nodep = queuep->head;
    while (nodep != NULL && nodep->page != page)
        nodep = nodep->next;

    if (!nodep)
        return 0;

    /* Found the node; unlink it from the list. */
    if (nodep->prev != NULL)
        nodep->prev->next = nodep->next;
    if (nodep->next != NULL)
        nodep->next->prev = nodep->prev;

    /* Reset head and tail pointers */
    if(queuep->head == nodep)
        queuep->head = nodep->next;
    if(queuep->tail == nodep)
        queuep->tail = nodep->prev;

    /* Delete the node. */
    free(nodep);

    /* We removed a node so return 1. */
    return 1;
}

/* Initialize the policy.  Return nonzero for success, 0 for failure. */
int policy_init(int max_resident) {
    fprintf(stderr, "Using CLRU eviction policy.\n\n");

    //allocate loaded    
    loaded = malloc(sizeof(loaded_pages_t));
    if (loaded) {
        loaded->max_resident = max_resident;
        loaded->num_loaded = 0;
    }
    //allocate and check allocation of queue
    queuep = malloc(sizeof(Queue));
    if (queuep == NULL) {
        fprintf(stderr, "queue allocation failed\n");
        exit(1);
    }

    
    /* Return nonzero if initialization succeeded. */
    
    return (loaded != NULL);
}

/* Clean up the data used by the page replacement policy. */
void policy_cleanup(void) {
    free(loaded);
    free(queuep);
    loaded = NULL;
    queuep = NULL;
}


/* This function is called when the virtual memory system maps a page into the
 * virtual address space.  Record that the page is now resident.
 */
void policy_page_mapped(page_t page) {
    assert(loaded->num_loaded < loaded->max_resident);

    loaded->num_loaded++;
    
    // try to add the page to our queue; abort if the queue is null
    if (queuep != NULL) {
        queue_append(queuep, page);
    } else {
        printf("Null queue\n"); 
        abort();
    }
}


/* This function is called when the virtual memory system has a timer tick. */
void policy_timer_tick(void) {
    page_t p;
    int remove_success;
    // loop through the queue
    QueueNode *node = queuep->head;
    while (node != NULL){
        p = node->page;
        // if this page has been accessed since the last tick,
        if (is_page_accessed(p)) {
            
            // remove from the queue and put it at the end
            remove_success = queue_remove(queuep, p);
            if (!remove_success) {
                printf("failed to remove node from queue\n");
                abort();
            }
            queue_append(queuep, p);

            // clear access
            clear_page_accessed(p);
            //set page permissions so that we can track its next access
            set_page_permission(p, PAGEPERM_NONE);
        }
        // continue loop
        node = node->next;
    }

}


/* Choose a page to evict from the collection of mapped pages.  Then, record
 * that it is evicted.  We choose the page at the front of the queue.
 */
page_t choose_and_evict_victim_page(void) {
    page_t victim;

    /* Figure out which page to evict. */
    victim = queue_take(queuep);
    /* Shrink the collection of loaded pages now, by moving the last page in the
     * collection into the spot that the victim just occupied.
     */
    loaded->num_loaded--;

#if VERBOSE
    fprintf(stderr, "Choosing victim page %u to evict.\n", victim);
#endif

    return victim;
}

