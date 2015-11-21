/*
 * array_list.c
 *
 *  Created on: Apr 21, 2015
 *      Author: kittenRainbow
 */

#include "data_structures/array_list.h"

arrl_handle* arrl_create() {
    return arrl_create_fixed(DEFAULT_BUCKET_SIZE);
}

arrl_handle* arrl_create_fixed(uint32_t bucket_size) {
    arrl_handle* result = (arrl_handle *) kmalloc(sizeof(arrl_handle));
    void** bucket = kmalloc(bucket_size * sizeof(void*));
    llist_handle * l = llist_create(bucket);
    result->linked_list = l;
    result->size = 0;
    result->capacity = bucket_size;
    return result;
}

void arrl_append(arrl_handle* arrl, void* elem) {
    llist_node * curr_bucket;
    uint32_t list_size;
    uint32_t last_bucket_size;

    if (arrl->capacity == arrl->size) {
        void* data = kmalloc(arrl->bucket_size * sizeof(void*));
        llist_enqueue(arrl->linked_list, data);
        arrl->capacity += arrl->bucket_size;
    }

    curr_bucket = arrl->linked_list->head;
    list_size = arrl->size / arrl->bucket_size;
    last_bucket_size = arrl->size % arrl->bucket_size;

    for(uint32_t i=0; i < list_size; ++i) {
        curr_bucket= curr_bucket->next;
    }
    *(((int**)curr_bucket->data) + last_bucket_size) = (int*)elem;
}

void arrl_remove(arrl_handle* arrl, void* elem) {
    uint32_t bucket_size;
    uint32_t list_size;
    uint32_t bucket_index;
    uint32_t list_index;
    llist_node* curr_bucket;
    llist_node* tail;

    bucket_size = arrl->bucket_size;
    list_size = arrl->linked_list->count;
    curr_bucket = arrl->linked_list->head;
    tail = arrl->linked_list->tail;

    for (list_index = 0; list_index < list_size; ++list_index) {
        for (bucket_index = 0; bucket_index < bucket_size; ++bucket_index) {
            //if (elem == curr_bucket->data[bucket_index]) {
                kfree(elem);
                goto out;
            //}
        }
        curr_bucket = curr_bucket->next;
    }

    // Except the last bucket, shift each bucket up by one and copy first element of the next bucket to the last cell of the current bucket
    out: while (curr_bucket != tail) {
        os_memcpy(*(((uint32_t**)curr_bucket->data) + bucket_index + 1),
                  *(((uint32_t**)curr_bucket->data) + bucket_index + 0),
                bucket_size - bucket_index - 1);
        bucket_index = 0;
        *(((int**)curr_bucket->data) + bucket_size - 1) = *((int**)curr_bucket->next->data);
        curr_bucket = curr_bucket->next;
    }

    --arrl->size;
}

uint32_t arrl_contains(arrl_handle* arrl, void* elem) {
    uint32_t bucket_size = arrl->bucket_size;
    uint32_t list_size = arrl->linked_list->count;
    llist_node* bucket = arrl->linked_list->head;

    for (uint32_t list_index = 0; list_index < list_size; ++list_index) {
        for (uint32_t bucket_index = 0; bucket_index < bucket_size;
                ++bucket_index) {
            //if (elem == bucket->data[bucket_index]) {
                return 1;
            //}
        }
        bucket = bucket->next;
    }
    return 0;
}

uint32_t arrl_index_of(arrl_handle* arrl, void* elem) {
    int bucket_size = arrl->bucket_size;
    int list_size = arrl->linked_list->count;
    llist_node* bucket = arrl->linked_list->head;

    for (int list_index = 0; list_index < list_size; ++list_index) {
        for (int bucket_index = 0; bucket_index < bucket_size;
                ++bucket_index) {
            //if (elem == bucket->data[bucket_index]) {
                return list_index * DEFAULT_BUCKET_SIZE + bucket_index;
            //}
        }
        bucket = bucket->next;
    }
    return -1;
}

uint32_t arrl_count(arrl_handle* arrl) {
    return arrl->size;
}

