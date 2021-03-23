#include <processControlBlock.h>
#include <stdio.h>
#include <stdlib.h>

int idCounter = 0;

ProcessControlBlock * findNode(int id, ProcessControlBlock * list) {
    for (ProcessControlBlock * i = list; i->next != list && i->next != NULL; i = i->next)
        if (i->id == id) { return i; }
    return NULL;
}

void addPCBNodeBefore(ProcessControlBlock * oldNode, ProcessControlBlock * newNode) {
    if (oldNode->prev != NULL) {
        oldNode->prev->next = newNode;
        newNode->prev = oldNode->prev;
    }
    oldNode->prev = newNode;
    newNode->next = oldNode;
}

void addPCBNodeAfter(ProcessControlBlock * oldNode, ProcessControlBlock * newNode) {
    if (oldNode->next != NULL) {
        oldNode->next->prev = newNode;
        newNode->next = oldNode->next;
    }
    oldNode->next = newNode;
    newNode->prev = oldNode;
}

ProcessControlBlock * removePCBNode(ProcessControlBlock * node) {
    if (node->prev != NULL) node->prev->next = node->next;
    if (node->next != NULL) node->next->prev = node->prev;
    return node;
}

ProcessControlBlock * createPCB(int parent) {
    ProcessControlBlock * processControlBlock = (ProcessControlBlock *)kmalloc(sizeof(ProcessControlBlock));

    *processControlBlock = (ProcessControlBlock) {
        .id = idCounter++,
        .parent = parent,
        .vas = create_vas()
    };
    return processControlBlock;
}
