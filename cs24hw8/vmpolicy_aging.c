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
#include <stdint.h>
#include <signal.h>
#include <memory.h>
#include <sys/time.h>


/*============================================================================
 * "Loaded Pages" Data Structure
 *
 * This data structure records all pages that are currently loaded in the
 * virtual memory, so that we can choose a random page to evict very easily.
 */

typedef struct datum {
    page_t page;
    uint32_t age;
} datum_t;

typedef struct loaded_pages_t {
    /* The maximum number of pages that can be resident in memory at once. */
    int max_resident;
    
    /* The number of pages that are currently loaded.  This can initially be
     * less than max_resident.
     */
    int num_loaded;
    
    datum_t data[];
} loaded_pages_t;

/*============================================================================
 * Policy Implementation
 */
static loaded_pages_t *loaded;
static int AGE_SIZE = 31;
static uint32_t MIN_AGE = 9999;
static int MIN_I  = 0;
/* Initialize the policy.  Return nonzero for success, 0 for failure. */
int policy_init(int max_resident) {
    fprintf(stderr, "Using AGING eviction policy.\n\n");
    
    loaded = malloc(sizeof(loaded_pages_t) + max_resident * sizeof(datum_t));
    if (loaded) {
        loaded->max_resident = max_resident;
        loaded->num_loaded = 0;
    }
    
    /* Return nonzero if initialization succeeded. */
    return (loaded != NULL);
}

/* Clean up the data used by the page replacement policy. */
void policy_cleanup(void) {
    free(loaded);
    loaded = NULL;
}


/* This function is called when the virtual memory system maps a page into the
 * virtual address space.  Record that the page is now resident.
 */
void policy_page_mapped(page_t page) {
    assert(loaded->num_loaded < loaded->max_resident);
    loaded->data[loaded->num_loaded].page = page;
    loaded->data[loaded->num_loaded].age = 1 << AGE_SIZE;
    loaded->num_loaded++;
}


/* This function is called when the virtual memory system has a timer tick. */
void policy_timer_tick(void) {
    /* Do nothing! */
    int i = 0;
    MIN_AGE = 1 << AGE_SIZE;
    MIN_I = 0;
    while (i < loaded->num_loaded) {
        page_t p = loaded->data[i].page;

        loaded->data[i].age = loaded->data[i].age >> 1;

        if (is_page_accessed(p)) {
            loaded->data[i].age = loaded->data[i].age 
                                   | 1 << AGE_SIZE;
            clear_page_accessed(p);
            set_page_permission(p, PAGEPERM_NONE);
        } else {
            loaded->data[i].age = loaded->data[i].age & 
                            ~(1 << AGE_SIZE);
        }

        if (loaded->data[i].age < MIN_AGE) {
            MIN_AGE = loaded->data[i].age;
            MIN_I = i;
        }
        i++;

    }
}


/* Choose a page to evict from the collection of mapped pages.  Then, record
 * that it is evicted.  This is very simple since we are implementing a random
 * page-replacement policy.
 */
page_t choose_and_evict_victim_page(void) {
    int i_victim = MIN_I;
    page_t victim;

    /* Figure out which page to evict. */
//    victim = loaded->pages[i_victim];

// really?? search for index
   /* int i = 1;
    uint32_t min_age = 1 << AGE_SIZE;
    printf("min_age 1, %u\n", min_age);
   while (i < loaded->num_loaded) {
        if (loaded->data[i].age < min_age) {
            min_age = loaded->data[i].age;
            i_victim = i;
        }
        i++;
    }*/

   // printf("a %u\n", MIN_AGE);
  //  printf("b %u\n", MIN_I);
    
    if (i_victim < loaded->num_loaded) {
        victim = loaded->data[i_victim].page;
    }
    else {
        printf("did not find page \n");
        abort();
    }

    /* Shrink the collection of loaded pages now, by moving the last page in the
     * collection into the spot that the victim just occupied.
     */
    loaded->num_loaded--;
    loaded->data[i_victim].page = loaded->data[loaded->num_loaded].page;

#if VERBOSE
    fprintf(stderr, "Choosing victim page %u to evict.\n", victim);
#endif

    return victim;
}

