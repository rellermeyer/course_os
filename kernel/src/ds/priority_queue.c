#include <math.h>
#include <priority_queue.h>
#include <stdio.h>
#include <stdlib.h>

#define AMORTIZED_CONSTANT 2
#define DEFAULT_COUNT 10

// Internal implementation functions
void __prq_shift_up(prq_handle * queue, int idx);
void __prq_shift_down(prq_handle * queue, int idx);

void __prq_shift_up(prq_handle * queue, int idx) {
    prq_node ** heap = queue->__heap;
    prq_node *node, *parent;
    while (idx > 1 && heap[idx]->priority < heap[idx / 2]->priority) {
        node = heap[idx];
        parent = heap[idx / 2];
        parent->__index = idx;
        node->__index = idx / 2;
        heap[idx] = parent;
        heap[idx / 2] = node;
        idx /= 2;
    }
}

void __prq_shift_down(prq_handle * queue, int idx) {
    int cidx;  // index for child
    prq_node ** heap = queue->__heap;
    prq_node *node, *child;
    for (;;) {
        cidx = idx * 2;
        if (cidx > queue->__count) {
            break;  // it has no child
        }
        if (cidx < queue->__count) {
            if (heap[cidx]->priority > heap[cidx + 1]->priority) { ++cidx; }
        }
        // swap if necessary
        if (heap[cidx]->priority < heap[idx]->priority) {
            node = heap[cidx];
            child = heap[idx];
            node->__index = idx;
            child->__index = cidx;
            heap[cidx] = child;
            heap[idx] = node;
            idx = cidx;
        } else {
            break;
        }
    }
}

prq_node * prq_create_node() {
    return kmalloc(sizeof(prq_node));
}

void prq_free_node(prq_node * node) {
    assert(node != NULL);

    kfree(node);
}

prq_node * prq_peek(prq_handle * queue) {
    assert(queue != NULL);

    if (queue->__count > 0) {
        return queue->__heap[1];
    }
    return NULL;
}

int prq_count(prq_handle * queue) {
    assert(queue != NULL);

    return queue->__count;
}

void __prq_amortize(prq_handle * queue) {
    const int new_heap_size = queue->__heap_size * AMORTIZED_CONSTANT;
    queue->__heap = krealloc(queue->__heap, sizeof(prq_node *) * new_heap_size);
    queue->__heap_size = new_heap_size;
}

void prq_enqueue(prq_handle * queue, prq_node * node) {
    assert(queue != NULL);
    assert(node != NULL);

    prq_node ** const heap = queue->__heap;
    const int index = (node->__index = queue->__count + 1);
    heap[index] = node;
    ++(queue->__count);
    __prq_shift_up(queue, index);
    if (queue->__count + 1 == queue->__heap_size) {
        __prq_amortize(queue);
    }
}

void prq_remove(prq_handle * queue, prq_node * node) {
    assert(queue != NULL);
    assert(node != NULL);

    if (queue->__count == 0) {
        return;
    }
    prq_node ** heap = queue->__heap;
    prq_node * end = heap[queue->__count];
    int index = (end->__index = node->__index);
    if (index == -1) {
        return;
    }
    node->__index = -1;
    heap[index] = end;
    heap[queue->__count + 1] = 0;
    --queue->__count;
    // FIXME optimize later
    __prq_shift_up(queue, index);
    __prq_shift_down(queue, index);
}

prq_node * prq_dequeue(prq_handle * queue) {
    assert(queue != NULL);

    if (queue->__count == 0) {
        return 0;
    }
    prq_node ** heap = queue->__heap;
    prq_node * top = heap[1];
    prq_node * end = heap[queue->__count];
    end->__index = 1;
    top->__index = -1;
    heap[1] = end;
    heap[queue->__count] = 0;
    --queue->__count;
    __prq_shift_down(queue, 1);
    return top;
}

prq_handle * prq_create_fixed(int n) {
    assert(n >= 1);

    prq_handle * queue = kmalloc(sizeof(prq_handle));
    queue->__count = 0;
    queue->__heap_size = n + 1;
    queue->__heap = kmalloc(sizeof(prq_node *) * queue->__heap_size);
    return queue;
}

prq_handle * prq_create() {
    return prq_create_fixed(DEFAULT_COUNT);
}

void prq_free(prq_handle * queue) {
    assert(queue != NULL);
    assert(queue->__count == 0);

    kfree(queue->__heap);
    kfree(queue);
}
