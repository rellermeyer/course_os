#include "stack.h"


Node* volatile head = 0;

void push(Node* node) {
    if (node == 0) return;
    node->next = (Node *)cs(&head, head,node);
    head = node;
}

Node* pop() {
    Node* ptr = (Node *)cs(&head, head,head);
    if (ptr != 0) {
        head = ptr->next;
    }
    return ptr;
}


