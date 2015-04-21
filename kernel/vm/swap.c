// Implements VM swapping to disk, utilizing a framework and multiple types of swap-in and swap-out. The framework allows 
// us to decide which swap to use when the decision matters. In the case of a page fault (data abort or instruction
// abort), the kernel will go through the framework to swap.

#define NUM_SWAPPERS 2

// Warning: pretty ugly
// Swapping framework; is passed all relevant swapping functions and makes decisions as to which swap to use based currently only on priority. Lowest priority value (highest priority) is 0. amt_used stores the amount of space that a certain swapper currently has swapped, sorted highest priority to lowest (same order as in swappers array).
// Priority 0: regular secondary storage swap
// Priority 1: LZ compression swap
int vm_register_swapper(int (*store_page)(void *page, uint32_t *id), int (*retrieve_page)(void *page, uint32_t *id), int priority){
  int i = priority;
  int amt_used[NUM_SWAPPERS] = {0, 0}; 
  int *swappers[2*NUM_SWAPPERS] = {store_page_LZ, retrieve_pagePLZ, store_page, retrieve_page}; // swappers available, highest priority to lowest
  // loop until a swapper is found that has not used all its 64 GB (2^24 page bits * 2^12 bits per page) of memory, beginning at the highest max priority (as set by arg) and proceeding to lower priorities
  // TOOD: may want to check higher priorities for space? Or keep specified priority as max priority?
  while (amt_used[i] == 64000000){
    i++;
  } 
  int *store_page = swapper[2*i]; // gives pointer to proper store function
  int *retrieve_page = swappers[(2*i) + 1]; // gives pointer to proper retrieve function
  amt_used[i] += 4;
}

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

