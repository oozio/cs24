#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdint.h>

#include "multimap.h"

// a big number for object pool allocation size
const int ALOT = 50;

// number of times we've increased the size of the object pool
int times = 0;

// size of values array
#define N 13
/*============================================================================
 * TYPES
 *
 *   These types are defined in the implementation file so that they can
 *   be kept hidden to code outside this source file.  This is not for any
 *   security reason, but rather just so we can enforce that our testing
 *   programs are generic and don't have any access to implementation details.
 *============================================================================*/

 /* Represents a value that is associated with a given key in the multimap. */
typedef struct multimap_value {
    int values[N];
    int index;
    struct multimap_value *next;
} multimap_value;


/* Represents a key and its associated values in the multimap, as well as
 * pointers to the left and right child nodes in the multimap. */
typedef struct multimap_node {
    /* The key-value that this multimap node represents. */
    int key;

    /* A linked list of the values associated with this key in the multimap. */
    multimap_value *values;

    /* The tail of the linked list of values, so that we can retain the order
     * they were added to the list.
     */
    multimap_value *values_tail;

    /* The left child of the multimap node.  This will reference nodes that
     * hold keys that are strictly less than this node's key.
     */
    struct multimap_node *left_child;

    /* The right child of the multimap node.  This will reference nodes that
     * hold keys that are strictly greater than this node's key.
     */
    struct multimap_node *right_child;
} multimap_node;


/* The entry-point of the multimap data structure. */
struct multimap {
    multimap_node *root;
    multimap_node *start;
    multimap_node *end;
    multimap_node *free_start;
};


/*============================================================================
 * HELPER FUNCTION DECLARATIONS
 *
 *   Declarations of helper functions that are local to this module.  Again,
 *   these are not visible outside of this module.
 *============================================================================*/

multimap_node * alloc_mm_node(multimap *map, int OLD);

multimap_node * find_mm_node(multimap_node *root, int key,
                             int create_if_not_found, multimap *map, int OLD);

void free_multimap_values(multimap_value *values);
void free_multimap_node(multimap_node *node);


/*============================================================================
 * FUNCTION IMPLEMENTATIONS
 *============================================================================*/

// increases the object pool, given by the distance between map->start
// and map->end
// if times == 0, we have to initialize the pool with malloc instead
void grow_pool(multimap *map) {
    if (times == 0) {    
        // initialize pool with ALOT of nodes; set first free node to start
        map->start = malloc(sizeof(multimap_node)  * ALOT);
        map->end = map->start + (sizeof(multimap_node)  * ALOT);
        map->free_start = map->start;

        times ++;
    } else {
        times ++;
        // we want to keep track of how many times we've realloc'd, so
        // that we know how much total space we've allocated

        // current size
        int og = sizeof(multimap_node) * ALOT * times;
        
        // future size
        int increment = og + (sizeof(multimap_node) * ALOT);

        //realloc map->start to be bigger
        multimap_node * ptr1 = realloc(map->start, increment);
        if (ptr1 == NULL)          //reallocated pointer ptr1
        {       
            printf("\n realloc failed!!");
            exit(0);
        }
        else if (ptr1 != map->start)
        {
            map->start = ptr1;          
        }
        ptr1 = NULL;
        
        // shift map->end and map->free_start accordingly
        map->end = map->start + increment;

        map->free_start = map->end - sizeof(multimap_node) * ALOT;
    }
}

/* Allocates a multimap node, and zeros out its contents so that we know what
 * the initial value of everything will be.
 */

/* If there's free space available in the object pool, take the next free slot;
otherwise, grow the object pool by one pool size, and take the next free slot 
*/
multimap_node * alloc_mm_node(multimap *map, int OLD) {
    // original function from mm_implc.c
    // set OLD to 1 for allocation to work
    if (OLD == 1)  {
        multimap_node *node = malloc(sizeof(multimap_node));
        bzero(node, sizeof(multimap_node));
        return node;
    }

    // if there are no more free nodes in the map, 
    // the object pool is full; reallocate a bunch more
    if (map->end < map->free_start) {
       grow_pool(map);
    }

    // return the most immediate next free node
    multimap_node *node = map->free_start;
    map->free_start = map->free_start + sizeof(multimap_node);
    bzero(node, sizeof(multimap_node));
    

    if (node == NULL) {
        abort();
    }

    return node;
}   

/* This helper function searches for the multimap node that contains the
 * specified key.  If such a node doesn't exist, the function can initialize
 * a new node and add this into the structure, or it will simply return NULL.
 * The one exception is the root - if the root is NULL then the function will
 * return a new root node.
 */
multimap_node * find_mm_node(multimap_node *root, int key,
                             int create_if_not_found, multimap *map, int OLD) {
    multimap_node *node;

    /* If the entire multimap is empty, the root will be NULL. */
    if (root == NULL) {
        if (create_if_not_found) {
            root = alloc_mm_node(map, OLD);
            root->key = key;
        }
        return root;
    }

    /* Now we know the multimap has at least a root node, so start there. */
    node = root;
    while (1) {
        if (node->key == key)
            break;

        if (node->key > key) {   /* Follow left child */
            if (node->left_child == NULL && create_if_not_found) {
                /* No left child, but caller wants us to create a new node. */
                multimap_node *new = alloc_mm_node(map, OLD);
                new->key = key;

                node->left_child = new;
            }
            node = node->left_child;
        }
        else {                   /* Follow right child */
            if (node->right_child == NULL && create_if_not_found) {
                /* No right child, but caller wants us to create a new node. */
                multimap_node *new = alloc_mm_node(map, OLD);
                new->key = key;

                node->right_child = new;
            }
            node = node->right_child;
        }

        if (node == NULL)
            break;
    }

    return node;
}


/* This helper function frees all values in a multimap node's value-list. */
void free_multimap_values(multimap_value *values) {
    while (values != NULL) {
        multimap_value *next = values->next;
#ifdef DEBUG_ZERO
        /* Clear out what we are about to free, to expose issues quickly. */
        bzero(values, sizeof(multimap_value));
#endif
        free(values);
        values = next;
    }
}


/* This helper function frees a multimap node, including its children and
 * value-list.
 */
void free_multimap_node(multimap_node *node) {
    if (node == NULL)
        return;

    /* Free the children first. */
    free_multimap_node(node->left_child);
    free_multimap_node(node->right_child);

    /* Free the list of values. */
    free_multimap_values(node->values);

#ifdef DEBUG_ZERO
    /* Clear out what we are about to free, to expose issues quickly. */
    bzero(node, sizeof(multimap_node));
#endif
    free(node);
}


/* Initialize a multimap data structure. */
// we want to allocate an entire block at a time, so that the addresses
// will be spatially proximal 
multimap * init_multimap() {
    multimap *mm = malloc(sizeof(multimap));
    mm->root = NULL;
    times = 0;
    //initialize the map with a bunch of nodes, so that their addresses
    // will be close together
    grow_pool(mm);
    return mm;
}


/* Release all dynamically allocated memory associated with the multimap
 * data structure.
 */
void clear_multimap(multimap *mm) {
    assert(mm != NULL);
    free_multimap_node(mm->root);
    free(mm->start);

    mm->root = NULL;
}


/* Adds the specified (key, value) pair to the multimap. */
void mm_add_value(multimap *mm, int key, int value) {
    multimap_node *node;

    assert(mm != NULL);

    /* Look up the node with the specified key.  Create if not found. */

    node = find_mm_node(mm->root, key, /* create */ 1, mm, 1);
    if (mm->root == NULL)
        mm->root = node;

    assert(node != NULL);
    assert(node->key == key);

    /* Add the new value to the multimap node. */

    if (node->values_tail != NULL) {// if the linked list already exists

        //if there's empty slots in this value array
        if (node->values_tail->index < N) { 
            // write new value into the array
            node->values_tail->values[node->values_tail->index] = value; 
            node->values_tail->index = node->values_tail->index + 1;

        } else { //otherwise, make and initialize a new value list 
            multimap_value *new_value = malloc(sizeof(multimap_value)); 
            node->values_tail->next = new_value;  
            node->values_tail = node->values_tail->next;
            node->values_tail->values[0] = value;
            node->values_tail->index = 1;
            node->values_tail->next = NULL;
        }   
    }
 
    else { //otherwise, the list is empty, and we should initialize it
        multimap_value *new_value = malloc(sizeof(multimap_value)); 
        node->values = new_value;
        node->values->values[0] = value;
        node->values->index = 1;
        node->values->next = NULL;

        node->values_tail = node->values;
    }

 //   node->values_tail = new_value;

}


/* Returns nonzero if the multimap contains the specified key-value, zero
 * otherwise.
 */
int mm_contains_key(multimap *mm, int key) {
    return find_mm_node(mm->root, key, /* create */ 0, mm, 1) != NULL;
}


/* Returns nonzero if the multimap contains the specified (key, value) pair,
 * zero otherwise.
 */
int mm_contains_pair(multimap *mm, int key, int value) {
    multimap_node *node;
    multimap_value *curr;

    node = find_mm_node(mm->root, key, /* create */ 1, mm, 1);
    if (node == NULL)
        return 0;

    curr = node->values;
    while (curr != NULL) {
        for (int i = 0; i < N; i++) {
            if (curr->values[i] == value)
                return 1;        
        }     

        curr = curr->next;
    }

    return 0;
}


/* This helper function is used by mm_traverse() to traverse every pair within
 * the multimap.
 */
void mm_traverse_helper(multimap_node *node, void (*f)(int key, int value)) {
    multimap_value *curr;

    if (node == NULL)
        return;

    mm_traverse_helper(node->left_child, f);

    curr = node->values;
    while (curr != NULL) {
        for (int i = 0; i < N; i++) {
            f(node->key, curr->values[i]);
        }     
        curr = curr->next;
    }

    mm_traverse_helper(node->right_child, f);
}


/* Performs an in-order traversal of the multimap, passing each (key, value)
 * pair to the specified function.
 */
void mm_traverse(multimap *mm, void (*f)(int key, int value)) {
    mm_traverse_helper(mm->root, f);
}

