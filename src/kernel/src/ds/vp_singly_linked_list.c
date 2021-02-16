#include <stdio.h>
#include <stdlib.h>
#include <vp_singly_linked_list.h>

VPSinglyLinkedList * vpsll_create() {
    VPSinglyLinkedList * res = kmalloc(sizeof(VPSinglyLinkedList));
    res->head = NULL;
    res->length = 0;
    return res;
}


void vpsll_free(VPSinglyLinkedList * lst, FreeFunc freef) {
    struct VPSinglyLinkedListLink * curr = lst->head;
    while (curr != NULL) {
        struct VPSinglyLinkedListLink * last = curr;
        curr = curr->next;
        if (freef != NULL) { freef(last->data); }
        kfree(last);
    }
    kfree(lst);
}

void vpsll_push(VPSinglyLinkedList * lst, void * data) {
    struct VPSinglyLinkedListLink * node = kmalloc(sizeof(struct VPSinglyLinkedListLink));
    node->data = data;

    node->next = lst->head;
    lst->head = node;

    lst->length++;
}

void * vpsll_pop(VPSinglyLinkedList * lst) {
    struct VPSinglyLinkedListLink * oldhead = lst->head;
    void * contents = oldhead->data;

    lst->head = oldhead->next;
    kfree(oldhead);

    lst->length--;

    return contents;
}


// Remove the first item for which compf returns true.
void * vpsll_remove(VPSinglyLinkedList * lst, void * data, CompareFunc compf) {
    struct VPSinglyLinkedListLink * prev = NULL;
    VPSLL_FOREACH(lst, i) {
        if (compf(i->data, data)) {
            void * value = i->data;
            if (prev == NULL) {
                lst->head = i->next;
            } else {
                prev->next = i->next;
                kfree(i);
            }
            lst->length--;
            return value;
        }
        prev = i;
    }

    return NULL;
}

inline static struct VPSinglyLinkedListLink * __link_at_index(VPSinglyLinkedList * lst,
                                                              size_t index) {
    if (index > lst->length) { return NULL; }
    VPSLL_FOREACH(lst, i) {
        if (index-- == 0) { return i; }
    }

    return NULL;
}

void * vpsll_get(VPSinglyLinkedList * lst, size_t index) {
    return __link_at_index(lst, index)->data;
}

// returns the old value to free
void * vpsll_set(VPSinglyLinkedList * lst, size_t index, void * value) {
    struct VPSinglyLinkedListLink * link = __link_at_index(lst, index);
    if (link == NULL) { return NULL; }

    void * old = link->data;
    link->data = value;

    return old;
}

// Returns if the compf returns true for any item in the list (compared with the value).
bool vpsll_contains(VPSinglyLinkedList * lst, void * value, CompareFunc compf) {
    VPSLL_FOREACH(lst, i) {
        if (compf(i->data, value)) { return true; }
    }
    return false;
}

size_t vpsll_length(VPSinglyLinkedList * lst) {
    return lst->length;
}

VPSinglyLinkedListIterator vpslli_create(VPSinglyLinkedList * lst) {
    return lst->head;
}

void * vpslli_next(VPSinglyLinkedListIterator * lsti) {
    void * nxt = (*lsti)->data;
    *lsti = (*lsti)->next;
    return nxt;
}

bool vpslli_empty(VPSinglyLinkedListIterator lsti) {
    return lsti == NULL;
}
