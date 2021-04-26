#ifndef KERNEL_INCLUDE_PRIORITY_QUEUE_H_
#define KERNEL_INCLUDE_PRIORITY_QUEUE_H_

/*
 * Priority queue implementation.
 * The element with the lowest priority value will get dequeued first.
 * Supports duplicate priorities.
 */

typedef struct prq_node {
    int __index;
    int priority;
    void * data;
} prq_node;

typedef struct prq_handle {
    prq_node ** __heap;
    int __count;
    int __heap_size;
} prq_handle;

void prq_enqueue(prq_handle * queue, prq_node * node);
prq_node * prq_dequeue(prq_handle * queue);
void prq_remove(prq_handle * queue, prq_node * node);
prq_handle * prq_create();
prq_handle * prq_create_fixed(int n);
void prq_free(prq_handle * queue);
int prq_count(prq_handle * queue);
prq_node * prq_peek(prq_handle * queue);
prq_node * prq_create_node();
void prq_free_node(prq_node * node);

#endif
