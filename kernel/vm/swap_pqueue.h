#ifndef __VM_SWAP_PQUEUE_H
#define __VM_SWAP_PQUEUE_H
#include <stdint.h>

struct node{
	struct swap_space_list *next;
	struct swap_entry *head;
	uint8_t lower_bits; //swap space ID [8-bits]
	uint16_t flags; //SWP_USED (1000 or 1), SWP_WRITEOK (0100 or 2) OR BOTH (1100 or 3)
	uint8_t priority; //lower is better
}; // Total: 12 bytes

static struct node *head;
static struct node *current;
static uint8_t s;

void pqueue_init();
void reset();
void push();
void *pop();


#endif
