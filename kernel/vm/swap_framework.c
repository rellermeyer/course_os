#include "vm.h"
#include "memory.h"
#include "klibc.h"
#include <stdint.h>
#include "swap_framework.h"

void swap_init(){
	// initialize the first swap space head
	head = (struct swap_space_list*) kmalloc(sizeof(struct swap_space_list));
	head->next = 0;
	uint32_t entry_size = sizeof(struct swap_entry) + PAGE_SIZE;
	// initialize & allocate swap entry space for swapper
	head->head = (struct swap_entry*) kmalloc_aligned(entry_size * PAGE_ENTRIES, entry_size);
	head->lower_bits = 0;
	head->flags = 2;
	head->priority = 0;	
}


int store_page(void *page, uint32_t *ID)
{
	
	memory_count+=4096;
}

int retrieve_page(void *page, uint32_t *ID);


os_size_t sum_stored()
{
	return memory_count;
}

int vm_swap_page(void *page, uint32_t *ID);

int vm_register_swap_space(int (*store_page)(void *page, uint32_t *ID), int (*retrieve_page)
		(void *page, uint32_t *ID), int priority);

int vm_deregister_swap_space(int (*store_page)(void *page, uint32_t *ID), int (*retrieve_page)
		(void *page, uint32_t *ID), int priority);

uint32_t vm_page_fault(void *page);

uint32_t vm_scan_pages(void *page, uint32_t *ID);

