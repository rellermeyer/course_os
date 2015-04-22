
/*
Log
4/2: Adjusted bump allocation algorithm: Sean V, Faseeh A, John G, Taylor S
4/7: Fixed mem_alloc again works properly: Sean V, Faseeh A, Taylor S.

*/
#ifndef __MEM_ALLOC_H__
#define __MEM_ALLOC_H__

#include <stdint.h>
#include <global_defs.h>

#define MEM_START 0x500000

typedef uint32_t* (*heap_extend_handler)(uint32_t amount);

typedef struct alloc_handle {
    uint32_t *heap;
    uint32_t heap_size;
    heap_extend_handler extend_handler;
    uint32_t __align_0; // help align to 64-bit boundary. not sure if needed?
} __attribute__((packed)) alloc_handle;
 
alloc_handle* alloc_create(uint32_t * heap, uint32_t size, heap_extend_handler extend_handler);

void *aligned_mem_alloc(size_t, size_t);


void *init_heap();
void *allocate(uint32_t, uint32_t*, int32_t);
void deallocate(void*, uint32_t*, int32_t);

int mcheck();
void test_allocate();

#endif
