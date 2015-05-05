#include "swap_pqueue.h"
#include "swap_framework.h"
#include <stdint.h>
#include "klibc.h"

void pqueue_init(struct swap_space *ss)
{ 
	head = (struct node*) kmalloc(sizeof(struct node));
	head->next = 0;
	head->e_head = ss->e_head;
	head->lower_bits = ss->lower_bits;
	head->priority = ss->priority;
	head->store_func = ss->store_func;
	head->retrieve_func = ss->retrieve_func;
	path = head;
	s = 1;
}

void pqueue_push(struct swap_space* ss)
{
	struct node *restore = path; int count=1;
	path = head;
	while(count<=pqueue_size()){
		if(path->priority > ss->priority && count==1){ //set as new head
			head = (struct node*) kmalloc(sizeof(struct node));
			head->next = path;
			pqueue_set(head, ss);
			path = restore;
			break;
		}
		if(path->priority < ss->priority && path->next == 0){ //set as new tail
			path = restore;
			path->next = (struct node*) kmalloc(sizeof(struct node));
			path = path->next;
			pqueue_set(path, ss);
			path->next = NULL;
			break;
		}
		if(path->priority < ss->priority && count!=pqueue_size() && 
				(path->next)->priority > ss->priority){ //set in between nodes
			struct node *temp = (struct node*) kmalloc(sizeof(struct node));
			pqueue_set(temp, ss);
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
	struct node *temp = head;
	head = head->next;
	kfree(temp);
	s--;
}

void pqueue_pop_back()
{
	kfree(path);
	path = head;
	while(path->next == 0){
		path = path->next;
	}
	s--;
}

void pqueue_set(struct node *h, struct swap_space *ss){
	h->e_head = ss->e_head;
	h->lower_bits = ss->lower_bits;
	h->priority = ss->priority;
	h->store_func = ss->store_func;
	h->retrieve_func = ss->retrieve_func;	
}

struct node* pqueue_index(int i)
{
	if(i == 0){
		return head;
	}
	if(i==(pqueue_size()-1)){
		return path;
	}

	struct node *restore = head;
	for(int x = 0; x<i; x++){
		restore = restore->next;
	}
	return restore;
}

uint8_t pqueue_size()
{
	return s;	
}

// returns specified value from the head of list
uint32_t pqueue_peek(int type)
{
	switch(type){
		case 0: return head->lower_bits;
		case 1: return head->priority;
	}
}


