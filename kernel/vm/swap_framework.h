#ifndef __VM_SWAP_FRAMEWORK_H
#define __VM_SWAP_FRAMEWORK_H

#include "vm.h"
#include "memory.h"
#include "klibc.h"
#include "./frame.h"

/* Function: swap_framework
 * Purpose: To present an API for swapping pages to various swap spaces
 * so that pages may be stored independently of other related operations
 *
 * EXAMPLE OF VIRTUAL MEMORY ID/ADDRESS FROM SWAP SPACE
 *	_____________________________________________________________
 *	|| [24 bit] SWAP SPACE ID | [8 bit] SWAP SPACE SPECIFIC ID ||
 *	¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 *	Total: 32 Bit Address
 *	therefore, there are 256 possible swap spaces possible
 *	w/ 2^12 page entries each (Assuming 4kB pages)
 */

#define SWAP_SPACES (1<<8)
#define PAGE_ENTRIES (1<<12) //Assuming 4kB Pages right now

struct swap_info {
	struct swap_info* next;
	uint32_t higher_bits;
	uint8_t lower_bits;
	uint32_t swap_map[PAGE_ENTRIES]; 
	uint32_t memory_count;
};


// swap_init initializes all necessary values, linkedlists and 
void swap_init(os_size_t);


/* store_page will store a page to media - not main memory - storage, e.g. HDD
 *	VARIABLES
 * void* page -> data to be paged
 * os_size_t pagesize: The size of a page in bytes, i.e. 4096 b
 * uint32_t* ID -> The page ID or address
 * Returns: The stored page's (24-bit) swap space ID/Address 
 */
int store_page(void*, os_size_t, uint32_t*);


/* retrieve_page will retrieve the page identified the ID pointer and
 * will store it back into main memory
 * Returns: The retrieved page's (32-bit) new main memory ID/Address (like example)
 *
 * Note: Page size was set by store_page 
 */
int retrieve_page(void*, uint32_t*);


// Returns: The total stored memory in bytes by function store_page(void*, uint32_t*)
os_size_t sum_stored();

/*
 *
 */
int vm_swap_page(void*, uint32_t*);


/*
 *
 */
int vm_unswap_page(void*, uint32_t);


/*
 *
 */
int vm_register_swapper(int (*)(void*, uint32_t*), int (*)(void*, uint32_t*), int);


#endif
