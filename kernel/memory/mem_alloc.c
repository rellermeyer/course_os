#include "mem_alloc.h"
#include "vm.h"
#include "allocator.h"
#include "klibc.h"

uint32_t *nextBlock = (uint32_t*) MEM_START;
uint32_t buffer_size;
alloc_handle * allocator;
uint32_t __mem_extend_heap(uint32_t amt);

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

// TODO: what if there's an error allocating the page?
// Returns a pointer to the new (big) free block's header
uint32_t __mem_extend_heap(uint32_t amt)
{
    uint32_t amt_added = 0;
    while (amt_added < amt) {
        int retval = vm_allocate_page(KERNEL_VAS,
                (void*) (MEM_START + buffer_size), VM_PERM_PRIVILEGED_RW);
        if (retval) {
            /*os_printf("ERROR: vm_allocate_page(,%d,) returned %d\n",
                    MEM_START + amt_added, retval);*/
            break;
        }
        amt_added += BLOCK_SIZE;
        buffer_size += BLOCK_SIZE;
    }

    return amt_added;
}

uint32_t init_heap()
{
    int retval = vm_allocate_page(KERNEL_VAS, (void*) MEM_START,
            VM_PERM_PRIVILEGED_RW);
    if (retval) {
        //os_printf("ERROR: vm_allocate_page returned %d\n", retval);
        return STATUS_FAIL;
    }

    buffer_size = BLOCK_SIZE;
    allocator = alloc_create((uint32_t*) MEM_START, buffer_size, &__mem_extend_heap);
    return STATUS_OK;
}

void* allocate(uint32_t size, uint32_t* heap, int32_t heap_size)
{
	return alloc_allocate(allocator, size);
}

void deallocate(void* ptr, uint32_t* heap, int32_t heap_size)
{
    return alloc_deallocate(allocator, ptr);
}

alloc_handle * mem_get_allocator(){
    return allocator;
}

int mem_check(){
    return alloc_check(allocator);
}

uint32_t mem_get_heap_size(){
    return alloc_get_heap_size(allocator);
}
