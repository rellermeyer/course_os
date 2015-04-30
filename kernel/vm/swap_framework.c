#include "vm.h"
#include "memory.h"
#include "klibc.h"
#include <stdint.h>
#include "swap_framework.h"
#include "swap_pqueue.h"

void swap_init()
{
	// initialize the first swap space head
	head = (struct swap_space*) kmalloc(sizeof(struct swap_space))
}

int store_page(void *page, uint32_t *ID)
{
	
	memory_count+=4096;
	return *ID;
}

int retrieve_page(void *page, uint32_t *ID)
{
	
	return *ID;
}


os_size_t sum_stored()
{
	return memory_count;
						}

int vm_swap_page(void *page, uint32_t *ID);

int vm_register_swap_space(int (*store_page)(void *page, uint32_t *ID), int (*retrieve_page)
			   (void *page, uint32_t *ID), int priority)
{
	if (pqeueue_size() != 0){
		head = (struct swap_space*) kmalloc(sizeof(struct swap_space));
	}		 
	head->pages_used = 0;
	head->lower_bits = pqueue_size();
	head->flags = 2;
	head->priority = priority;
	head->store_func = store_page;
	head->retrieve_func = retrieve_page;
	uint32_t entry_size = sizeof(struct swap_entry) + PAGE_SIZE;
	// TODO: Will vary allocation strategy based on given swap algorithms
	head->e_head = (struct swap_entry*) kmalloc_aligned(entry_size * PAGE_ENTRIES, entry_size);
        pqueue_add(head);	
}       

int vm_deregister_swap_space(int (*store_page)(void *page, uint32_t *ID), int (*retrieve_page)
		(void *page, uint32_t *ID), int priority);

uint32_t vm_page_fault(void *page);

uint32_t vm_scan_pages(void *page, uint32_t *ID);

