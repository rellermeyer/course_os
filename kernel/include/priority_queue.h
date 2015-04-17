/*
 * priority_queue.h
 *
 *  Created on: Apr 16, 2015
 *      Author: mwkurian
 */

#ifndef KERNEL_INCLUDE_PRIORITY_QUEUE_H_
#define KERNEL_INCLUDE_PRIORITY_QUEUE_H_

#define PRQ_DEBUG 0

typedef struct prq_node {
    int priority;
    void * data;
} prq_node;

typedef struct prq_handle {
    prq_node * heap;
    int size;
} prq_handle;

void prq_enqueue(prq_node node, prq_handle * queue);
prq_node prq_dequeue(prq_handle * queue);
void prq_init(prq_handle * queue, int n);

#endif /* KERNEL_INCLUDE_PRIORITY_QUEUE_H_ */
