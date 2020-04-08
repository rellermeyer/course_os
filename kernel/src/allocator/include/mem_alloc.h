#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

#include <allocator.h>
#include <stdint.h>


void init_heap();
void * allocate(uint32_t size);
void deallocate(void * ptr);

buddy_allocator * mem_get_allocator();
uint32_t mem_get_heap_size();

uint32_t allocation_size(void * ptr);

size_t heap_end;

#endif
