#ifndef PROCESSCONTROLBLOCK_H
#define PROCESSCONTROLBLOCK_H

// TODO who builds these? where/who save/increases the idcounter?

typedef void (*ExecutionStateHandler)();

typedef struct ExecutionState {
    // fill when ready
    int reg1;
    int reg2;
    int reg3;
    

    ExecutionStateHandler handler;
} ExecutionState;

typedef struct ProcessControlBlock {
    int id;
    int parent;
    // Runningtime? total? starttime? last time active?
    ExecutionState * executionState;
    ProcessControlBlock * prev;
    ProcessControlBlock * next;
    int wakeupTime;
} ProcessControlBlock;

void findNode(int id, ProcessControlBlock * list);

void addPCBNodeBefore(ProcessControlBlock * oldNode, ProcessControlBlock * newNode);
void addPCBNodeAfter(ProcessControlBlock * oldNode, ProcessControlBlock * newNode);
void addPCBNodeBefore(int * oldNode, ProcessControlBlock * newNode);
void addPCBNodeAfter(int * oldNode, ProcessControlBlock * newNode);

void removePCBNode(ProcessControlBlock *);
ProcessControlBlock * removePCBNode(int id, ProcessControlBlock * list);

#endif
