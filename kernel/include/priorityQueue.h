#include "process.h"
#include "global_defs.h"

#define NOT_SET -1

/* Data Structure: Doubly Linked List */
typedef struct node{
    struct node *next;
    struct node *prev;
    pcb *PCB; // Pointer to the process control block
    int priority;
} Node;

Node *head;

void init_q();
int add(pcb *PCB, int priority);
pcb* remove(pcb *PCB);
void join(pcb *other_PCB);
void dispatch(pcb *PCB);
void schedule();
void task_yield();
Boolean is_in_queue(int pid);
