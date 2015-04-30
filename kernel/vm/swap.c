// Implements VM swapping to disk or other medium, utilizing a framework and multiple types of swap-in and swap-out. The framework allows 
// us to decide which swap to use when the decision matters. In the case of a page fault (data abort or instruction
// abort), the kernel will go through this framework to swap based on priority information 



#define NUM_SWAPPERS 2
int* swappers[2*NUM_SWAPPERS]; 
int amt_used[NUM_SWAPPERS];


// Swapper initialization function; is called for each pair of swap functions (store and retrieve) to store them in the list of swap functions. Sorted by priority from highest priority (lowest value, currently 0) to lowest (currently 1). 
// Warning: very hardcoded to knowing the specific number of swap functions and respective priorities that are going to be loaded. Swapper list and swapper load list are only as big as the defined number of functions. Could make list bigger but this allows potential holes in the list if priority values aren't specific and strict.
int vm_register_swapper(int (*store_page)(void *page, uint32_t *id), int (*retrieve_page)(void *page, uint32_t *id), int priority){
  swappers[(2*priority)] = store_page;
  swappers[(2*priority) + 1] = retrieve_page;
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

