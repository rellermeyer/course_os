#include <stdint.h>
#include <malloc_syscalls.h>
#include "../arch/arm/syscall_arch.h"
#include "stdint.h"

/*Malloc: Allocates a block of size bytes of memory. 
Returns a pointer to the beggining of the block. If
size <= 0, a null pointer is returned.*/

void* malloc(uint32_t size)
{
	if (size <= 0)
	{
		//os_printf("Inavlid size: must be greater than 0 bytes\n");
		return 0;
	}
	//size will be passed into R0 for the syscall.
	long retval = __syscall1(SYSCALL_MALLOC,(long) size);

	//returns a pointer to begginning of allocated block(s)
	return (void*)retval;
}

/*alligned_alloc: Allocates an alligned block of memory, returns
pointer to the 
If size <= 0, a null pointer is returned.*/

void* alligned_alloc(uint32_t num, uint32_t size)
{
	if (size <= 0 || num <= 0)
	{
		//os_printf("Inavlid size and/or byte number: must be greater than 0\n");
		return 0;
	}
	//syscall with two arguments
	//num passed into R0, size passed into R1
	long retval = __syscall2(SYSCALL_ALLIGNED_ALLOC, (long) num, (long) size);
	//returns a pointer to begginning of allocated block(s)
	return (void*) retval;
}

/*Free: Deallocates a block of memory previously
allocated by Malloc, alligned_alloc, etc.. */

void free(void* ptr)
{
	//if null pointer, do nothing
	if (ptr == 0)
	{
		//os_printf("null pointer passed as parameter\n");
		return 0;
	}

	//pointer of allocated block passed into R0
	long retval = __syscall1(SYSCALL_FREE,(long)ptr);
	
	//negative retval for trying to free a block that doesnt exist
	if (retval < 0)
	{
		//os_printf("Memory you are trying to free has
			//not been previously allocated\n");
	}

	//nothing returned for free
}

 