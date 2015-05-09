/*
 Log
 4/2: Adjusted bump allocation algorithm: Sean V, Faseeh A, John G, Taylor S
 4/7: Fixed mem_alloc again works properly: Sean V, Faseeh A, Taylor S.

 */
#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <stdint.h>
#include <global_defs.h>

#define MEM_START 0x500000

typedef uint32_t (*heap_extend_handler)(uint32_t amount);

typedef struct alloc_handle {
    uint32_t *heap;
    uint32_t heap_size;
    heap_extend_handler extend_handler;
} alloc_handle;

alloc_handle* alloc_create(uint32_t * heap, uint32_t size,
        heap_extend_handler extend_handler);
alloc_handle* alloc_create_fixed(uint32_t * buffer, uint32_t buffer_size);
void* alloc_allocate(alloc_handle * allocator, uint32_t size);
void alloc_deallocate(alloc_handle* allocator, void* ptr);
uint32_t* alloc_get_heap(alloc_handle* allocator);
uint32_t alloc_get_heap_size(alloc_handle* allocator);
int alloc_check(alloc_handle* allocator);

#endif
