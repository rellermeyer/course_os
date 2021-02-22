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
    struct ProcessControlBlock * prev;
    struct ProcessControlBlock * next;
    int wakeupTime;
} ProcessControlBlock;

ProcessControlBlock * findNode(int id, ProcessControlBlock * list);

void addPCBNodeBefore(ProcessControlBlock * oldNode, ProcessControlBlock * newNode);
void addPCBNodeAfter(ProcessControlBlock * oldNode, ProcessControlBlock * newNode);

ProcessControlBlock * removePCBNode(ProcessControlBlock *);
#endif
