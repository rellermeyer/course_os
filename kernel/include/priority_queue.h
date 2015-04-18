/*
 * priority_queue.h
 *
 *  Created on: Apr 16, 2015
 *      Author: mwkurian
 */

#ifndef KERNEL_INCLUDE_PRIORITY_QUEUE_H_
#define KERNEL_INCLUDE_PRIORITY_QUEUE_H_

typedef struct prq_node {
    int index;  // FIXME double underscore
    int priority;
    void * data;
} prq_node;

typedef struct prq_handle {
    prq_node ** heap;
    int count;
} prq_handle;

void prq_enqueue(prq_handle * queue, prq_node * node);
prq_node * prq_dequeue(prq_handle * queue);
void prq_remove(prq_handle * queue, prq_node * node);
prq_handle* prq_create(int n);
void prq_free(prq_handle * queue);
int prq_count(prq_handle * queue);
prq_node * prq_peek(prq_handle * queue);
#endif /* KERNEL_INCLUDE_PRIORITY_QUEUE_H_ */
