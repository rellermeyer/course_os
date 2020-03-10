#include <stdio.h>
#include <allocator.h>
#include <string.h>
#include <vm2.h>

heap_t * allocator;
size_t heap_end;

/// Initializes the heap by asking for HEAP_INIT_SIZE of pages, and putting the heap datastructure there.
/// After the heap is initialized [kmalloc] and related functions can be used.
void init_heap() {
    heap_end = KERNEL_HEAP_BASE;

    // Allocate space for the heap_t struct. is too large but at least big enough.
    void * ret = vm2_allocate_page(kernell1PageTable, KERNEL_HEAP_BASE, false, (struct PagePermission) {
            .access = KernelRW,
            .executable = false
    }, NULL);
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

    while(heap_end < (heap_start + HEAP_INIT_SIZE + PAGE_SIZE)) {
        if (vm2_allocate_page(kernell1PageTable, heap_end, false, (struct PagePermission) {
                .access = KernelRW,
                .executable = false
        }, NULL) == NULL) {
            FATAL("Allocation of the kernel heap went wrong!");
        }

        heap_end += PAGE_SIZE;
    }

    assert(heap_end >= (heap_start + HEAP_INIT_SIZE));

    create_heap(heap, heap_start);
    allocator = heap;

    INFO("Heap successfully initialized.");
}

/// Internal wrapper around heap_alloc. Should only be indirectly used through kmalloc/krealloc/kcalloc/kfree
void *allocate(uint32_t size) {
    return heap_alloc(allocator, size);
}

/// Internal wrapper around heap_free. Should only be indirectly used through kmalloc/krealloc/kcalloc/kfree
void deallocate(void *ptr) {
    return heap_free(allocator, ptr);
}

/// Internal function to get the size of an allocation. Should only be indirectly used through kmalloc/krealloc/kcalloc/kfree
uint32_t allocation_size(void* ptr) {
    return get_alloc_size(ptr);
}

/// Internal function to get the global allocator. Should only be indirectly used through kmalloc/krealloc/kcalloc/kfree
heap_t * mem_get_allocator(){
    return allocator;
}

/// Internal function to get the size of the heap. Should only be indirectly used through kmalloc/krealloc/kcalloc/kfree
uint32_t mem_get_heap_size(){
    return allocator->end - allocator->start;
}
