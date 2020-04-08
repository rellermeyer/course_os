///


#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define HEAP_SIZE ((size_t)((1 << 20) + sizeof(buddy_allocator)))

#define MIN_ORDER 4
#define MAX_ORDER 21

typedef struct {
    size_t base;
    size_t free_lists[MAX_ORDER - MIN_ORDER];

#ifdef MEM_DEBUG
    size_t bytes_allocated;
#endif
} buddy_allocator;

typedef buddy_allocator heap_t;

typedef struct {
    size_t size;
} buddy_allocation_header;

typedef struct {
    buddy_allocation_header header;
    uint8_t allocation[];
} buddy_allocation;

buddy_allocator * buddy_init(buddy_allocator *address, size_t size);
void* buddy_alloc(buddy_allocator * buddy, size_t size);
void buddy_dealloc(buddy_allocator * buddy, size_t address);
size_t get_alloc_size(size_t address);
void buddy_status(buddy_allocator * buddy);

#endif
