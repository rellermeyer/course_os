#include <mem_alloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void *kmalloc_aligned(uint32_t size, uint32_t alignment) {
    void *block;
    void *ptr;

    switch (alignment) {
        case 4:
            block = kmalloc(size + 4);
            ptr = (void *) (((uint32_t) block + 4) & ~0x3);
            return ptr;
        case 1024:
            block = kmalloc(size + 1024);
            ptr = (void *) (((uint32_t) block + 1024) & ~0x1ff);
            return ptr;
        case 4096:
            block = kmalloc(size + 4096);
            ptr = (void *) (((uint32_t) block + 4096) & ~0x7ff);
            return ptr;
        case 16 * 1024:
            block = kmalloc(size + 16 * 1024);
            ptr = (void *) (((uint32_t) block + 16 * 1024) & ~0x1fff);
            return ptr;
        default:
            return kmalloc(size);
    }
}

void kfree(void *ptr) {
    deallocate((uint32_t *) ptr);
}

void *kmalloc(uint32_t size) {
    void *block = (void *) allocate(size);
    return block;
}


// Allocates n * size portion of memory (set to 0) and returns it
void *kcalloc(uint32_t n, uint32_t size) {
    uint32_t total_size = n * size;
    void *block = kmalloc(total_size);

    return block ? memset(block, 0, total_size) : NULL;
}

uint32_t kmalloc_size(void* ptr) {
    return allocation_size(ptr);
}

// FIXME: Implement kmalloc_size() (or something like it)
// TODO: Implement in-place realloc if the next block is free.
// Resize memory pointed to by ptr to new size
void * krealloc(void *ptr, uint32_t newsize) {
//    os_printf("Size: %u, newsize: %u\n", kmalloc_size(ptr), size);

    uint32_t oldsize = kmalloc_size(ptr);

    if (newsize == 0) {
        kfree(ptr);
        return NULL;
    } else if (ptr == NULL) {
        return kmalloc(newsize);
    } else if (newsize <= oldsize) {
        return ptr;
    } else {
        void *newptr = kmalloc(newsize);
        if (newptr) {
            memcpy(newptr, ptr, oldsize);
            kfree(ptr);
        }
        return newptr;
    }
}

/**
 * umalloc allocates memory on the user heap
 *
 * @param  size of the block of memory allocated
 * @param  uint32_t size
 * @return returns a pointer to the allocated block of memory
 */
void *umalloc(uint32_t size) {
    void *block = (void *) proc_allocate(size);
    return block;
}

/**
 * ualigned alloc allocates memory on the user heap
 * according to a specified alignemnt
 *
 * @param  size of the block of memory allocated, and alignment desired
 * @param  uint32_t size, uint32_alignment
 * @return returns a pointer to the allocated block of memory
 * 		   that is a multiple of the specified allignement
 */

void *ualigned_alloc(uint32_t size, uint32_t alignment) {
    void *block;
    void *ptr;

    switch (alignment) {
        case 4:
            block = umalloc(size + 4);
            ptr = (void *) (((uint32_t) block + 4) & ~0x3);
            return ptr;
        case 1024:
            block = umalloc(size + 1024);
            ptr = (void *) (((uint32_t) block + 1024) & ~0x1ff);
            return ptr;
        case 4096:
            block = umalloc(size + 4096);
            ptr = (void *) (((uint32_t) block + 4096) & ~0x7ff);
            return ptr;
        case 16 * 1024:
            block = umalloc(size + 16 * 1024);
            ptr = (void *) (((uint32_t) block + 16 * 1024) & ~0x1fff);
            return ptr;
        default:
            return umalloc(size);
    }
}

/**
 * free's an allocated block of memory on the heap
 *
 * @param  pointer to a block of memeory on the heap
 * @param  void* ptr
 * @return nothing returned
 */
void ufree(void *ptr) {
    proc_deallocate((uint32_t *) ptr);
}
