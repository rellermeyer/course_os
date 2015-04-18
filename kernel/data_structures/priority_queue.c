#include "../include/priority_queue.h"
#include "klibc.h"

void __prq_shift_up(prq_handle * queue, int idx);
void __prq_shift_down(prq_handle * queue, int idx);

void __prq_shift_up(prq_handle * queue, int idx) {
    prq_node ** heap = queue->heap;
    prq_node * node, *parent;
    while (heap[idx]->priority < heap[idx / 2]->priority && idx > 1) {
        node = heap[idx];
        parent = heap[idx / 2];
        parent->index = idx;
        node->index = idx / 2;
        heap[idx] = parent;
        heap[idx / 2] = node;
        idx /= 2;
    }
}

void __prq_shift_down(prq_handle * queue, int idx) {
    int cidx;        //index for child
    prq_node ** heap = queue->heap;
    prq_node * node, *child;
    for (;;) {
        cidx = idx * 2;
        if (cidx > size) {
            break;   //it has no child
        }
        if (cidx < size) {
            if (heap[cidx]->priority > heap[cidx + 1]->priority) {
                ++cidx;
            }
        }
        //swap if necessary
        if (heap[cidx]->priority < heap[idx]->priority) {
            node = heap[cidx];
            child = heap[idx];
            node->index = idx;
            child->index = cidx;
            heap[cidx] = child;
            heap[idx] = node;
            idx = cidx;
        } else {
            break;
        }
    }
}

prq_node * prq_peek(prq_handle * queue) {
    if (queue) {
        if (queue->size > 0) {
            return queue->heap[1];
        }
    }

    return 0;
}

int prq_size(prq_handle * queue) {
    if (queue) {
        return queue->size;
    }

    return 0;
}

void prq_enqueue(prq_handle * queue, prq_node * node) {
    prq_node ** heap = queue->heap;
    int index = node->index = size + 1;
    heap[index] = node;
    ++queue->size;
    __prq_shift_up(heap, index);
}

void prq_dequeue(prq_handle * queue, prq_node * node) {
    prq_node ** heap = queue->heap;
    prq_node * end = heap[size];
    int index = (end->index = node->index);
    node->index = -1;
    heap[index] = end;
    heap[size] = 0;
    --queue->size;
    // FIXME optimize later
    __prq_shift_up(queue, index);
    __prq_shift_down(queue, index);
}

prq_node * prq_dequeue(prq_handle * queue) {
    prq_node ** heap = queue->heap;
    prq_node * top = heap[1];
    prq_node * end = heap[size];
    end->index = 1;
    top->index = -1;
    heap[1] = end;
    heap[size] = 0;
    --queue->size;
    __prq_shift_down(queue, 1);
    return top;
}

// FIXME @CalvinBench
// Add support for variable length priority queues
// Add test cases as necessary
void prq_init(prq_handle * queue, int n) {
    queue->size = 0;
    queue->heap = (prq_node**) kmalloc(sizeof(prq_node*) * (n + 1));
}
