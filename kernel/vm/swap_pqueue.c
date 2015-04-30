#include "swap_pqueue.h"
#include <stdint.h>

void pqueue_init(struct *swap_space)
{ 
	head = (struct *node) kmalloc(sizeof(struct node));
	head->next = 0;
	head->e_head = swap_space->e_head;
	head->pages_used = swap_space->pages_used;
	head->lower_bits = swap_space->lower_bits;
	head->flags = swap_space->flags;
	head->priority = swap_space->priority;
	head->store_func = swap_space->store_func;
	head->retrieve_func = swap_space->retrieve_func;
	path = head;
	s = 1;
}

void push(struct *swap_space)
{
	node *restore = path; int count=1;
	path = head;
	while(count<=size()){
		if(path->priority > x && count==1){ //set as new head
			head = (struct node*) kmalloc(sizeof(struct node));
			head->next = path;
			set(head, swap_space);
			path = restore;
			break;
		}
		if(path->priority < x && path->next == 0){ //set as new tail
			path = restore;
			path->next = (struct node*) kmalloc(sizeof(struct node));
			path = path->next;
			set(path, swap_space);
			path->next = NULL;
			break;
		}
		if(path->priority < x && count!=size() && (path->next)->val > x){ //set in between nodes
			node *temp = (struct node*) kmalloc(sizeof(struct node));
			set(temp, swap_space);
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
void pop()
{
	node *temp = head;
	head = head->next;
	free(temp);
	s--;
	if(head != NULL)
		return head->val;
	return 0;
}

void set(struct *h, struct *swap_space){
	h->e_head = swap_space->e_head;
	h->pages_used = swap_space->pages_used;
	h->lower_bits = swap_space->lower_bits;
	h->flags = swap_space->flags;
	h->priority = swap_space->priority;
	h->store_func = swap_space->store_func;
	h->retrieve_func = swap_space->retrieve_func;	
}

struct index(int i)
{
	if(x == 0){
		return head;
	}
	node *restore = head;
	while(int x = 0; x<i; x++){
		restore = restore->next;
	}
	return restore;
}

uint8_t size()
{
	return s;	
}

// returns specified value from the head of list
int peek(int type)
{
	switch(type){
		case 0: return head->lower_bits;
		case 1: return head->flags;
		case 2: return head->priority;
	}
}

