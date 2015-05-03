#include <stdint.h>
//#include "../arch/arm/syscall_arch.h"
#include "stdint.h"

/***********************************
*	    SYSCALLS for Malloc        *
************************************/

#define SYSCALL_SET_PERM 7 
#define SYSCALL_MEM_MAP 8
#define SYSCALL_MALLOC 13
#define SYSCALL_FREE 14


/***********************************
*	   Functions for Malloc        *
************************************/


/*Malloc: Allocates a block of size bytesof memory. 
Returns a pointer to the beggining of the block */  
void* malloc(uint32_t size);

/*Calloc: Allocates a block of memory for an array of 
num elements, each of them size bytes long. All bits
are initalized to zero. Returns a pointer of the block*/
void* calloc(uint32_t num, uint32_t size);

/*Free: Deallocates a block of memory previously
allocated by Malloc, Calloc, etc.. */
void free(void* ptr);

