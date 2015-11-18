#include "klibc.h"
#include "data_structures/priority_queue.h"

#define AMORITIZED_CONSTANT 2
#define DEFAULT_COUNT 10

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
        if (cidx > queue->count) {
            break;   //it has no child
        }
        if (cidx < queue->count) {
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

prq_node * prq_create_node() {
    return kmalloc(sizeof(prq_node));
}

void prq_free_node(prq_node * node) {
    if (node) {
        kfree(node);
    }
}

prq_node * prq_peek(prq_handle * queue) {
    if (queue) {
        if (queue->count > 0) {
            return queue->heap[1];
        }
    }

    return 0;
}

int prq_count(prq_handle * queue) {
    if (queue) {
        return queue->count;
    }

    return 0;
}

void __prq_amoritize(prq_handle * queue) {
    int new_heap_size = queue->heap_size * AMORITIZED_CONSTANT;
    prq_node** new_heap = (prq_node**) kmalloc(
            sizeof(prq_node*) * new_heap_size);
    os_memcpy((uint32_t *) queue->heap, (uint32_t *) new_heap,
            (os_size_t) queue->heap_size * sizeof(prq_node*));
    kfree(queue->heap);
    queue->heap = new_heap;
    queue->heap_size = new_heap_size;
}

void prq_enqueue(prq_handle * queue, prq_node * node) {
    prq_node ** heap = queue->heap;
    int index = (node->index = queue->count + 1);
    heap[index] = node;
    ++queue->count;
    __prq_shift_up(queue, index);
    if (queue->count + 1 == queue->heap_size) {
        __prq_amoritize(queue);
    }
}

void prq_remove(prq_handle * queue, prq_node * node) {
    if (!queue->count) {
        return;
    }
    prq_node ** heap = queue->heap;
    prq_node * end = heap[queue->count];
    int index = (end->index = node->index);
    if (index == -1) {
        return;
    }
    node->index = -1;
    heap[index] = end;
    heap[queue->count + 1] = 0;
    --queue->count;
    // FIXME optimize later
    __prq_shift_up(queue, index);
    __prq_shift_down(queue, index);
}

prq_node * prq_dequeue(prq_handle * queue) {
    if (!queue->count) {
        return 0;
    }
    prq_node ** heap = queue->heap;
    prq_node * top = heap[1];
    prq_node * end = heap[queue->count];
    end->index = 1;
    top->index = -1;
    heap[1] = end;
    heap[queue->count] = 0;
    --queue->count;
    __prq_shift_down(queue, 1);
    return top;
}

prq_handle * prq_create_fixed(int n) {
    n = MAX(1, n);
    prq_handle * queue = (prq_handle*) kmalloc(sizeof(prq_handle));
    queue->count = 0;
    queue->heap_size = n + 1;
    queue->heap = (prq_node**) kmalloc(sizeof(prq_node*) * queue->heap_size);
    return queue;
}

prq_handle * prq_create() {
    return prq_create_fixed(DEFAULT_COUNT);
}

void prq_free(prq_handle * queue) {
    if (queue) {
        kfree(queue->heap);
        kfree(queue);
    }
}
