#include <klibc.h>
#include <mem_alloc.h>
#include <allocator.h>
#include <string.h>
#include <vm2.h>

heap_t * allocator;
size_t heap_end;

/*
 * The kernel heap is organized in blocks. Each block has a header and a
 * footer each a 4 byte integer, at the beginning and end of the block.
 * The header and footer both specify the size of the block in question.
 * Used blocks are indicated by the heap header and the heap footer being
 * negative.
 *
 * To allocate a block, we start at the first block and walk through each
 * one, finding the first free block large enough to support a header,
 * footer, and the size requested.
 *
 * To free a block, we do a bunch of merging stuff to check to see if we
 * should merge with the blocks on the left or right of us, respectively.
 */

void init_heap() {
    heap_end = KERNEL_HEAP_BASE;

    // Allocate space for the heap_t struct. is too large but at least big enough.
    void * ret = vm2_allocate_kernel_page(kernell1PageTable, KERNEL_HEAP_BASE, false, 0);
    if (ret == NULL) {
        FATAL("Couldn't allocate page for the kernel heap");
    }

    heap_t * heap = (heap_t *) KERNEL_HEAP_BASE;

    memset(heap, 0, sizeof(heap_t));

    heap_end += sizeof(heap_t);


    for(int i = 0; i < BIN_COUNT; i++) {
        heap->bins[i] = (bin_t *) heap_end;
        memset(heap->bins[0], 0, sizeof(bin_t));
        heap_end += sizeof(bin_t);
    }

    assert(KERNEL_HEAP_BASE + sizeof(heap_t) + BIN_COUNT * sizeof(bin_t) == heap_end);

    // current heap end.
    heap_end = ALIGN(heap_end, PAGE_SIZE);
    size_t heap_start = heap_end;

    while(heap_end < (heap_start + HEAP_INIT_SIZE)) {
        if (vm2_allocate_kernel_page(kernell1PageTable, heap_end, false, 0) == NULL) {
            FATAL("Allocation of the kernel heap went wrong!");
        }

        heap_end += PAGE_SIZE;
    }

    assert(heap_end >= (heap_start + HEAP_INIT_SIZE));

    create_heap(heap, heap_start);
    allocator = heap;

    INFO("Heap successfully initialized.");
}

void *allocate(uint32_t size) {
    return heap_alloc(allocator, size);
}

void deallocate(void *ptr) {
    return heap_free(allocator, ptr);
}

uint32_t allocation_size(void* ptr) {
    return get_alloc_size(ptr);
}

heap_t * mem_get_allocator(){
    return allocator;
}

uint32_t mem_get_heap_size(){
    return allocator->end - allocator->start;
}
