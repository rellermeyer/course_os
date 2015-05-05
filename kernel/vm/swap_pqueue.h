#ifndef __VM_SWAP_PQUEUE_H
#define __VM_SWAP_PQUEUE_H
#include <stdint.h>
#include "swap_framework.h"

struct node{
	struct node *next;
	struct swap_entry *e_head;
	uint8_t lower_bits; // swap space ID [8-bits]
	uint16_t flags; // SWP_USED (1000 or 1), SWP_WRITEOK (0010 or 2) OR BOTH (0011 or 3)
	uint8_t priority; // lower is better 
        uint32_t *store_func;
        uint32_t *retrieve_func;
}; // Total: 12 bytes

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

/* pqueue_pop_front & pqueue_back pops a node from the head/back of 
 * the pqueue_pop_frontriority queue as well as freeing the given memory of the head/back
 */
void pqueue_pop_front();
void pqueue_pop_back();

// pqueue_size returns the current size of the priority queue
uint8_t pqueue_size();

/* pqueue_index finds the given index of the priority queue 
 * and returns the struct of it
 */
struct node* pqueue_index(int);

/* returns the specified value from the head of the list
 * It 'peeks' at the head of the list for a specified value
 * 0 - lower_bits
 * 1 - priority
 */
uint32_t pqueue_peek(int);

/* pqueue_find finds the actual index of the swap space ID 
 * (or lower_bit value)
 * Return: index of found value
 */ 
//int pqueue_find(int);


#endif
