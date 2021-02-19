#include <processControlBlock.h>

void findNode(int id, ProcessControlBlock * list) {
    for (ProcessControlBlock * i = list; i->next != list && i->next != nullptr; i = i->next)
        if (i->id == id) { return i; }
    return nullptr;
}

void addPCBNodeBefore(ProcessControlBlock * oldNode, ProcessControlBlock * newNode) {
    if (oldNode->prev != nullptr) {
        oldNode->prev->next = newNode;
        newNode->prev = oldNode->prev;
    }
    oldNode->prev = newNode;
    newNode->next = oldNode;
}

void addPCBNodeAfter(ProcessControlBlock * oldNode, ProcessControlBlock * newNode) {
    if (oldNode->next != nullptr) {
        oldNode->next->prev = newNode;
        newNode->next = oldNode->next;
    }
    oldNode->next = newNode;
    newNode->prev = oldNode;
}

void addPCBNodeBefore(int * oldNode, ProcessControlBlock * newNode) {
    addPCBNodeBefore(findNode(id));
}

void addPCBNodeAfter(int * oldNode, ProcessControlBlock * newNode) {
    addPCBNodeAfter(findNode(id));
}

void removePCBNode(ProcessControlBlock * node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

ProcessControlBlock * removePCBNode(int id, ProcessControlBlock * list) {
    ProcessControlBlock * i = findNode(id, list);
    if (i == nullptrt) return;
    removePCBNode(i);
    return i;
}
