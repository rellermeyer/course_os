/*
 * array_list.h
 *
 *  Created on: Apr 21, 2015
 *      Author: namaz89
 */

#ifndef KERNEL_INCLUDE_ARRAY_LIST_H_
#define KERNEL_INCLUDE_ARRAY_LIST_H_

#include "data_structures/linked_list.h"
#include "klibc.h"

#define DEFAULT_BUCKET_SIZE 20

typedef struct arrl_handle arrl_handle;

struct arrl_handle {
    llist_handle * linked_list;
    int bucket_size;
    int size;
    int capacity;
};

arrl_handle* arrl_create();
arrl_handle* arrl_create_fixed(uint32_t bucket_size);
void arrl_append(arrl_handle* arrl, void* elem);
void arrl_remove(arrl_handle* arrl, void* elem);
void arrl_remove_all(arrl_handle* arrl, void* elem);
uint32_t arrl_contains(arrl_handle* arrl, void* elem);
uint32_t arrl_index_of(arrl_handle* arrl, void* elem);
uint32_t arrl_count(arrl_handle* arrl);

#endif /* KERNEL_INCLUDE_ARRAY_LIST_H_ */
