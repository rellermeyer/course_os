
#ifndef VP_SINGLY_LINKED_LIST_H
#define VP_SINGLY_LINKED_LIST_H

#include <ds.h>

// TODO: Docs

struct VPSinglyLinkedListLink {
    struct VPSinglyLinkedListLink * next;
    void * data;
};

typedef struct VPSinglyLinkedList {
    struct VPSinglyLinkedListLink * head;
    size_t length;
} VPSinglyLinkedList;

// Iterator over a SLL
typedef struct VPSinglyLinkedListLink * VPSinglyLinkedListIterator;

VPSinglyLinkedListIterator vpslli_create(VPSinglyLinkedList * lst);
void * vpslli_next(VPSinglyLinkedListIterator * lsti);
bool vpslli_empty(VPSinglyLinkedListIterator lsti);

VPSinglyLinkedList * vpsll_create();
void vpsll_free(VPSinglyLinkedList * lst, FreeFunc freef);
void vpsll_push(VPSinglyLinkedList * lst, void * data);
void vpsll_push_to_back(VPSinglyLinkedList * lst, void * data);

void * vpsll_pop(VPSinglyLinkedList * lst);

// Remove the first item for which compf returns true.
void * vpsll_remove(VPSinglyLinkedList * lst, void * data, CompareFunc compf);
void * vpsll_get(VPSinglyLinkedList * lst, size_t index);

// returns the old value to free
void * vpsll_set(VPSinglyLinkedList * lst, size_t index, void * value);

// Returns if the compf returns true for any item in the list (compared with the value).
bool vpsll_contains(VPSinglyLinkedList * lst, void * value, CompareFunc compf);
void *vpsll_find(VPSinglyLinkedList * lst, void * value,  CompareFunc compf);

size_t vpsll_length(VPSinglyLinkedList * lst);

#define VPSLL_FOREACH(lst, i)                                           \
    for (VPSinglyLinkedListIterator i = vpslli_create(lst); !vpslli_empty(i); vpslli_next(&(i)))
#endif
