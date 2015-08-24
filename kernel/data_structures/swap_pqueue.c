#include <stdint.h>
#include "swap_pqueue.h"
#include "swap_framework.h"
#include "klibc.h"

void pqueue_init(struct swap_space *ss)
{ 
	head = (struct node*) kmalloc(sizeof(struct node));
	head->next = NULL;
	head->e_head = ss->e_head;
	head->lower_bits = ss->lower_bits;
	head->priority = ss->priority;
	head->store_func = ss->store_func;
	head->retrieve_func = ss->retrieve_func;
	path = head;
	s = 1;
}

void pqueue_push(struct swap_space *ss)
{
	struct node *restore = path; int count=1;
	path = head;
	while (count<=pqueue_size()) {
		if (path->priority > ss->priority && count==1) { //set as new head
			head = (struct node*) kmalloc(sizeof(struct node));
			head->next = path;
			pqueue_set(head, ss);
			path = restore;
			break;
		}
		if (path->priority < ss->priority && path->next == NULL) { //set as new tail
			path = restore;
			path->next = (struct node*) kmalloc(sizeof(struct node));
			path = path->next;
			pqueue_set(path, ss);
			path->next = NULL;
			break;
		}
		if (path->priority < ss->priority && count!=pqueue_size() && 
				(path->next)->priority > ss->priority) { //set in between nodes
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
	while (path->next == NULL) {
		path = path->next;
	}
	s--;
}

void pqueue_pop_at(uint8_t ssid)
{	
	struct node *temp = head;
	int x;
	for (x = 0; x<pqueue_size(); x++) {
		if (temp->lower_bits == ssid) {
			break;
		}
		temp = temp->next;
	}
	if (x == 0) {
 		pqueue_pop_front();
	} else if (x == pqueue_size()-1) {
		pqueue_pop_back();
	} else {
		struct node *temp2 = head;
		for (int y = 1; y<x; y++) {
			temp2 = temp2->next;
		}
		temp2->next = temp->next;
		kfree(temp);
	}
}

void pqueue_set(struct node *h, struct swap_space *ss){
	h->lower_bits = ss->lower_bits;
	h->priority = ss->priority;
	h->store_func = ss->store_func;
	h->retrieve_func = ss->retrieve_func;
	h->e_head = ss->e_head;	
}

struct node *pqueue_index(int i)
{
	if (i == 0) {
		return head;
	}
	if (i == (pqueue_size()-1)) {
		return path;
	}

	struct node *restore = head;
	int x;
	for (x = 0; x<i; x++) {
		restore = restore->next;
	}

	return restore;
}

uint8_t pqueue_size()
{
	return s;	
}

// returns specified value from the head of list
void *pqueue_peek(int type)
{
	void *foo;
	switch (type) {
		case 0:
		       	foo = &(head->lower_bits);
		case 1: 
			foo = &(head->priority);
		case 2: 
			foo = head->e_head;
	}
	return foo;
}

struct node *pqueue_find(uint8_t ssid)
{
	struct node *temp = head;

	while (temp != NULL) {
		if (temp->lower_bits == ssid) {
			return temp;	 
		}
		temp = temp->next;
	}

	return NULL;
}


