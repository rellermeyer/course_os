#include <mem_alloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void kfree(void * ptr) {
    deallocate((uint32_t *)ptr);
}

void * kmalloc(uint32_t size) {
    void * block = (void *)allocate(size);
    return block;
}


// Allocates n * size portion of memory (set to 0) and returns it
void * kcalloc(size_t n, size_t size) {
    uint32_t total_size = n * size;
    void * block = kmalloc(total_size);

    if (block == NULL) {
        return NULL;
    } else {
        memset(block, 0, total_size);
        return block;
    }
}

uint32_t kmalloc_size(void * ptr) {
    return allocation_size(ptr);
}

// TODO: Implement in-place realloc if the next block is free.
// Resize memory pointed to by ptr to new size
void * krealloc(void * ptr, uint32_t newsize) {
    if (ptr == NULL) {
#if MEM_DEBUG
        TRACE("[MEM DEBUG] Realloc with nullptr");
#endif

        return kmalloc(newsize);
    }

    uint32_t oldsize = kmalloc_size(ptr);


    if (newsize == 0) {
        kfree(ptr);
        return NULL;
    } else if (newsize <= oldsize) {
        return ptr;
    } else {
        void * newptr = kmalloc(newsize);
        if (newptr) {
            memcpy(newptr, ptr, oldsize);
            kfree(ptr);
        }
        return newptr;
    }
}
