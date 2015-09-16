#include "vm.h"
#include "memory.h"
#include "klibc.h"
#include <stdint.h>
#include "swap_framework.h"
#include "swap_pqueue.h"
#include "swap_fs.h"
#include "fastlz/fastlz.h"

// NOTE: SWAPPING CANNOT WORK UNTIL FILESYSTEMS CAN ALLOCATE MORE THAN 16 PAGES AT A TIME
void swap_init()
{
	// initialize a swap space struct to hold values to then pass
	// TODO: change to guarantee allocation in RAM
	holder = (struct swap_space*) kmalloc(sizeof(struct swap_space));
}

// INCOMPLETE
uint32_t store_page_LZ(void *page, uint32_t *ID)
{
	struct node *LZ_swap = (struct node*) pqueue_peek(2);

	// allocate this on RAM this is where you os_memcpy the given page!!  
	// NOTE: THAT 1.05 DOESN'T MULTIPLY TO A CLEAN NUMBER
	void *e_page = kmalloc(PAGE_SIZE * 1.05); // output buffer needs to be at least 5% bigger than input

	int32_t cmp_size;
	cmp_size = fastlz_compress(page, PAGE_SIZE, e_page);

	/*if compressed size is greater than uncompressed size, input may not be 
	 compressible; return error */
	if (cmp_size > PAGE_SIZE)
	{
		return 0;
	}

	struct swap_entry *curr_ent = LZ_swap->e_head;
	int i = 0;
	while (curr_ent->cmp_page != NULL)
	{
		curr_ent += sizeof(struct swap_entry);
		i++;
	}
	*ID = i;
	curr_ent->cmp_size = cmp_size;
	curr_ent->e_flags = 1; //NOT CORRECT, put wherever it comes from here
	struct vas *kvas = KERNEL_VAS;
	int curr_add = 0x200000;

	// loop until free physical memory frame is found
	while (vm_allocate_page(kvas, curr_add, 4) == 0)
	{
		curr_add += PAGE_SIZE;
	}

	// after deciding physical memory location, store physical memory location in cmp_page	
	curr_ent->cmp_page = curr_add;
	kfree(e_page); // after copying over relevant bits of cmp data in this oversized buffer
	if (curr_ent->cmp_page != NULL)
	{
		curr_ent += sizeof(struct swap_entry);
		curr_ent = (struct swap_entry*) kmalloc(sizeof(struct swap_entry));
	}
	memory_count += 4096;
	return *ID;
}

uint32_t store_page(void *page, uint32_t *ID)
{
	uint8_t lowbits = *((uint32_t*) page) & 0x000000FF;
	if (swapfs_store(page, ID, lowbits) == -1)
	{
		return 0;
	}
	memory_count += 4096;
	return *ID;
}

// INCOMPLETE
uint32_t retrieve_page_LZ(void *page, uint32_t *ID)
{
	struct node *lz_swap = (struct node*) pqueue_peek(2);
	struct swap_entry *curr_ent = lz_swap->e_head;
	for (int i = 0; i < *ID; i++)
	{
		curr_ent += sizeof(struct swap_entry);
	}
	void *uncomp_page = kmalloc(PAGE_SIZE);
	int uncomp_size = fastlz_decompress(curr_ent->cmp_page, /*constant?*/
			COMPRESSED_SIZE / PAGE_ENTRIES, uncomp_page, PAGE_SIZE);

	/* if uncompressed size is not the size of a page or is 0 
	 (indicating corrupted data or a too small output buffer (the latter of 
	 which should never happen)), returns error */
	if (uncomp_size != PAGE_SIZE || uncomp_size == 0)
	{
		return 0;
	}
	// place the decompressed page back into memory. 
	os_memcpy(page, uncomp_page, PAGE_SIZE);
	kfree(uncomp_page);
	struct vas* kvas = KERNEL_VAS;
	vm_free_page(kvas, curr_ent->cmp_page);
	curr_ent->e_flags = 0;
	curr_ent->cmp_page = NULL;
	memory_count -= 4096;
	return *ID;
}

uint32_t retrieve_page(void *page, uint32_t *ID)
{
	uint8_t lowbits = *((uint32_t*) page) & 0x000000FF;
//	if (swapfs_retrieve(page, ID, lowbits) == -1) { // GIVES ERROR FOR SOME REASON??
//		return 0;
//	}
	memory_count -= 4096;
	return *ID;
}

os_size_t sum_stored()
{
	return memory_count;
}

uint32_t vm_swapin_page(void *page, uint32_t *ID)
{
	struct node *swap_area;
	uint8_t ssid = *((uint32_t*) page) & 0x000000FF;
//	uint32_t sse = *((uint32_t*)page) & 0xFFFFFF00; 

	// check if swap space exists
	if ((swap_area = pqueue_find(ssid)) == NULL)
	{
		/// check if there's enough memory for LZ compression
		// NOTE: always false until store/retrieve_page_LZ works
		if (0/*vm_count_free_frames() > COMPRESSED_SIZE/PAGE_ENTRIES*/)
		{
			vm_register_swap_space(store_page_LZ, retrieve_page_LZ, 0, -1);
			swap_area = pqueue_find(-1);
		}
		else
		{
			vm_register_swap_space(store_page, retrieve_page, 1, ssid);
			swap_area = pqueue_find(ssid);
		}
	}

	return swap_area->store_func(page, ID);
}

uint32_t vm_swapout_page(void *page, uint32_t *ID)
{
	uint8_t ssid = *((uint32_t*) page) & 0x000000FF;
	struct node *swap_area = pqueue_find(ssid);
	return swap_area->retrieve_func(page, ID);
}

int vm_register_swap_space(func store_p, func retrieve_p, int priority,
		int16_t ssid)
{
	/* waiting on fs team to increase 16 page limit, parameters 
	 SHOULD be (PAGE_ENTRIES, ssid) or (sse, ssid) */

	if (priority == 1)
	{
		swapfs_init(PAGE_ENTRIES, ssid);
		holder->e_head = NULL;
	}
	if (priority == 0)
	{
		if (holder->e_head = (struct swap_entry*) kmalloc(
				sizeof(struct swap_entry)) == NULL)
		{
			return -1;
		}
//		holder->e_head->next = NULL;
		holder->e_head->e_flags = 0;
		holder->e_head->cmp_page = NULL;
	}
	holder->lower_bits = ssid;
	holder->priority = priority;
	holder->store_func = store_p;
	holder->retrieve_func = retrieve_p;
	pqueue_push(holder);
	return 1;
}

void vm_deregister_swap_space(uint8_t ssid)
{
	pqueue_pop_at(ssid);
}

uint32_t vm_page_fault(void *page)
{
	/* TODO:
	 * check if memory is full and needs to evict a page from RAM
	 * figure out which page to evict than evict that page
	 * page in the new page (done.)
	 */

	//TODO: eventually change from equaling page to an actual id by establishing a proper hash table
	uint32_t* id = (uint32_t*) page;

	int temp;
	if ((temp = vm_swap_free_mapping(KERNEL_VAS, page, id)) < 0)
	{
		if (temp == VM_ERR_BADV)
		{
			os_printf("Virtual pointer passed was invalid [page_fault]");
		}
		if (temp == VM_ERR_NOT_MAPPED)
		{
			os_printf(
					"Virtual pointer was not mapped using set_mapping [page_fault]");
		}
		return 0;
	}

	uint8_t ssid = *((uint32_t*) page) & 0x000000FF;
	uint32_t ss_entry = *((uint32_t*) page) & 0xFFFFFF00;
	struct node *swap_area = pqueue_find(ssid);
	// load memory back into RAM
	swap_area->retrieve_func(page, &ss_entry);

	// find a free frame to map
	void *pptr = vm_get_free_frame();
	// TODO: once hashtable is properly set change permission to proper value
	int perm = VM_PERM_USER_RW;
	if (vm_set_mapping(KERNEL_VAS, page, pptr, perm) < 0)
	{
		if (temp == VM_ERR_BADV)
		{
			os_printf("Virtual pointer passed was invalid [page_fault]");
		}
		if (temp == VM_ERR_BADP)
		{
			os_printf("Physical pointer passed was invalid [page_fault]");
		}
		if (temp == VM_ERR_MAPPED)
		{
			os_printf(
					"The virtual pointer passed has already been mapped [page_fault]");
		}
		return 0;
	}
	// Get level 2 page table
	uint32_t *l2pt = (uint32_t*) VM_ENTRY_GET_L2(
			(uint32_t) VM_L1_GET_ENTRY(vm_get_current_vas()->l1_pagetable,
					page));

	// confirm from @lkolby whether this will work for setting the page table
	uint32_t *tmp = (uint32_t*) VM_L2_ENTRY(l2pt, page);
	*tmp = (uint32_t*) page;

	return *((uint32_t*) pptr);
}

//uint32_t* vm_scan_pages(void *page, uint32_t *ID);

