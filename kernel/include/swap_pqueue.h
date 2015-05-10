#ifndef __VM_SWAP_PQUEUE_H
#define __VM_SWAP_PQUEUE_H

#include <stdint.h>
#include "swap_framework.h"


/* Last Update: 05/10/15 */


struct node{
	struct node *next;
	struct swap_entry *e_head;
	uint8_t lower_bits; // swap space ID [8-bits]
//	uint16_t flags; // SWP_USED (1000 or 1), SWP_WRITEOK (0010 or 2) 
	uint8_t priority; // lower is better 
        func store_func;
        func retrieve_func;
}; // Total: 16 bytes

static struct node *head;
static struct node *path; //tail
static uint8_t s;


/* pqueue_init initializes the priority queue with a given
 * swap_space type struct
 */
void pqueue_init(struct swap_space*);


/* pqueue_set does a deep copy of the value of the second
 * struct to the front struct
 */
void pqueue_set(struct node*, struct swap_space*);


/* pqueue_push pushes the swap_space struct onto the proper
 * index based on the priority found in the struct
 */
void pqueue_push(struct swap_space*);


/* pqueue_pop_front & pqueue_pop_back pops a node from the head/back of the 
 * pqueue_pop_front priority queue as well as freeing the given memory of the 
 * head/back
 */
void pqueue_pop_front();
void pqueue_pop_back();
void pqueue_pop_at(uint8_t); // Note: 'at' ssid not index

/* pqueue_size returns the current size of the priority queue */
uint8_t pqueue_size();


/* pqueue_index finds the given index of the priority queue
 * 
 * Returns: The struct of the given index in pqueue
 * NOTE: THIS IS NOT THE SAME AS THE SWAP_SPACE STRUCT
 */
struct node *pqueue_index(int);


/* It 'peeks' at the head of the list for a specified value
 *
 * Returns: The specified value from the head of the list
 * 0 - lower_bits
 * 1 - priority
 * 2 - e_head
 */
void *pqueue_peek(int);


/* pqueue_find finds the actual index of the swap space ID (lower_bit value)
 *
 * Returns: The found node specified or NULL if not found
 * NOTE: THIS IS NOT THE SAME AS THE SWAP_SPACE STRUCT
 */ 
struct node *pqueue_find(uint8_t);


#endif
