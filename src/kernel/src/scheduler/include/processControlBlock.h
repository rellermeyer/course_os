#ifndef PROCESSCONTROLBLOCK_H
#define PROCESSCONTROLBLOCK_H

typedef enum {
    Running,
    Ready,
    Waiting,
} ProcessState;

typedef struct ExecutionState {
    // fill when ready
} ExecutionState;

typedef struct ProcessControlBlock {
    int id;
    int parent;
    // Runningtime? total? starttime? last time active?
    ProcessState state;
    ExecutionState * executionState;
} ProcessControlBlock;

#endif
