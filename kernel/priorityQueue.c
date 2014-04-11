#include <stdlib.h>
#include <stdio.h>

#define NOT_SET -1 

/* Data Structure: Doubly Linked List */
typedef struct {
    struct node *next;
    struct node *prev;
    void *PCB; //Pointer PCB
    int priority;
} Node;

/* Global Variables */
Node *head;
Node *currentNode;
/********************/

/* Initializes all the global variables */
void init() {
    head = mem_alloc(sizeof(Node)); //need to change to mem_alloc
    head->next = NULL;
    head->prev = NULL;
    head->PCB = NULL;
    head->priority = NOT_SET;
    currentNode = head;
}

/* Add a PCB to the queue with a given priority */
int add(void *PCB, int priority) {
	if(head == NULL) {
		init();
	}

    Node *newTask = mem_alloc(sizeof(Node)); //need to change to mem_alloc
    newTask->next = NULL;
    newTask->prev = NULL;
    newTask->PCB = PCB;
    newTask->priority = priority;

    if(head->next == NULL) {
        head->next = newTask;
        newTask->next = head;
        newTask->prev = head;
        head->prev = newTask;
    }
    else {
        currentNode = head;
        while(currentNode->next != head && currentNode->next->priority > newTask->priority) {
            currentNode = currentNode->next;
        }
        newTask->next = currentNode->next;
        newTask->prev = currentNode;
        currentNode->next->prev = newTask;
        currentNode->next = newTask;
    }

    return 1; // Change if there should be a condition for not adding a PCB to the priority queue.
}

void* remove(void *PCB) { //PCB abstraction will change the parameters, can search for processes by PID.
	currentNode = head;
	while(currentNode->next->PCB->PID != PCB->PID) {
		currentNode = currentNode->next;
	}
	Node *nodeToRemove = currentNode->next;
	currentNode->next = nodeToRemove->next;
	currentNode->next->prev = currentNode;
	nodeToRemove->next = nodeToRemove->prev = NULL;
	return nodeToRemove->PCB;
}

void join(void* TCB) { //future implementation for threads.
}

void dispatch(void *PCB) { //requires free-standing ASM code
//Might have to switch modes to user mode? We want restore the PC,SP, and saved registers
}

void schedule() {
	if(head->next == NULL) {
		//error
		//idle
	}
	else {
		Node *nodeToDispatch = head->next;
		remove(head->next);
		dispatch(nodeToDispatch);	
	}
}
