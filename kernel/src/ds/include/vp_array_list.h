#ifndef VP_ARRAYLIST_H
#define VP_ARRAYLIST_H
#include <stdint.h>
#include <ds.h>


typedef struct VPArrayList {
    uint32_t length;
    uint32_t capacity;
    void ** array;
} VPArrayList;

VPArrayList * vpa_create(uint32_t initial_cap);

// Frees the list.
void vpa_free(VPArrayList * arr, FreeFunc freef);

// Gets element of specified index;
void * vpa_get(VPArrayList * list, uint32_t index);

// Sets element to specific data
void vpa_set(VPArrayList * list, uint32_t index, void * data);

// Inserts data at the end of the list
uint32_t vpa_push(VPArrayList * list, void * data);

// Returns the topmost element and removes it fromhe list.
void * vpa_pop(VPArrayList * list);

// Truncates the list to specified size
void vpa_resize(VPArrayList * list, uint32_t new_size, FreeFunc freeFunc);

void * vpa_remove(VPArrayList * list, size_t index);

#endif