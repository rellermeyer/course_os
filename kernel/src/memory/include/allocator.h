#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdint.h>

#define HEAP_INIT_SIZE 0x10000
#define HEAP_MAX_SIZE 0xF0000
#define HEAP_MIN_SIZE 0x10000

#define MIN_ALLOC_SZ 4

#define MIN_WILDERNESS 0x2000
#define MAX_WILDERNESS 0x1000000

#define BIN_COUNT 9
#define BIN_MAX_IDX (BIN_COUNT - 1)

/// Heap

typedef struct node_t {
    uint32_t hole;
    uint32_t size;
    struct node_t* next;
    struct node_t* prev;
} node_t;

typedef struct {
    node_t *header;
} footer_t;

typedef struct {
    node_t* head;
} bin_t;

typedef struct {
    uint32_t start;
    uint32_t end;
    bin_t *bins[BIN_COUNT];

#ifdef MEM_DEBUG
    size_t bytes_allocated;
#endif
} heap_t;

uint32_t overhead;

void create_heap(heap_t *heap, uint32_t start);

void *heap_alloc(heap_t *heap, uint32_t size);
void heap_free(heap_t *heap, void *p);
uint32_t expand(heap_t *heap, uint32_t sz);
void contract(heap_t *heap, uint32_t sz);

uint32_t get_bin_index(uint32_t sz);
void create_foot(node_t *head);
footer_t *get_foot(node_t *head);

node_t *get_wilderness(heap_t *heap);

uint32_t get_alloc_size(void * ptr);

/// Linked list

void add_node(bin_t *bin, node_t *node);

void remove_node(bin_t *bin, node_t *node);

node_t *get_best_fit(bin_t *list, uint32_t size);
node_t *get_last_node(bin_t *list);

node_t *next(node_t *current);
node_t *prev(node_t *current);

#endif