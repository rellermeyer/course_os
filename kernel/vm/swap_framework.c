#include "vm.h"
#include "memory.h"
#include "klibc.h"
#include <stdint.h>
#include "swap_framework.h"
#include "swap_pqueue.h"
#include "swap_fs.h"
#include "fastlz/fastlz.h"

void swap_init()
{
	// initialize a swap space struct to hold values to then pass
	holder = (struct swap_space*) kmalloc(sizeof(struct swap_space));	
}

uint32_t store_page_LZ(void *page, uint32_t ID){
	struct node *LZ_swap = pqueue_find(0);
	void *cmp_page = malloc(PAGE_SIZE*1.05); // output buffer needs to be at least 5% bigger than input;
	int cmp_size;
	cmp_size = fastlz_compress(page, PAGE_SIZE, cmp_page);

	// if compressed size is greater than uncompressed size, input may not be compressible; return error
	if (cmp_size > PAGE_SIZE){
		return NULL;
	}
	struct swap_entry *curr_ent = LZ_swap->e_head;
	int i = 0;
	while(curr_ent->free = 0){
		curr_ent = curr_ent->next;
		i++;
	}
	ID = i;
	curr_ent->higher_bits = ID;
	curr_ent->free = 0;
	curr_ent->cmp_size = cmp_size;
	// curr_ent->e_flags = wherever it comes from
	// TODO: need to copy the compressed bits into memory. Use kernel's VAS to access physical memory and put it pretty much wherever

	// memcpy(*dest whatever it may be, *cmp_page, cmp_size)
	// curr_ent->cmp_page = *dest  after deciding dest, store location of compressed page in entry; may need to remember old page ID for some reason?
	free(cmp_page); // after copying over relevant bits of cmp data in this oversized buffer
	memory_count += 4096;
	return ID;
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

uint32_t retrieve_page_LZ(void *page, uint32_t ID){
	struct node *lz_swap = pqueue_find(0);
	struct swap_entry *curr_ent = lz_swap->e_head;
	while(curr_ent->higher_bits != ID){
		curr_ent = curr_ent->next;
	}
	void* uncomp_page = malloc(PAGE_SIZE);
	int uncomp_size = fastlz_decompress(curr_ent->cmp_page, curr_ent->cmp_size, uncomp_page, PAGE_SIZE);

	// if uncompressed size is not the size of a page or is 0 (indicating corrupted data or a too small output buffer (the latter of 
	// which should never happen)), returns error
	if(uncomp_size != PAGE_SIZE || uncomp_size == 0){
		return NULL;	
	}
	// place the decompressed page back into memory. 
	memcpy(page, uncomp_page, PAGE_SIZE);
	free(uncomp_page);
	curr_ent->e_flags = 0; 
	curr_ent->free = 1;
	// curr_ent->cmp_size = 0; likely not necessary 
	// curr_ent->cmp_page = 0; likely not necessary
	memory_count -= 4096;
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

os_size_t sum_stored()
{
	return memory_count;
}

uint32_t *vm_swapin_page(void *page, uint32_t *ID)
{
	struct node *swap_area;
	uint8_t ssid = *((uint32_t*)page) & 0x000000FF;
//	uint32_t sse = *((uint32_t*)page) & 0xFFFFFF00; 
	
	// check if swap space exists
	if ((swap_area = pqueue_find(ssid)) == NULL){ 
		///check if there's enough memory for LZ compression
		if(0){ // always false until store/retrieve_page_LZ works
			vm_register_swap_space(store_page_LZ, retrieve_page_LZ, 0, ssid);
			swap_area = pqueue_find(ssid);
			void* lz_e_head = kmalloc_aligned(sizeof(struct swap_entry)*PAGE_ENTRIES, sizeof(struct swap_entry));
			int* curr_add = lz_e_head;
			for (int i = 0; i < PAGE_ENTRIES; i++){
				struct swap_entry *temp = (struct swap_entry*)curr_add;
				curr_add += sizeof(struct swap_entry);
			}
			swap_area->e_head = lz_e_head;
				
		} else {
			vm_register_swap_space(store_page, retrieve_page, 1, ssid);
			swap_area = pqueue_find(ssid);
		}
	}
	
	return swap_area->store_func(page, ID);
}

uint32_t *vm_swapout_page(void *page, uint32_t *ID)
{
	
}

int8_t vm_register_swap_space(func store_p, func retrieve_p, int priority, uint8_t ssid)
{	
	/* waiting on fs team to increase 16 page limit, parameters 
	   SHOULD be (PAGE_ENTRIES, ssid) or (sse, ssid) */ 

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

uint32_t vm_page_fault(void *page);

//uint32_t* vm_scan_pages(void *page, uint32_t *ID);

