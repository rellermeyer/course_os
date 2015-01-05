#include "mem_alloc.h"
#include "klibc.h"

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

void *init_heap(uint32_t size)
{
	heap = (uint32_t*) mem_alloc(size);
	//os_printf("&heap=%x\n", heap);
	heap_size = size;

	uint32_t* heap_header = heap;
	uint32_t* heap_footer = heap + heap_size - sizeof(int);

	*heap_header = heap_size;
	*heap_footer = heap_size;

	//os_printf("heap_header=%x\n", heap_header);
	//os_printf("heap_footer=%x\n", heap_footer);
	return heap;
}

void* allocate(uint32_t size, uint32_t* heap, int32_t heap_size)
{
	if (size > (heap_size + 2 * sizeof(int32_t)))
		return 0;
	int32_t i, ret_ptr;
	for (i = 0; i < heap_size; i += 0)
	{

		//os_printf("byte=%d\n",i);

		uint32_t* header_addr = heap + i;
		int32_t header = *header_addr;

		//os_printf("&header=%x\n",header_addr);

		uint32_t* footer_addr = heap + i + sizeof(int32_t) + size;

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
				return heap + ret_ptr;

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
				uint32_t* new_header = heap + i + 2 * sizeof(int32_t) + size;
				*new_header = old_space - occ_space;
				//insert new free block footer
				uint32_t* new_footer = heap + i + sizeof(int32_t) + old_space;
				*new_footer = old_space - occ_space;

				return heap + ret_ptr;
			}
		}
		//jump to the next block
		else
		{
			i = i + abs(header) + 2 * sizeof(int32_t);
		}
	}

	return (void*) 0;
}

void deallocate(void* ptr, uint32_t* heap, int32_t heap_size)
{
	uint32_t first_block = 0;
	uint32_t last_block = 0;

	uint32_t* header_addr = (uint32_t*) ptr - sizeof(int32_t);
	uint32_t size = abs(*header_addr);

	uint32_t* footer_addr = (uint32_t*) ptr + size;

	if (header_addr == heap)
	{
		first_block = 1;
	}

	if (footer_addr + sizeof(int32_t) == (uint32_t*) heap + heap_size)
	{
		last_block = 1;
	}

	//only check and coalesce right block
	if (first_block)
	{
		uint32_t* right_header_addr = (uint32_t*) footer_addr + sizeof(int32_t);
		int32_t right_block_size = *right_header_addr;

		//free right block
		if (right_block_size > 0)
		{
			//set new header at freed blocks header
			*header_addr = size + right_block_size + 2 * sizeof(int32_t);
			//set new footer at right blocks footer
			uint32_t* right_footer_addr = (uint32_t*) footer_addr
					+ 2 * sizeof(int32_t) + right_block_size;
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
		uint32_t* left_block_header = (uint32_t*) header_addr - sizeof(int32_t);
		int32_t left_block_size = *left_block_header;

		//free left block
		if (left_block_size > 0)
		{
			//set new header at left blocks header
			uint32_t* left_header_addr = (uint32_t*) header_addr
					- 2 * sizeof(int32_t) - left_block_size;
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
		uint32_t* right_block_header = (uint32_t*) footer_addr + sizeof(int32_t);
		int32_t right_block_size = *right_block_header;

		uint32_t* left_block_header = (uint32_t*) header_addr - sizeof(int32_t);
		int32_t left_block_size = *left_block_header;

		//both adjacent blocks are free
		if (right_block_size > 0 && left_block_size > 0)
		{
			int32_t new_size = size + right_block_size + left_block_size
					+ 4 * sizeof(int32_t);

			//set new header at left blocks header
			uint32_t* left_header_addr = (uint32_t*) header_addr
					- 2 * sizeof(int32_t) - left_block_size;
			*left_header_addr = new_size;
			//set new footer at right blocks footer
			uint32_t* right_footer_addr = (uint32_t*) footer_addr
					+ 2 * sizeof(int32_t) + right_block_size;
			*right_footer_addr = new_size;
		}

		//only right free block
		else if (right_block_size > 0 && left_block_size < 0)
		{
			//set new header at freed blocks header
			*header_addr = size + right_block_size + 2 * sizeof(int32_t);
			//set new footer at right blocks footer
			uint32_t* right_footer_addr = (uint32_t*) footer_addr
					+ 2 * sizeof(int32_t) + right_block_size;
			*right_footer_addr = size + right_block_size + 2 * sizeof(int32_t);
		}
		//only left free block
		else if (left_block_size > 0 && right_block_size < 0)
		{
			//set new header at left blocks header
			uint32_t* left_header_addr = (uint32_t*) header_addr
					- 2 * sizeof(int32_t) - left_block_size;
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

void mcheck()
{

	char* ptr = (char*) heap;
	uint32_t* end_ptr = (uint32_t*) heap + 2 * sizeof(int32_t) + heap_size;
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
			os_printf("INCONSISTENT HEAP");
			return;
		}

		ptr = ptr + block_size + 2 * sizeof(int32_t);
		block++;
		if ((uint32_t*) ptr == end_ptr)
			return;
	}
}
