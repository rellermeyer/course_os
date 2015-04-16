#include "../include/mem_alloc.h"
#include "../include/priority_queue.h"

void __prq_insert(prq_node aNode, prq_node* heap, int size);
void __prq_shift_down(prq_node* heap, int size, int idx);
prq_node __prq_remove_min(prq_node * heap, int size);
prq_node __prq_remove_min(prq_node * heap, int size);

void __prq_insert(prq_node aNode, prq_node* heap, int size) {
	int idx;
	prq_node tmp;
	idx = size + 1;
	heap[idx] = aNode;
	while (heap[idx].value < heap[idx / 2].value && idx > 1) {
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
			if (heap[cidx].value > heap[cidx + 1].value) {
				++cidx;
			}
		}

		//swap if necessary
		if (heap[cidx].value < heap[idx].value) {
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
	int cidx;
	prq_node rv = heap[1];
	if (PRQ_DEBUG) {
		printf("%d:%d:%d\n", size, heap[1].value, heap[size].value);
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

void init_queue(prq_handle * queue, int n) {
	queue->size = 0;
	queue->heap = (prq_node*) malloc(sizeof(prq_node) * (n + 1));
}
