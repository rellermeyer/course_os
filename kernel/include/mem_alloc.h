/*
Log
4/2: Adjusted bump allocation algorithm: Sean V, Faseeh A, John G, Taylor S
4/7: Fixed mem_alloc again works properly: Sean V, Faseeh A, Taylor S.

*/
#ifndef __MEM_ALLOC_H__
#define __MEM_ALLOC_H__

#include "allocator.h"
#include <stdint.h>
#include <global_defs.h>

#define MEM_START 0x500000

uint32_t init_heap();
void *allocate(uint32_t, uint32_t* /*unused*/, int32_t/*unused*/);
void deallocate(void*, uint32_t*/*unused*/, int32_t/*unused*/);
alloc_handle* mem_get_allocator();
int mem_check();
uint32_t mem_get_heap_size();

#endif
