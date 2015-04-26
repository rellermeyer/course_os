#ifndef __SWAP_H
#define __SWAP_H 2

// Implements VM swapping to disk, utilizing a framework and multiple types of swap-in and swap-out. The framework allows 
// us to decide which swap to use when the decision matters. In the case of a page fault (data abort or instruction
// abort), the kernel will go through the framework to swap.

#include "vm.h"

// Swapping framework; is passed all relevant swapping functions and makes decisions as to which swap to use based currently only on priority.
int vm_register_swapper(int (*store_page)(void *page, uint32_t *id), int (*retrieve_page)(void *page, uint32_t *id), int priority);

// Stores a page into underlying storage. The variable id stores the id of the page for future use. Page size is currently set to 4 KB.
int store_page(void *page, uint32_t *id);

// Retrieves the page indentified by id, and stores it in memory identified by page. Page size is currently set to 4 KB.
int retrieve_page(void *page, uint32_t *id);



// For the near future: implement LZ compression swapping, which compresses data in memory and places it in a specific location 
// in hard memory. In most (all) cases, the overhead of compressing and decompressing is far less performance-impacting than the
// slow read and write speeds of secondary storage.

// 
int store_page_LZ(void *page, uint32_t *id);

//
int retrieve_page_LZ(void *page, uint32_t *id);




#endif
