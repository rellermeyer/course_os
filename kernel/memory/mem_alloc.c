#include "mem_alloc.h"
#include "klibc.h"
#include "vm.h"

uint32_t *nextBlock = (uint32_t*) MEM_START;

//bump pointer allocation
void *mem_alloc(uint32_t size)
{
	uint32_t temp = size / 4;

	if ((size % 4) > 0)
	{
		temp++;
	}
	uint32_t* allocBlock = nextBlock;
	nextBlock = nextBlock + size;

	return allocBlock;
}

/*
 * The kernel heap is organized in blocks. Each block has a header and a
 * footer each a 4 byte integer, at the beginning and end of the block.
 * The header and footer both specify the size of the block in question.
 * Used blocks are indicated by the heap header and the heap footer being
 * negative.
 *
 * To allocate a block, we start at the first block and walk through each
 * one, finding the first free block large enough to support a header,
 * footer, and the size requested.
 *
 * To free a block, we do a bunch of merging stuff to check to see if we
 * should merge with the blocks on the left or right of us, respectively.
 */

void *init_heap()
{
	//heap = (uint32_t*) mem_alloc(size);
	int retval = vm_allocate_page(KERNEL_VAS, (void*)MEM_START, VM_PERM_PRIVILEGED_RW);
	if (retval) {
		os_printf("ERROR: vm_allocate_page returned %d\n", retval);
	}
	heap = (uint32_t*)MEM_START;
	heap_size = BLOCK_SIZE; // We have exactly 1 block

	//os_printf("&heap=%x\n", heap);
	//heap_size = size;

	uint32_t* heap_header = heap;
	uint32_t* heap_footer = (uint32_t*)((void*)heap + heap_size - sizeof(int));

	*heap_header = heap_size - 2*sizeof(uint32_t);
	*heap_footer = heap_size - 2*sizeof(uint32_t);

	os_printf("heap size = 0x%x\n", heap_size);
	os_printf("heap_header=%x (%x == %x)\n", heap_header, *heap_header, heap_size - 2*sizeof(uint32_t));
	os_printf("heap_footer=%x\n", heap_footer);
	return heap;
}

// TODO: what if there's an error allocating the page?
// Returns a pointer to the new (big) free block's header
uint32_t *extend_heap(uint32_t amt)
{
	uint32_t start_size = heap_size;
	uint32_t amt_added = 0;
	while (heap_size-start_size < amt) {
		int retval = vm_allocate_page(KERNEL_VAS, (void*)(MEM_START+heap_size), VM_PERM_PRIVILEGED_RW);
		if (retval) {
			os_printf("ERROR: vm_allocate_page(,%d,) returned %d\n", MEM_START+heap_size, retval);
		}
		heap_size += BLOCK_SIZE;
		amt_added += BLOCK_SIZE;
	}

	// Now extend the footer block
	uint32_t *orig_footer = (uint32_t*)((void*)MEM_START+start_size-sizeof(uint32_t));
	os_printf("%x %x\n", orig_footer, *orig_footer);

	// If it's free, simply move it (and update the header)
	// If it's used, add a free block to the end
	if (*orig_footer > 0) {
		os_printf("Last block was free.\n");
		uint32_t *orig_header = (uint32_t*)(MEM_START+start_size-2*sizeof(uint32_t)-*orig_footer);
		uint32_t *new_footer = (uint32_t*)(MEM_START+heap_size-sizeof(uint32_t));
		os_printf("%x\n", orig_header);
		*new_footer = *orig_footer + amt_added;
		*orig_header += amt_added;
		return orig_header;
	} else {
		os_printf("Last block was used.\n");
		uint32_t *new_header = (uint32_t*)(MEM_START+start_size);
		uint32_t *new_footer = (uint32_t*)(MEM_START+heap_size-sizeof(uint32_t));
		*new_header = amt_added - 2*sizeof(uint32_t);
		*new_footer = amt_added - 2*sizeof(uint32_t);
		return new_header;
	}
	return 0x0;
}

void* allocate(uint32_t size, uint32_t* heap, int32_t heap_size)
{
	// Round size up to the nearest 4 bytes...
	/*if (size&3) {
		size = (size&~3) + 4;
	}*/

	if (size > (heap_size + 2 * sizeof(int32_t)))
		return 0;
	int32_t i, ret_ptr;
	for (i = 0; i < heap_size;)
	{

		//os_printf("byte=%d\n",i);

		uint32_t* header_addr = (uint32_t*)((void*)heap + i);
		int32_t header = *header_addr;

		//os_printf("&header=%x header=%d\n",header_addr, header);

		uint32_t* footer_addr = (uint32_t*)((void*)heap + i + sizeof(int32_t) + size);

		//free and >= request
		if (header > 0 && header >= size)
		{

			//cannot split this block
			if ((header - size) < (2 * sizeof(int32_t) + sizeof(char)))
			{

				ret_ptr = i + sizeof(int32_t);
				//mark header as used
				*header_addr = header * (-1);
				//insert a footer at end of block
				*footer_addr = header * (-1);
				return (uint32_t*)((void*)heap + ret_ptr);
				//return heap + ret_ptr;

			}

			//can split this block
			else
			{
				ret_ptr = i + sizeof(int32_t);

				int32_t old_space = header;
				int32_t occ_space = size + 2 * sizeof(int32_t);
				//mark header as used
				*header_addr = size * (-1);
				//insert footer
				*footer_addr = size * (-1);

				//insert new free block header
				uint32_t* new_header = (uint32_t*)((void*)heap + i + 2 * sizeof(int32_t) + size);
				*new_header = old_space - occ_space;
				//insert new free block footer
				uint32_t* new_footer = (uint32_t*)((void*)heap + i + sizeof(int32_t) + old_space);
				*new_footer = old_space - occ_space;

				/*os_printf("header & footer: %x and %x\n", new_header, new_footer);
				os_printf("header & footer: %x and %x\n", header_addr, footer_addr);
				os_printf("%x and %x and %d and %d\n", *header_addr, *footer_addr, *new_header, *new_footer);*/

				return (uint32_t*)((void*)heap + ret_ptr);
			}
		}
		//jump to the next block
		else
		{
			i = i + abs(header) + 2 * sizeof(int32_t);
			//os_printf("i: 0x%x\n",i);
		}
	}

	// Allocate some more memory.
	uint32_t new_amt = size+2*sizeof(uint32_t);
	uint32_t *header = extend_heap(new_amt);
	os_printf("Allocated more memory %u.\n", new_amt);

	// Recursive call. TODO: (relatively) Inefficient
	return allocate(size, heap, heap_size);

	uint32_t *new_footer = (uint32_t*)((void*)header + size + sizeof(uint32_t));
	uint32_t *new_header = new_footer + 1;
	uint32_t *old_footer = (void*)header + sizeof(uint32_t) + *header;

	os_printf("old_footer = 0x%x\n",old_footer);
	os_printf("new_header = 0x%x\n", new_header);

	*new_header = *header - size - 2*sizeof(uint32_t);
	*new_footer = *new_header;
	*header = -size;
	*old_footer = -size;

	// The memory immediately after new_header
	return (void*) (new_header+1);
}

void deallocate(void* ptr, uint32_t* heap, int32_t heap_size)
{
	os_printf("Deallocating %x\n", ptr);

	uint32_t first_block = 0;
	uint32_t last_block = 0;

	uint32_t* header_addr = (uint32_t*) ((void*)ptr - sizeof(int32_t));
	uint32_t size = abs(*header_addr);

	uint32_t* footer_addr = (uint32_t*) ((void*)ptr + size);

	if (header_addr == heap)
	{
		first_block = 1;
	}

	if (footer_addr + sizeof(int32_t) == (uint32_t*) heap + heap_size)
	{
		last_block = 1;
	}

	os_printf("first block: %d, last block: %d\n", first_block, last_block);

	//only check and coalesce right block
	if (first_block)
	{
		uint32_t* right_header_addr = (uint32_t*) ((void*)footer_addr + sizeof(int32_t));
		int32_t right_block_size = *right_header_addr;

		//free right block
		if (right_block_size > 0)
		{
			//set new header at freed blocks header
			*header_addr = size + right_block_size + 2 * sizeof(int32_t);
			//set new footer at right blocks footer
			uint32_t* right_footer_addr = (uint32_t*) ((void*)footer_addr
								   + 2 * sizeof(int32_t) + right_block_size);
			*right_footer_addr = size + right_block_size + 2 * sizeof(int32_t);
		}
		else
		{
			//make freed blocks header and footer positive
			*header_addr = size;
			*footer_addr = size;
		}
	}

	//only check and coalesce left block
	if (last_block)
	{
		uint32_t* left_block_header = (uint32_t*) ((void*)header_addr - sizeof(int32_t));
		int32_t left_block_size = *left_block_header;

		//free left block
		if (left_block_size > 0)
		{
			//set new header at left blocks header
			uint32_t* left_header_addr = (uint32_t*) ((void*)header_addr
								  - 2 * sizeof(int32_t) - left_block_size);
			*left_header_addr = size + left_block_size + 2 * sizeof(int32_t);
			//set new footer at freed blocks footer
			*footer_addr = size + left_block_size + 2 * sizeof(int32_t);
		}
		else
		{
			*header_addr = size;
			*footer_addr = size;
		}
	}

	//check and coalesce both adjacent blocks
	if (!first_block && !last_block)
	{
		uint32_t* right_block_header = (uint32_t*) ((void*)footer_addr + sizeof(int32_t));
		int32_t right_block_size = *right_block_header;

		uint32_t* left_block_header = (uint32_t*) ((void*)header_addr - sizeof(int32_t));
		int32_t left_block_size = *left_block_header;

		//both adjacent blocks are free
		if (right_block_size > 0 && left_block_size > 0)
		{
			int32_t new_size = size + right_block_size + left_block_size
					+ 4 * sizeof(int32_t);

			//set new header at left blocks header
			uint32_t* left_header_addr = (uint32_t*) ((void*)header_addr
								  - 2 * sizeof(int32_t) - left_block_size);
			*left_header_addr = new_size;
			//set new footer at right blocks footer
			uint32_t* right_footer_addr = (uint32_t*) ((void*)footer_addr
								   + 2 * sizeof(int32_t) + right_block_size);
			*right_footer_addr = new_size;
		}

		//only right free block
		else if (right_block_size > 0 && left_block_size < 0)
		{
			//set new header at freed blocks header
			*header_addr = size + right_block_size + 2 * sizeof(int32_t);
			//set new footer at right blocks footer
			uint32_t* right_footer_addr = (uint32_t*) ((void*)footer_addr
								   + 2 * sizeof(int32_t) + right_block_size);
			*right_footer_addr = size + right_block_size + 2 * sizeof(int32_t);
		}
		//only left free block
		else if (left_block_size > 0 && right_block_size < 0)
		{
			//set new header at left blocks header
			uint32_t* left_header_addr = (uint32_t*) ((void*)header_addr
								  - 2 * sizeof(int32_t) - left_block_size);
			*left_header_addr = size + left_block_size + 2 * sizeof(int32_t);
			//set new footer at freed blocks footer
			*footer_addr = size + left_block_size + 2 * sizeof(int32_t);
		}
		else
		{
			*header_addr = size;
			*footer_addr = size;
		}

	}
}

void* aligned_mem_alloc(uint32_t size, uint32_t alignment)
{
	if (alignment == 4)
	{
		void* block = kmalloc(size + 4);
		void* ptr = (void*) (((uint32_t) block + 4) & ~0x3);
		return ptr;
	}

	else if (alignment == 1024)
	{
		void* block = kmalloc(size + 1024);
		void* ptr = (void*) (((uint32_t) block + 1024) & ~0x1ff);
		return ptr;
	}

	else if (alignment == 4096)
	{
		void* block = kmalloc(size + 4096);
		void* ptr = (void*) (((uint32_t) block + 4096) & ~0x7ff);
		return ptr;
	}

	else if (alignment == 16 * 1024)
	{
		void* block = kmalloc(size + 16 * 1024);
		void* ptr = (void*) (((uint32_t) block + 16 * 1024) & ~0x1fff);
		return ptr;
	}

	else
		return kmalloc(size);
}

int mcheck()
{

	char* ptr = (char*) heap;
	uint32_t* end_ptr = (uint32_t*) ((void*)heap + heap_size);
	int i, block = 0;
	for (i = 0; i < heap_size; i += 0)
	{
		uint32_t* block_addr = (uint32_t*) (ptr + sizeof(int32_t));

		uint32_t* header_addr = (uint32_t*) ptr;
		int32_t block_header = *header_addr;
		int32_t block_size = abs(block_header);

		uint32_t* footer_addr = (uint32_t*) (ptr + sizeof(int32_t) + block_size);
		int32_t block_footer = *footer_addr;

		if (block_header == block_footer && block_header < 0)
		{
			os_printf("Block %d Allocated:", block);
			os_printf("\tsize = %d, address = %x\n", block_size, block_addr);
		}
		else if (block_header == block_footer && block_header > 0)
		{
			os_printf("Block %d Free:", block);
			os_printf("\tsize = %d, address = %x\n", block_size, block_addr);
		}
		else
		{
			os_printf("INCONSISTENT HEAP\n");
			os_printf("block_header = %d\n",block_header);
			os_printf("block_footer = %d\n",block_footer);
			os_printf("header addr = %x\n",header_addr);
			os_printf("footer addr = %x\n",footer_addr);
			return 1;
		}

		ptr = ptr + block_size + 2 * sizeof(int32_t);
		block++;
		os_printf("%x %x\n", ptr, end_ptr);
		if ((uint32_t*) ptr == end_ptr)
			return 0;
	}
	return 0;
}

//nbits is 0-22
uint32_t gen_rand(uint64_t *state, int nbits)
{
	*state = *state * 41 + (*state >> 5);
	uint32_t mask = ((1<<nbits)-1)<<10;
	uint32_t v = (*state & mask);
	return v>>10;
}

int vm_count_free_frames();

void test_allocate()
{
	os_printf("***** Test code for allocater (test_allocate()): *****\n");

	if (mcheck()) {
		os_printf("Inconsistent memory to begin with...\n");
	}

	// Allocate some memory
	char *p = allocate(15, heap, heap_size);
	p = allocate(15, heap, heap_size);
	os_strcpy(p, "Hello!");
	os_printf("%s\n", p);

	if (mcheck()) {
		os_printf("allocate(15) failed.\n\n");
		int i;
		for (i=0; i<8; i++) {
			int j;
			for (j=0; j<8; j++) {
				os_printf("%X ", ((char*)heap)[i*8+j]);
			}
			os_printf("\n");
		}
	}

	// Allocate a bunch of blocks of "random" size
	uint64_t rng = 1234567891;

	char *pntrs[256];
	uint32_t alloced = 0;
	int i;
	os_printf("Starting out w/ %u bytes of heap\n", heap_size);
	for (i=0; i<256; i++) {
		uint32_t size = gen_rand(&rng, 15);
		pntrs[i] = allocate(size, heap, heap_size);
		alloced += size;
		os_printf("%u %u %u %u %d\n", i, heap_size, size, alloced, vm_count_free_frames());

		/*if (mcheck()) {
			os_printf("Memory is inconsistent. Stopping test.\n");
			break;
			}*/
	}

	// Test one of them
	pntrs[230][0] = 1;
	os_printf("%d == 1?\n", pntrs[230][0]);

	if (mcheck()) {
		os_printf("Memory is inconsistent :-(\n");
	} else {
		os_printf("Memory is consistent :-)\n");
	}

	// Free all the pntrs
	for (i=0; i<256; i++) {
		if (pntrs[i]) {
			deallocate(pntrs[i], heap, heap_size);
		}
	}

	deallocate(p, heap, heap_size);

	os_printf("heap_size = %d bytes\n", heap_size);
	os_printf("******************************************************\n");
}
