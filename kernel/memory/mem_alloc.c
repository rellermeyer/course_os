#include "mem_alloc.h"
#include "klibc.h"
#include "vm.h"
#include "allocator.h"

uint32_t *nextBlock = (uint32_t*) MEM_START;
uint32_t buffer_size;

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

alloc_handle * allocator;

uint32_t *extend_heap(uint32_t amt);

STATUS init_heap()
{
    int retval = vm_allocate_page(KERNEL_VAS, (void*) MEM_START,
            VM_PERM_PRIVILEGED_RW);
    if (retval) {
        os_printf("ERROR: vm_allocate_page returned %d\n", retval);
        return STATUS_FAIL;
    }

    buffer_size = BLOCK_SIZE;
    allocator = alloc_create(MEM_START, buffer_size, &extend_heap);
    return STATUS_OK;
}

// TODO: what if there's an error allocating the page?
// Returns a pointer to the new (big) free block's header
uint32_t *extend_heap(uint32_t amt)
{
    uint32_t amt_added = 0;
    while (amt_added < amt) {
        int retval = vm_allocate_page(KERNEL_VAS,
                (void*) (MEM_START + buffer_size), VM_PERM_PRIVILEGED_RW);
        if (retval) {
            os_printf("ERROR: vm_allocate_page(,%d,) returned %d\n",
                    MEM_START + amt_added, retval);
            break;
        }
        amt_added += BLOCK_SIZE;
        buffer_size += BLOCK_SIZE;
    }

    return amt_added;
}

void* allocate(uint32_t size, uint32_t* heap, int32_t heap_size)
{
	return alloc_allocate(allocator, size);
}

void deallocate(void* ptr, uint32_t* heap, int32_t heap_size)
{
    return alloc_deallocate(allocator, ptr);
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

alloc_handle * mem_get_allocator(){
    return allocator;
}

STATUS mem_check(){
    return alloc_check(allocator);
}

void test_allocate()
{
	os_printf("***** Test code for allocater (test_allocate()): *****\n");

	if (alloc_check(allocator)) {
		os_printf("Inconsistent memory to begin with...\n");
	}

	// Allocate some memory
	char *p = alloc_allocate(allocator, 15);
	p = alloc_allocate(allocator, 15);
	os_strcpy(p, "Hello!");
	os_printf("%s\n", p);

	if (alloc_check(allocator)) {
		os_printf("allocate(15) failed.\n\n");
		int i;
		for (i=0; i<8; i++) {
			int j;
			for (j=0; j<8; j++) {
				os_printf("%X ", ((char*)alloc_get_heap(allocator))[i*8+j]);
			}
			os_printf("\n");
		}
	}

	// Allocate a bunch of blocks of "random" size
	uint64_t rng = 1234567891;

	char *pntrs[256];
	uint32_t alloced = 0;
	int i;
	os_printf("Starting out w/ %u bytes of heap\n", alloc_get_heap_size(allocator));
	for (i=0; i<256; i++) {
		uint32_t size = gen_rand(&rng, 15);
		pntrs[i] = allocate(size, alloc_get_heap(allocator), alloc_get_heap_size(allocator));
		alloced += size;
		os_printf("%u %u %u %u %d\n", i, alloc_get_heap_size(allocator), size, alloced, vm_count_free_frames());

		/*if (alloc_check(allocator)) {
			os_printf("Memory is inconsistent. Stopping test.\n");
			break;
			}*/
	}

	// Test one of them
	pntrs[230][0] = 1;
	os_printf("%d == 1?\n", pntrs[230][0]);

	if (alloc_check(allocator)) {
		os_printf("Memory is inconsistent :-(\n");
	} else {
		os_printf("Memory is consistent :-)\n");
	}

	// Free all the pntrs
	for (i=0; i<256; i++) {
		if (pntrs[i]) {
			deallocate(pntrs[i], alloc_get_heap(allocator), alloc_get_heap_size(allocator));
		}
	}

	deallocate(p, alloc_get_heap(allocator), alloc_get_heap_size(allocator));

	if (alloc_check(allocator)) {
		os_printf("Memory is inconsistent :-(\n");
	} else {
		os_printf("Memory is consistent :-)\n");
	}

	os_printf("heap_size = %d bytes\n", alloc_get_heap_size(allocator));
	os_printf("******************************************************\n");
}
