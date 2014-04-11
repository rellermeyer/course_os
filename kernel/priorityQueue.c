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
    head = malloc(sizeof(Node));
    head->next = NULL;
    head->prev = NULL;
    head->task = NULL;
    head->priority = NOT_SET;
    currentNode = head;
}

/* Add a task to the queue with a given priority */
int add(void* task, int priority) {
	if(head == NULL) {
		init();
	}

    Node *newTask = malloc(sizeof(Node));
    newTask->next = NULL;
    newTask->prev = NULL;
    newTask->task = task;
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

    return 1; // Change if there should be a condition for not adding a task to the priority queue.
}

void* remove(void *task) { //PCB abstraction will change the parameters, can search for processes by PID.
	currentNode = head;
	while(currentNode->next != task) {
		currentNode = currentNode->next;
	}
	Node *nodeToRemove = currentNode->next;
	currentNode->next = nodeToRemove->next;
	currentNode->next->prev = currentNode;
	nodeToRemove->next = nodeToRemove->prev = NULL;
	return nodeToRemove->task;
}

void dispatch(void *task) { //requires free-standing ASM code
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
