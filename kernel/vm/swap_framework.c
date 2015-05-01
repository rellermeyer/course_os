#include "vm.h"
#include "memory.h"
#include "klibc.h"
#include <stdint.h>
#include "swap_framework.h"
#include "swap_pqueue.h"
#include "swap_fs.h"

void swap_init()
{
	// initialize the first swap space head
	head = (struct swap_space*) kmalloc(sizeof(struct swap_space));
	memory_count = 0;
	if(/*enough memory for LZ*/){
		vm_register_swap_space(store_page_LZ, retrieve_page_LZ, 0);
	}
	else{
		swapfs_init(16);
		vm_register_swap_space(store_page, retrieve_page, 1);
	}

}

uint32_t *store_page_LZ(void *page, uint32_t *ID);

uint32_t *store_page(void *page, uint32_t *ID)
{
	//MAY BE USELESS#####################
	int i = 0;
	if(pqueue_index(0)->flags == 1){
		for(i = 1; i<pqueue_size(); i++){
			if(pqueue_index(i)->flags > 1){
				break;
			}
		}
		if(i == pqueue_size()){
			if(/*enough memory for lZ*/){
				vm_register_swap_space(store_page_LZ, retrieve_page_LZ, 0);
			}
			else{
				vm_register_swap_space(store_page_LZ, retrieve_page_LZ, 1);
			}
		}
	}
	struct node *current_node = pqueue_index(i);
	struct swap_entry *temp = current_node->e_head;
	// looping for available swap space page entry
	for(int x = 0; x<PAGE_ENTRIES; x++){
		if(temp->free == 1){
			break;
		}
		temp = temp->next;
	}
	// generate new ID
	uint32_t identify = ((temp->higher_bits)<<8) + current_node->lower_bits;
	ID = identify;
	current_node->e_flags = 1;
	current_node->page = page;
	current_node->free = 0;
	// store/move memory
	//MAY BE USELESS#######################

	ID = swapfs_store(page, ID);
	// If Failure
	if(ID == -1){
		return 0;
	}
	memory_count+=4096;
	return *ID;
}


uint32_t *store_page_LZ(void *page, uint32_t *ID);

uint32_t *retrieve_page(void *page, uint32_t *ID)
{
	//MAY BE USELESS
	struct swap_entry *temp = pqueue_index(pqueue_find((ID<<24)>>24))->e_head; 
	for(int x = 0; x<(ID>>8); x++){
		temp = temp->next;
	}
	temp->free = 1;
	temp->e_flags = 2;
	//MAY BE USELESS
	
	if(uint32_t addr = swapfs_recieve(page, ID)==-1){
		return 0;	
	}
	memeory_count-=4096;	
	return addr;
}


os_size_t sum_stored()
{
	return memory_count;
}

uint32_t *vm_swap_page(void *page, uint32_t *ID){
	if(pqueue_index(0)->flags > 1){  
		pqueue_index(0)->store_func(page, ID);
	}
	else{
		for(int i = 1; i<pqueue_size(); i++){
			if(pqueue_index(i)->flags>1){
				pqueue_index(i)->store_func(page, ID);
				break;
			}
		}
	}
}

uint32_t *vm_unswap_page(void *page, uint32_t *ID){
	return pqueue_index(ID*)->retrieve_func(page, ID);
}

uint8_t vm_register_swap_space(int (*store_p)(void *page, uint32_t *ID), int (*retrieve_p)
			   (void *page, uint32_t *ID), int priority)
{
	if (pqeueue_size() != 1){
		head = (struct swap_space*) kmalloc(sizeof(struct swap_space));
	}		 
	front->pages_free = PAGE_SIZE;
	front->lower_bits = pqueue_size();
	front->flags = 2;
	front->priority = priority;
	front->store_func = &store_p;
	front->retrieve_func = &retrieve_p;
//	uint32_t entry_size = sizeof(struct swap_entry) + PAGE_SIZE;
	// TODO: Will vary allocation strategy based on given swap algorithms
	front->e_head = (struct swap_entry*) kmalloc_aligned(PAGE_ENTRIES*sizeof(swap_entry), sizeof(swap_entry));
	struct swap_entry *current_node = front->e_head;
	for(int x = 0; x<PAGE_ENTRIES; x++){
		current_node->higher_bits = x;
		current_node = current_node->next;
	}
        pqueue_add(head);	
}       

uint8_t vm_deregister_swap_space()
{
	return pqueue_pop_back();
}

uint32_t vm_page_fault(void *page);

uint32_t vm_scan_pages(void *page, uint32_t *ID);

