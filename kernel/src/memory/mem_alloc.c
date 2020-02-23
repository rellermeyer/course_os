

#include "klibc.h"
#include <mem_alloc.h>
#include <allocator.h>
#include <vm.h>
#include <string.h>

uint32_t *nextBlock = (uint32_t*) MEM_START;
uint32_t buffer_size;
heap_t * allocator;
uint32_t __mem_extend_heap(uint32_t amt);

heap_t * proc_allocator;
uint32_t proc_buffer_size;
uint32_t __mem_extend_proc_heap(uint32_t amt);

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
            os_printf("ERROR: vm_allocate_page(,%d,) returned %d\n",
                    MEM_START + amt_added, retval);
            break;
        }
        amt_added += BLOCK_SIZE;
        buffer_size += BLOCK_SIZE;
    }

    return amt_added;
}

uint32_t init_heap()
{
    // Allocate space for the heap_t struct. is too large but at least big enough.
    int retval = vm_allocate_page(KERNEL_VAS, (void*) MEM_START, VM_PERM_PRIVILEGED_RW);
    if (retval) {
        os_printf("ERROR: vm_allocate_page returned %d\n", retval);
        return STATUS_FAIL;
    }

    heap_t * heap = (heap_t * ) MEM_START;
    memset(heap, 0, sizeof(heap_t));

    uint32_t addr = (uint32_t)((void *) MEM_START + sizeof(heap_t));

    for(int i = 0; i < BIN_COUNT; i++) {
        heap->bins[i] = (bin_t *) addr;
        memset(heap->bins[0], 0, sizeof(bin_t));
        addr += sizeof(bin_t);
    }

    assert(MEM_START + sizeof(heap_t) + BIN_COUNT * sizeof(bin_t) == addr)


    // Find the next page boundary above addr
    addr = (addr & ~(BLOCK_SIZE-1)) + BLOCK_SIZE;

    void * ret = vm_allocate_pages(KERNEL_VAS, (void *) addr, HEAP_INIT_SIZE, VM_PERM_PRIVILEGED_RW);
    if(addr + HEAP_INIT_SIZE != (uint32_t) ret) {
        os_printf("ERROR: vm_allocate_pages allocated a different amount than requested\n");
        return STATUS_FAIL;
    }

    //    allocator = alloc_create((uint32_t*) MEM_START, buffer_size, &__mem_extend_heap);
    create_heap(heap, addr);
    allocator = heap;
    return STATUS_OK;
}

uint32_t init_process_heap(struct vas* vas) {
    int retval = vm_allocate_page(vas, (void*) PROC_START, VM_PERM_USER_RW);
    if (retval) {
        os_printf("ERROR: vm_allocate_page returned %d\n", retval);
        return STATUS_FAIL;
    }

    heap_t * heap = (heap_t * ) PROC_START;
    uint32_t addr = (uint32_t)((void *) PROC_START + sizeof(heap_t));

    for(int i = 0; i < BIN_COUNT; i++) {
        heap->bins[i] = (bin_t *) addr;
        addr += sizeof(bin_t);
    }

    // Find the next page boundary above addr
    addr = (addr & ~(BLOCK_SIZE-1)) + BLOCK_SIZE;

    void * ret = vm_allocate_pages(vas, (void *) addr, HEAP_INIT_SIZE, VM_PERM_USER_RW);
    if(addr + HEAP_INIT_SIZE != (uint32_t) ret){
        os_printf("ERROR: vm_allocate_pages allocated a different amount than requested\n");
        return STATUS_FAIL;
    }

    create_heap(heap, addr);
    proc_allocator = heap;
    vm_free_mapping(KERNEL_VAS, (void*) PROC_START); // ?
    return STATUS_OK;
}

uint32_t __mem_extend_proc_heap(uint32_t amt) {
	struct vas* pvas = vm_get_current_vas();

    uint32_t amt_added = 0;
    while (amt_added < amt)
    {
        int retval = vm_allocate_page(pvas, (void*) PROC_START, VM_PERM_USER_RW);
        vm_map_shared_memory(KERNEL_VAS, (void*)PROC_START, pvas, (void*)PROC_START, VM_PERM_USER_RW);
        if (retval) {
            os_printf("ERROR: vm_allocate_page returned %d\n", retval);
            return STATUS_FAIL;
        }
        else{
            os_printf("Page allocated for process heap at %x:\n",PROC_START);
        }
        amt_added += BLOCK_SIZE;
        proc_buffer_size += BLOCK_SIZE;
    }

    vm_free_mapping(KERNEL_VAS,(void*)PROC_START);
    return amt_added;
}

void* proc_allocate(uint32_t size)
{
    return heap_alloc(proc_allocator, size);
}

void proc_deallocate(void* ptr)
{
    return heap_free(proc_allocator, ptr);
}

void *allocate(uint32_t size) {
	return heap_alloc(allocator, size);
}

void deallocate(void *ptr) {
    return heap_free(allocator, ptr);
}

uint32_t allocation_size(void* ptr) {
    return get_alloc_size(ptr);
}

heap_t * mem_get_allocator(){
    return allocator;
}

heap_t * mem_get_proc_allocator(){
    return proc_allocator;
}

uint32_t mem_get_heap_size(){
    return allocator->end - allocator->start;
}
