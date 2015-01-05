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

uint32_t *heap;
uint32_t heap_size;

void *mem_alloc(uint32_t);
void *aligned_mem_alloc(size_t, size_t);

void *init_heap(uint32_t);
void *allocate(uint32_t, uint32_t*, int32_t);
void deallocate(void*, uint32_t*, int32_t);

void mcheck();

#endif
