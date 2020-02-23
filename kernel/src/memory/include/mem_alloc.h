#ifndef __MEM_ALLOC_H__
#define __MEM_ALLOC_H__

#include <allocator.h>
#include <stdint.h>

#define MEM_START 0x500000
#define PROC_START 0x90000000

struct vas;

uint32_t init_heap();
void *allocate(uint32_t size);
void deallocate(void *ptr);

/**
 * Initializes a processes heap, in the same manner that the
 * kernel heap is initialized
 *
 * @param  pointer to virtual address space (the process's VAS)
 * @param  struct vas* vas
 * @return returns status code (for error or for ok)
 */
uint32_t init_process_heap(struct vas* vas);

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
heap_t* mem_get_allocator();
uint32_t mem_get_heap_size();

// get size of allocated blocks
uint32_t allocation_size(void* ptr);
uint32_t proc_blocksize(void* ptr);

#endif
