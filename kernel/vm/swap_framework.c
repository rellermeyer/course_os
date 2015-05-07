#include "vm.h"
#include "memory.h"
#include "klibc.h"
#include <stdint.h>
#include "swap_framework.h"
#include "swap_pqueue.h"
#include "swap_fs.h"

void swap_init()
{
	// initialize a swap space struct to hold values to then pass
	holder = (struct swap_space*) kmalloc(sizeof(struct swap_space));	
}

uint32_t *store_page(void *page, uint32_t *ID)
{
	uint8_t lowbits = *((uint32_t*)page) & 0x000000FF;
	if (swapfs_store(page, ID, lowbits) == -1){
		return NULL;
	}
	memory_count+=4096;
	return ID;
}

uint32_t *store_page_LZ(void *page, uint32_t *ID)
{
		
	memory_count+=4096;
	return ID;
}

uint32_t *retrieve_page(void *page, uint32_t *ID)
{
	uint8_t lowbits = *((uint32_t*)page) & 0x000000FF;
	if (swapfs_retrieve(page, ID, lowbits) == -1){
		return NULL;
	}
	memory_count-=4096;	
	return ID;
}

uint32_t *retrieve_page_LZ(void *page, uint32_t *ID)
{

	memory_count-=4096;
	return ID;
}

os_size_t sum_stored()
{
	return memory_count;
}

uint32_t *vm_swapout_page(void *page, uint32_t *ID)
{
	struct node *swap_area;
	uint8_t ssid = *((uint32_t*)page) & 0x000000FF;
	
	// check if swap space exists
	if ((swap_area = pqueue_find(ssid)) == NULL){ 
		///check if there's enough memory for LZ compression
		if(0){ // always false until store/retrieve_page_LZ works
			vm_register_swap_space(store_page_LZ, retrieve_page_LZ, 0, ssid);
			swap_area = pqueue_find(ssid);		
		} else {
			vm_register_swap_space(store_page, retrieve_page, 1, ssid);
			swap_area = pqueue_find(ssid);
		}
	}
	
	return swap_area->store_func(page, ID);
}

uint32_t *vm_swapin_page(void *page, uint32_t *ID)
{
	uint8_t ssid = *((uint32_t*)page) & 0x000000FF;
	struct node *swap_area = pqueue_find(ssid);
	return swap_area->retrieve_func(page, ID);
}

int8_t vm_register_swap_space(func store_p, func retrieve_p, int priority, uint8_t ssid)
{	
	/* waiting on fs team to increase 16 page limit, parameters 
	   SHOULD be (PAGE_ENTRIES, ssid) or (sse, ssid) but may not work */ 

	if (priority == 1) {
		swapfs_init(PAGE_ENTRIES, ssid);
	}
	
	holder->lower_bits = ssid;
	holder->priority = priority;
	holder->store_func = store_p;
	holder->retrieve_func = retrieve_p;
	pqueue_push(holder);
}       

void vm_deregister_swap_space(uint8_t ssid)
{
	 pqueue_pop_at(ssid);
}

uint32_t vm_page_fault(void *page){
	/* TODO:
	 * check if memory is full and needs to evict a page from RAM
	 * figure out which page to evict than evict that page
	 * page in the new page (done.)
	 */	
	uint8_t ssid = *((uint32_t*)page) & 0x000000FF;
	uint32_t ss_entry = *((uint32_t*)page) & 0xFFFFFF00;
	struct node *swap_area = pqueue_find(ssid);
	// load memory back into RAM
	swap_area->retrieve_func(page, &ss_entry);
	struct vas *vm_current_vas = (struct vas*)V_L1PTBASE;	

//	lastly, update the page table
//	uint32_t *pptr = VM_ENTRY_GET_L2(VM_L1_GET_ENTRY(vm_get_current_vas()->l1_pagetable, page));
	

}



