#include <stdint.h>
//#include "../arch/arm/syscall_arch.h"
#include "stdint.h"

/***********************************
 *	    SYSCALLS for Malloc        *
 ************************************/

#define SYSCALL_MALLOC 13
#define SYSCALL_ALIGNED_ALLOC 14
#define SYSCALL_FREE 15

/**
 * malloc does a system call to allocate memory on the user heap
 *
 * @param  size of the block of memory allocated
 * @param  uint32_t size
 * @return returns a pointer to the allocated block of memory
 returns 0 if size is less than 1;
 */
void* malloc(uint32_t size);

/**
 * aligned alloc does a system call to allocate memory on the user heap
 * according to a specified alignemnt
 *
 * @param  size of the block of memory allocated, and alignment desired
 * @param  uint32_t size >= 0, uint32_t alignment >=0
 * @return returns a pointer to the allocated block of memory
 * 		   that is a multiple of the specified allignement
 *		   returns 0 if size or alignment are less than 1
 */
void* aligned_alloc(uint32_t size, uint32_t alignment);

/**
 * free does a system call to free an allocated block of memory on the heap
 *
 * @param  pointer to a block of memeory on the heap
 * @param  void* ptr
 * @return nothing returned
 */
void free(void* ptr);

