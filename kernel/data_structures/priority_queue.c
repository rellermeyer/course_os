#include "../include/priority_queue.h"
#include "../include/mem_alloc.h"
#include "klibc.h"

void __prq_insert(prq_node node, prq_node* heap, int size);
void __prq_shift_down(prq_node* heap, int size, int idx);
prq_node __prq_remove_min(prq_node * heap, int size);
prq_node __prq_remove_min(prq_node * heap, int size);

void __prq_insert(prq_node node, prq_node* heap, int size) {
	int idx;
	prq_node tmp;
	idx = size + 1;
	heap[idx] = node;
	while (heap[idx].priority < heap[idx / 2].priority && idx > 1) {
		tmp = heap[idx];
		heap[idx] = heap[idx / 2];
		heap[idx / 2] = tmp;
		idx /= 2;
	}
}

void __prq_shift_down(prq_node* heap, int size, int idx) {

	int cidx;        //index for child
	prq_node tmp;
	for (;;) {
		cidx = idx * 2;
		if (cidx > size) {
			break;   //it has no child
		}
		if (cidx < size) {
			if (heap[cidx].priority > heap[cidx + 1].priority) {
				++cidx;
			}
		}

		//swap if necessary
		if (heap[cidx].priority < heap[idx].priority) {
			tmp = heap[cidx];
			heap[cidx] = heap[idx];
			heap[idx] = tmp;
			idx = cidx;
		} else {
			break;
		}
	}
}

prq_node __prq_remove_min(prq_node * heap, int size) {
	prq_node rv = heap[1];
	if (PRQ_DEBUG) {
		os_printf("%d:%d:%d\n", size, heap[1].priority, heap[size].priority);
	}
	heap[1] = heap[size];
	--size;
	__prq_shift_down(heap, size, 1);
	return rv;

}

void prq_enqueue(prq_node node, prq_handle * queue) {
	__prq_insert(node, queue->heap, queue->size);
	++queue->size;
}

prq_node prq_dequeue(prq_handle * queue) {
	prq_node rv = __prq_remove_min(queue->heap, queue->size);
	--queue->size;
	return rv;
}

// FIXME @CalvinBench
// Add support for variable length priority queues
// Add test cases as necessary
void prq_init(prq_handle * queue, int n) {
	queue->size = 0;
	queue->heap = (prq_node*) mem_alloc(sizeof(prq_node) * (n + 1));
}
