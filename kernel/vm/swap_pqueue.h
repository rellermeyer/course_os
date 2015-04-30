#ifndef __VM_SWAP_PQUEUE_H
#define __VM_SWAP_PQUEUE_H
#include <stdint.h>

struct node{
	struct node *next;
	struct swap_entry *e_head;
        uint16_t pages_used;
	uint8_t lower_bits; // swap space ID [8-bits]
	uint16_t flags; // SWP_USED (1000 or 1), SWP_WRITEOK (0010 or 2) OR BOTH (0011 or 3)
	uint8_t priority; // lower is better 
        uint32_t *store_func;
        uint32_t *retrieve_func;
}; // Total: 12 bytes

static struct node *head;
static struct node *path; // tail
static uint8_t s;

void pqueue_init(struct*);
void set(struct*, struct*);
void push(struct*);
void pop();


#endif
