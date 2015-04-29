#include "swap_pqueue.h"
#include <stdint.h>

void pqueue_init(struct swap_space){ 
	head = swap_space;
	head->next = 0;
	current = head;
	s = 0;
}

//path will ALWAYS be pointing to the tail

//create new tail node pointed to be pointed to by next
//move path to new tail node and set val and next pointer

//resets LinkedList or, more accurately, properly sets it
void reset(){
	this->head = new node;
	head->val = x;
	head->next = NULL;
	current = head;
	this->s = 1;
}

void push(int x){
	if(head == NULL && path == NULL && s == 0){
		reset(x);
	}
	else{
		node *restore = path; int count=1;
		path = head;
		while(count<=size()){
			if(path->val > x && count==1){ //set as new head
				head = (struct node*) kmalloc(sizeof(struct node));
				head->val = x;
				head->next = path;
				path = restore;
				break;
			}

			if(path->val < x && path->next == 0){ //set as new tail
				path = restore;
				path->next = (struct node*) kmalloc(sizeof(struct node));
				path = path->next;
				path->val = x;
				path->next = NULL;
				break;
			}
			
			if(path->val < x && count!=size() && (path->next)->val > x){ //set in between nodes
				node *temp = (struct node*) kmalloc(sizeof(struct node));
				temp->val = x;
				temp->next = path->next;
				path->next = temp;
				path = restore;
				break;
			}

			path = path->next;
			count++;
		}
	}
	
	s++;
}

//set head to next node and delete previous node
void pop(){
	node *temp = head;
	head = head->next;
	delete temp;
	s--;
	if(head != NULL)
		return head->val;
	return 0;
}

uint8_t size(){
	return s;	
}

// returns specified value from the head of list
int peek(int type){
	switch(type){
		case 0: return head->lower_bits;
		case 1: return head->flags;
		case 2: return head->priority;
	}
}

