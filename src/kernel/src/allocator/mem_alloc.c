
#include <allocator.h>
#include <stdio.h>
#include <string.h>
#include <vm2.h>

heap_t * allocator;
size_t heap_end;

/// Initializes the heap by asking for HEAP_INIT_SIZE of pages, and putting the heap datastructure
/// there. After the heap is initialized [kmalloc] and related functions can be used.
void init_heap() {
    heap_end = KERNEL_HEAP_BASE;

    // current heap end.
    size_t heap_start = heap_end;

    while (heap_end < (heap_start + HEAP_SIZE)) {
        if (vm2_allocate_page(kernell1PageTable,
                              heap_end,
                              false,
                              (struct PagePermission){.access = KernelRW, .executable = false},
                              NULL) == NULL) {
            FATAL("Allocation of the kernel heap went wrong!");
        }

        heap_end += PAGE_SIZE;
    }

    assert(heap_end >= (heap_start + HEAP_SIZE));

    buddy_allocator * heap = (buddy_allocator *) heap_start;

    assert(buddy_init(heap, HEAP_SIZE) != NULL)
    allocator = heap;

    INFO("Heap successfully initialized.");
}

/// Internal wrapper around heap_alloc. Should only be indirectly used through
/// kmalloc/krealloc/kcalloc/kfree
void * allocate(uint32_t size) {
    return buddy_alloc(allocator, size);
}

/// Internal wrapper around heap_free. Should only be indirectly used through
/// kmalloc/krealloc/kcalloc/kfree
void deallocate(void * ptr) {
    return buddy_dealloc(allocator, (size_t)ptr);
}

/// Internal function to get the size of an allocation. Should only be indirectly used through
/// kmalloc/krealloc/kcalloc/kfree
uint32_t allocation_size(void * ptr) {
    return get_alloc_size((size_t)ptr);
}

/// Internal function to get the global allocator. Should only be indirectly used through
/// kmalloc/krealloc/kcalloc/kfree
heap_t * mem_get_allocator() {
    return allocator;
}

//
///// Internal function to get the size of the heap. Should only be indirectly used through
///// kmalloc/krealloc/kcalloc/kfree
uint32_t mem_get_heap_size() {
    return HEAP_SIZE;
}
