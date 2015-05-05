#include "swap_pqueue.h"
#include <stdint.h>
#include "include/klibc.h"

void pqueue_init(struct *swap_space)
{ 
	head = (struct *node) kmalloc(sizeof(struct node));
	head->next = 0;
	head->e_head = swap_space->e_head;
	head->pages_free = swap_space->pages_free;
	head->lower_bits = swap_space->lower_bits;
	head->flags = swap_space->flags;
	head->priority = swap_space->priority;
	head->store_func = swap_space->store_func;
	head->retrieve_func = swap_space->retrieve_func;
	path = head;
	s = 1;
}

void pqueue_push(struct *swap_space)
{
	node *restore = path; int count=1;
	path = head;
	while(count<=pqueue_size()){
		if(path->priority > x && count==1){ //set as new head
			head = (struct node*) kmalloc(sizeof(struct node));
			head->next = path;
			pqueue_set(head, swap_space);
			path = restore;
			break;
		}
		if(path->priority < x && path->next == 0){ //set as new tail
			path = restore;
			path->next = (struct node*) kmalloc(sizeof(struct node));
			path = path->next;
			pqueue_set(path, swap_space);
			path->next = NULL;
			break;
		}
		if(path->priority < x && count!=pqueue_size() && (path->next)->val > x){ //set in between nodes
			node *temp = (struct node*) kmalloc(sizeof(struct node));
			pqueue_set(temp, swap_space);
			temp->next = path->next;
			path->next = temp;
			path = restore;
			break;
		}
		path = path->next;
		count++;
	}
	s++;
}

//set head to next node and delete previous node
//pops from front
void pqueue_pop_front()
{
	node *temp = head;
	head = head->next;
	free(temp);
	s--;
}

void pqueue_pop_back()
{
	free(path);
	path = head;
	while(path->next == 0){
		path = path->next;
	}
	s--;
}

void pqueue_set(struct *h, struct *swap_space){
	h->e_head = swap_space->e_head;
	h->pages_free = swap_space->pages_free;
	h->lower_bits = swap_space->lower_bits;
	h->flags = swap_space->flags;
	h->priority = swap_space->priority;
	h->store_func = swap_space->store_func;
	h->retrieve_func = swap_space->retrieve_func;	
}

struct *pqueue_index(int i)
{
	if(x == 0){
		return head;
	}
	if(x==(pqueue_size()-1)){
		return path;
	}
	node *restore = head;
	while(int x = 0; x<i; x++){
		restore = restore->next;
	}
	return restore;
}

uint8_t pqueue_size()
{
	return s;	
}

// returns specified value from the head of list
int pqueue_peek(int type)
{
	switch(type){
		case 0: return head->lower_bits;
		case 1: return head->flags;
		case 2: return head->priority;
		case 3: return head->pages_free;
	}
}

