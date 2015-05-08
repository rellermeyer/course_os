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
#define PROC_START 0x90000000	


uint32_t init_heap();
void *allocate(uint32_t, uint32_t* /*unused*/, int32_t/*unused*/);
void deallocate(void*, uint32_t*/*unused*/, int32_t/*unused*/);

/**
 * Initializes a processes heap, in the same manner that the 
 * kernel heap is initialized
 *
 * @param  pointer to virtual address space (the process's VAS)
 * @param  struct vas* vas
 * @return returns status code (for error or for ok)
 */
uint32_t init_process_heap();

/**
 * Allocates memory on the heap for a process
 *
 * @param  size of allocated memory
 * @param  uint32_t size
 * @return a pointer to allocated memory
 */
void *proc_allocate(uint32_t);

/**
 * Deallocates memory on the heap for a process
 *
 * @param  pointer to allocated memory
 * @param  void* ptr
 * @return nothing
 */
void proc_deallocate(void*);
alloc_handle* mem_get_allocator();
int mem_check();
uint32_t mem_get_heap_size();

#endif
