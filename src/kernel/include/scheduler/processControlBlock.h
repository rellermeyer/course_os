#ifndef PROCESSCONTROLBLOCK_H
#define PROCESSCONTROLBLOCK_H
#include <vas2.h>
#include <stdint.h>

typedef struct ProcessControlBlock {
    int id;
    int parent;

    struct ProcessControlBlock * prev;
    struct ProcessControlBlock * next;
    struct vas2 * vas;
    int wakeupTime;
} ProcessControlBlock;

ProcessControlBlock * findNode(int id, ProcessControlBlock * list);

void addPCBNodeBefore(ProcessControlBlock * oldNode, ProcessControlBlock * newNode);
void addPCBNodeAfter(ProcessControlBlock * oldNode, ProcessControlBlock * newNode);

ProcessControlBlock * removePCBNode(ProcessControlBlock *);
ProcessControlBlock * createPCB(int parent);
#endif
