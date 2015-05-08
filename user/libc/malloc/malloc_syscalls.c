#include <stdint.h>
#include <malloc_syscalls.h>
#include "../arch/arm/syscall_arch.h"
#include "stdint.h"

/**
 * malloc does a system call to allocate memory on the user heap
 *
 * @param  size of the block of memory allocated
 * @param  uint32_t size > 0
 * @return returns a pointer to the allocated block of memory
 		   returns 0 if size is less than 1
 */ 
void* malloc(uint32_t size)
{
	if (size <= 0)
	{
		return 0;
	}
	//size will be passed into R0 for the syscall.
	long retval = __syscall1(SYSCALL_MALLOC,(long) size);

	//returns a pointer to begginning of allocated block(s)
	return (void*)retval;
}

/**
 * aligned alloc does a system call to allocate memory on the user heap
 * according to a specified alignemnt
 *
 * @param  size of the block of memory allocated, and alignment desired
 * @param  uint32_t size >0, uint32_t alignment >0
 * @return returns a pointer to the allocated block of memory
 * 		   that is a multiple of the specified allignement
 *		   returns 0 if size or alignment are less than 1
 */
void* aligned_alloc(uint32_t size, uint32_t alignment)
{
	if (size <= 0 || alignment <= 0)
	{
		return 0;
	}
	//syscall with two arguments
	//size passed into R0, alignment passed into R1
	long retval = __syscall2(SYSCALL_ALIGNED_ALLOC, (long) size, (long) alignment);
	//returns a pointer to begginning of allocated block(s)
	return (void*) retval;
}

/**
 * free does a system call to free an allocated block of memory on the heap
 *
 * @param  pointer to a block of memeory on the heap, does nothing if pointer is null
 * @param  void* ptr
 * @return nothing returned
 */
void free(void* ptr)
{
	//if null pointer, do nothing
	if (ptr == 0)
	{
		return 0;
	}

	//pointer of allocated block passed into R0
	long retval = __syscall1(SYSCALL_FREE,(long)ptr);

	//nothing returned
}

 