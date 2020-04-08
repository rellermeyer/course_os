#include <allocator.h>
#include <stdio.h>
#include <string.h>
#include <vm2.h>

buddy_alloc_t * allocator;
size_t allocator_size;
size_t heap_end;


void init_heap() {
    heap_end = KERNEL_HEAP_BASE;

    // Allocate space for the heap_t struct. is too large but at least big enough.
    void * ret = vm2_allocate_page(kernell1PageTable,
                                   KERNEL_HEAP_BASE,
                                   false,
                                   (struct PagePermission){.access = KernelRW, .executable = false},
                                   NULL);
    if (ret == NULL) { FATAL("Couldn't allocate page for the kernel heap"); }

    buddy_alloc_t * heap = (buddy_alloc_t * )KERNEL_HEAP_BASE;

    memset(heap, 0, sizeof(buddy_alloc_t));

    heap_end += sizeof(buddy_alloc_t);

    size_t heap_start = heap_end;
    while (heap_end < (heap_start + allocator_size)) {
        if (vm2_allocate_page(kernell1PageTable,
                              heap_end,
                              false,
                              (struct PagePermission){.access = KernelRW, .executable = false},
                              NULL) == NULL) {
            FATAL("Allocation of the kernel heap went wrong!");
        }

        heap_end += PAGE_SIZE;
    }

    assert(heap_end >= (heap_start + allocator_size));

    buddy_init(allocator, allocator_size);
    allocator = heap;

    INFO("Heap successfully initialized.");
}

/// Internal wrapper around heap_alloc. Should only be indirectly used through
/// kmalloc/krealloc/kcalloc/kfree
void * allocate(uint32_t size) {
    return buddy_alloc(allocator, size);
}

/// Internal function to get the size of an allocation. Should only be indirectly used through
/// kmalloc/krealloc/kcalloc/kfree
uint32_t allocation_size(void * ptr) {
    return get_alloc_size(ptr);
}

/// Internal wrapper around heap_free. Should only be indirectly used through
/// kmalloc/krealloc/kcalloc/kfree
void deallocate(void * ptr) {
    return buddy_dealloc(allocator, ptr, allocation_size(ptr));
}

/// Internal function to get the global oldlocator. Should only be indirectly used through
/// kmalloc/krealloc/kcalloc/kfree
buddy_alloc_t *  mem_get_allocator() {
    return allocator;
}

/// Internal function to get the size of the heap. Should only be indirectly used through
/// kmalloc/krealloc/kcalloc/kfree
uint32_t mem_get_heap_size() {
    return allocator_size;
}
