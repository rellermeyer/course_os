#ifndef __VM_SWAP_FRAMEWORK_H
#define __VM_SWAP_FRAMEWORK_H

#include "klibc.h"
#include <stdint.h>


/* Contributors: Noel Negusse and Jesse Thaden
 * Last Update: 05/06/15
 */

/* Function: swap_framework
 * Purpose: To present an API for swapping pages to various swap spaces
 * so that pages may be stored independently of other related operations
 *
 * EXAMPLE OF VIRTUAL MEMORY ID/ADDRESS FROM SWAP SPACE
 *	_____________________________________________________________
 *	|| [24 bit] SWAP SPACE ENTRY ID | [8 bit] SWAP SPACE    #  ||
 *	¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 *	*	Total: 32 Bit Address
 *	therefore, there are 256 possible swap spaces possible
 *	w/ 2^12 page entries each (Assuming 4kB pages)
 */

#define SWAP_SPACES (1<<8)
#define PAGE_ENTRIES (1<<12) // Assuming 4kB Pages right now
#define PAGE_SIZE (1<<12) // May carry an if statement later to account for different page sizes

typedef uint32_t *(*func)(void*, uint32_t*);

struct swap_space {
//	struct swap_entry *e_head;
//      uint16_t pages_free;
	uint8_t lower_bits; // swap space ID [8-bits]
//	uint16_t flags; // swp_used (1000 or 1), swp_writeok (0010 or 2) or both (0011 or 3)
	uint8_t priority; // lower is better
	func store_func;
	func retrieve_func;
}; // Total: 10 bytes

//MAY OR MAY NOT NEED
//struct swap_entry {
	//struct swap_entry *next;
	//uint32_t higher_bits; // swap entry ID [24-bit assuming 4kB pages]
	//uint16_t e_flags; // ENT_USED (1000 or 1), ENT_WRITEOK (0100 or 2) OR BOTH (1100 or 3)
        //uint8_t free; //0 - used, 1 - free
        //void *page; // virtual address pointer used for resolving page faults
//}; // Total: 15 bytes

static struct swap_space *holder;
static os_size_t memory_count;


// Swap_init initializes swap framework
void swap_init();
//void swap_init(os_size_t);  To be implemented... [specifies global page size]


/* store_page will store a page to media - not main memory - storage, e.g. HDD
 * from the page*. ID parameter passed will change to appropriate index (i.e. return value)
 * void* page -> data to be paged
 * os_size_t pagesize: The size of a page in bytes, i.e. 4096 b
 * uint32_t* ID -> The page ID or address
 *
 * Returns: A pointer to a index value (from a bit vector) in memory 
 * OR returns a NULL/0 on failure 
 */
uint32_t *store_page(void*, uint32_t*);
uint32_t *store_page_LZ(void*, uint32_t*);
// uint32_t *store_page(void*, os_size_t, uint32_t*); To be implemented... (will replace)
// uint32_t *store_pageLZ(void*, os_size_t, uint32_t*); diddo


/* retrieve_page will retrieve the page identified the ID pointer and
 * will store it back into main memory (specified by the void *page pointer)
 *
 * Returns: NULL on failure or simply passes back ID on success
 * NOTE: Page size was set by store_page
 */
uint32_t *retrieve_page(void*, uint32_t*);
uint32_t *retrieve_page_LZ(void*, uint32_t*);


/* Returns: The total stored memory in bytes by function store_page(void*, uint32_t*) */
os_size_t sum_stored();


/* vm_swap_page will store a page from main memory (specified by void *page) 
 * to a swap space either on disk or a compressed block in memory
 *
 * Returns: The ID pointer - bit vector index - of where the swap_space was stored and 
 * changes the ID pointer that value as well. Returns NULL on failure
 */
uint32_t *vm_swapin_page(void*, uint32_t*);
uint32_t *vm_swapout_page(void*, uint32_t*);


/* vm_register/vm_deregister will activate/deactivate a swap space and set a priority
 * accordingly to it. This allows the swap spaces use as well as increasing it's efficiency
 * NOTE: deregister_swap_space will always deregister the lowest priority swap space first 
 *
 * Returns: -1 or 1 whether the swap space registeration was a failure or success, respectively
 */
int8_t vm_register_swap_space(func, func, int, uint8_t);
void vm_deregister_swap_space(uint8_t);


/* Resolves the page fault and updates the corresponding page table
 *
 * Returns: The new resolved physical address of the page fault
 * NOTE: May return a 0 if the page_fault could not be resolved
 */
uint32_t vm_page_fault(void*);


/* vm_scan_pages will scan through all pages for the corresponding page
 * to the given page
 *
 * Returns: The address of matching swapped page 
 */
//uint32_t* vm_scan_pages(void*);

#endif
