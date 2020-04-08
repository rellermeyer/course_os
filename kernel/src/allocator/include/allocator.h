#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define MIN_ORDER 2
#define MAX_ORDER 10

typedef struct {
    uintptr_t base;
    uintptr_t free_lists[MAX_ORDER - MIN_ORDER];
} buddy_alloc_t;

buddy_alloc_t* buddy_init(uintptr_t address, size_t size);
void* buddy_alloc(buddy_alloc_t* buddy, size_t size);
void buddy_dealloc(buddy_alloc_t* buddy, uintptr_t address, size_t size);
void buddy_add_free_item(buddy_alloc_t* buddy, uintptr_t address, size_t order, bool new);
unsigned int get_alloc_size(void* ptr);
