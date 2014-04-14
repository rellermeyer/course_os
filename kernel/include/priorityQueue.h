#include "process.h"
#include "global_defs.h"

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
pcb* remove(pcb *PCB);
void join(pcb* other_PCB);
void dispatch(void *PCB);
void schedule();
void task_yield();
Boolean is_in_queue(int pid);
