#include "process.h"

#define NOT_SET -1

/* Data Structure: Doubly Linked List */
typedef struct {
    struct node *next;
    struct node *prev;
    pcb *PCB; // Pointer to the process control block
    int priority;
} Node;


void init();
int add(void *PCB, int priority);
void* remove(void *PCB);
void join(void* TCB);
void dispatch(void *PCB);
void schedule();
void task_yield();
