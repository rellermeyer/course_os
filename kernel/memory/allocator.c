#include "allocator.h"

alloc_handle* alloc_create(uint32_t * heap, uint32_t size, extend_heap_handle extend_handler){
  alloc_handle* alloc_handle_ptr;
  if(size <= sizeof(alloc_handle)) {
    return 0;
  }

  char * heap_start = heap + sizeof(alloc_handle);
  alloc_handle_ptr = (alloc_handle*) &heap[0];
  heap[0] = heap_start;
  heap[sizeof(uint32_t*)] = size - sizeof(alloc_handle);
  heap[sizeof(uint32_t*) + sizeof(extend_heap_handle)] = extend_handler;
  return alloc_handle_ptr;
}

uint32_t* __alloc_extend_heap(alloc_handle*allocator, uint32_t amount){
  uint32_t start_size = allocator->heap_size;
  uint32_t amount_added = allocator->extend_handler(amount);

    if(amount_added == 0){
       return 0;
    }

  // Now extend the footer block
  uint32_t *orig_footer = (uint32_t*)((void*)MEM_START+start_size-sizeof(uint32_t));

    allocator->heap_size += amount_added;

  // If it's free, simply move it (and update the header)
  // If it's used, add a free block to the end
  if (*orig_footer > 0) {
    uint32_t *orig_header = (uint32_t*)(MEM_START+start_size-2*sizeof(uint32_t)-*orig_footer);
    uint32_t *new_footer = (uint32_t*)(MEM_START+allocator->heap_size-sizeof(uint32_t));
    *new_footer = *orig_footer + amount_added;
    *orig_header += amount_added;
    return orig_header;
  } else {
    uint32_t *new_header = (uint32_t*)(MEM_START+start_size);
    uint32_t *new_footer = (uint32_t*)(MEM_START+allocator->heap_size-sizeof(uint32_t));
    *new_header = amount_added - 2*sizeof(uint32_t);
    *new_footer = amount_added - 2*sizeof(uint32_t);
    return new_header;
  }
  return 0x0;
}

void* alloc_allocate(alloc_handle * allocator, uint32_t size)
{
  if (size > (allocator->heap_size + 2 * sizeof(int32_t))) {
    return 0;
  }

  int32_t i, ret_ptr;
  
  for (i = 0; i < allocator->heap_size;)
  {
    uint32_t* header_addr = (uint32_t*)((void*)allocator->heap + i);
    int32_t header = *header_addr;

    uint32_t* footer_addr = (uint32_t*)((void*)allocator->heap + i + sizeof(int32_t) + size);

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
        return (uint32_t*)((void*)allocator->heap + ret_ptr);

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
        uint32_t* new_header = (uint32_t*)((void*)allocator->heap + i + 2 * sizeof(int32_t) + size);
        *new_header = old_space - occ_space;
        //insert new free block footer
        uint32_t* new_footer = (uint32_t*)((void*)allocator->heap + i + sizeof(int32_t) + old_space);
        *new_footer = old_space - occ_space;

        return (uint32_t*)((void*)allocator->heap + ret_ptr);
      }
    }
    //jump to the next block
    else
    {
      i = i + abs(header) + 2 * sizeof(int32_t);
    }
  }

  // Allocate some more memory.
  uint32_t new_amt = size+2*sizeof(uint32_t);
  uint32_t *header = __alloc_extend_heap(new_amt);

    if(header == 0) {
      return 0;
    }
    
  // Recursive call. TODO: (relatively) Inefficient
  return alloc_allocate(allocator, size);
}

void alloc_deallocate(alloc_handle* allocator, void* ptr)
{
  uint32_t first_block = 0;
  uint32_t last_block = 0;

  uint32_t* header_addr = (uint32_t*) ((void*)ptr - sizeof(int32_t));
  uint32_t size = abs(*header_addr);

  uint32_t* footer_addr = (uint32_t*) ((void*)ptr + size);

  if (header_addr == allocator->heap)
  {
    first_block = 1;
  }

  if (footer_addr + sizeof(int32_t) == (uint32_t*) allocator->heap + allocator->heap_size)
  {
    last_block = 1;
  }

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

STATUS alloc_check(alloc_handle* allocator)
{
  char* ptr = (char*) allocator->heap;
  uint32_t* end_ptr = (uint32_t*) ((void*)allocator->heap + allocator->heap_size);
  int i, block = 0;

  for (i = 0; i < allocator->heap_size; i += 0)
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
      return STATUS_FAILED;
    }

    ptr = ptr + block_size + 2 * sizeof(int32_t);
    block++;
    if ((uint32_t*) ptr == end_ptr){
      return STATUS_OK;
    }
  }
  return STATUS_OK;
}
