//
// Created by laura on 3/22/20.
//

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>


#ifndef BUDDY_H
#define BUDDY_H

// header size for each allocation.
#define HEADER_SIZE 8

// minimum allocation size is 16, because we already have an 8 byte header and need to stay 8 byte alligned
#define MIN_ALLOC_LOG2 4
#define MIN_ALLOC ((size_t)1 << MIN_ALLOC_LOG2)

//we also define a max alloc size
#define MAX_ALLOC_LOG2 31
#define MAX_ALLOC ((size_t)1 << MAX_ALLOC_LOG2)

//we define a number of buckets for each size that are each associated with the free list for that particular alloc size.
//Given a bucket index, the size of the allocations in that bucket can be
//found with "(size_t)1 << (MAX_ALLOC_LOG2 - bucket)"

#define BUCKET_COUNT (MAX_ALLOC_LOG2 - MIN_ALLOC_LOG2 + 1)

// define a list to store free blocks for each allocation size, along with an array of buckets (that each correspond to 1 size).
typedef struct list_t {
    struct list_t *prev, *next;
} list_t;

static list_t buckets[BUCKET_COUNT];
static size_t bucket_limit;


static uint8_t node_is_split[(1 << (BUCKET_COUNT - 1)) / 8];
//min and max memory addresses used by this allocator
static uint8_t *base_ptr;
static uint8_t *max_ptr;

static int update_max_ptr(uint8_t *new_value);

// Circularly-Linked-List utility functions.
static void list_init(list_t *list);
static void list_push(list_t *list, list_t *entry);
static void list_remove(list_t *entry);
static list_t *list_pop(list_t *list);

//Buddy related functions

static uint8_t *ptr_for_node(size_t index, size_t bucket);
static size_t node_for_ptr(uint8_t *ptr, size_t bucket);

static int parent_is_split(size_t index);
static void flip_parent_is_split(size_t index);

static size_t bucket_for_request(size_t request);
static int lower_bucket_limit(size_t bucket);

void * alloc_buddy(size_t request);
void free(void *ptr);
#endif