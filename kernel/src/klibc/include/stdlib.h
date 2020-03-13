#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

void * kmalloc(uint32_t size);
void * kmalloc_aligned(uint32_t size, uint32_t alignment);
void kfree(void * ptr);
void * krealloc(void * ptr, uint32_t size);
void * kcalloc(size_t n, size_t size);

/**
 * umalloc allocates memory on the user heap
 *
 * @param  size of the block of memory allocated
 * @param  uint32_t size
 * @return returns a pointer to the allocated block of memory
 */
void * umalloc(uint32_t size);  // does user level malloc work

/**
 * ualigned alloc allocates memory on the user heap
 * according to a specified alignemnt
 *
 * @param  size of the block of memory allocated, and alignment desired
 * @param  uint32_t size, uint32_alignment
 * @return returns a pointer to the allocated block of memory
 * 		   that is a multiple of the specified allignement
 */
void * ualigned_alloc(uint32_t size, uint32_t alignment);  // does user level aligned_alloc work

/**
 * free's an allocated block of memory on the heap
 *
 * @param  pointer to a block of memeory on the heap
 * @param  void* ptr
 * @return nothing returned
 */
void ufree(void *);  // does user level free work


#endif
