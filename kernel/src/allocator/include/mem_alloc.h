#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

#include <allocator.h>
#include <stdint.h>


void init_heap();
void * allocate(uint32_t size);
void deallocate(void * ptr);

uint32_t mem_get_heap_size();

// get size of allocated blocks
uint32_t allocation_size(void * ptr);
uint32_t proc_blocksize(void * ptr);

size_t heap_end;

#endif