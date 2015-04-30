/*
 * array_list.c
 *
 *  Created on: Apr 21, 2015
 *      Author: kittenRainbow
 */

#include "../include/array_list.h"

arrl_handle* arrl_create() {
    return arrl_create_fixed(DEFAULT_BUCKET_SIZE);
}

arrl_handle* arrl_create_fixed(uint32_t bucket_size) {
    arrl_handle* result = (arrl_handle *) kmalloc(sizeof(arrl_handle));
    result->list = (void***) kmalloc(sizeof(void**));
    void** bucket = kmalloc(bucket_size * sizeof(void*));
    result->list[0] = bucket;
    result->size = 0;
    result->capacity = bucket_size;
    return result;
}

void arrl_append(arrl_handle* arrl, void* elem) {
    uint32_t list_index;
    uint32_t bucket_index;

    list_index = arrl->size / arrl->bucket_size;
    bucket_index = arrl->size % arrl->bucket_size;

    if (arrl->capacity == arrl->size) {
        void*** list = (void***) kmalloc(
                (arrl->size / arrl->bucket_size + 1) * sizeof(void**));
        for (uint32_t index = 0; index < list_index; ++index) {
            list[index] = arrl->list[index];
        }

        kfree(arrl->list);
        arrl->list = list;
        list[list_index] = (void**) kmalloc(sizeof(void*) * arrl->bucket_size);
        arrl->capacity += arrl->bucket_size;
    }

    arrl->list[list_index][bucket_index] = elem;
    ++arrl->size;
}

void* arrl_get(arrl_handle* arrl, uint32_t index) {
    uint32_t list_index;
    uint32_t bucket_index;

    if (index >= arrl->size) {
        return 0;
    }

    list_index = arrl->size / index;
    bucket_index = arrl->size % index;

    return arrl->list[list_index][bucket_index];
}

void arrl_remove(arrl_handle* arrl, void* elem) {
    uint32_t bucket_size;
    uint32_t list_size;
    uint32_t bucket_index;
    uint32_t list_index;

    bucket_size = arrl->bucket_size;
    list_size = arrl->size / bucket_size;

    for (list_index = 0; list_index < list_size; ++list_index) {
        for (bucket_index = 0; bucket_index < bucket_size; ++bucket_index) {
            if (elem == arrl->list[list_index][bucket_index]) {
                goto out;
            }
        }
    }

    // Except the last bucket, shift each bucket up by one and copy first element of the next bucket to the last cell of the current bucket
    out: while (list_index != list_size) {
        os_memcpy(arrl->list[list_index][bucket_index + 1],
                arrl->list[list_index][bucket_index],
                (bucket_size - bucket_index - 1) * sizeof(void*));
        bucket_index = 0;
        arrl->list[list_index][bucket_size - 1] = arrl->list[list_index + 1][0];
        ++list_index;
    }
    os_memcpy(arrl->list[list_index][bucket_index + 1],
            arrl->list[list_index][bucket_index],
            ((arrl->size % bucket_index) - 1) * sizeof(void*));
    --arrl->size;
}

/*
void arrl_remove_all(arrl_handle* arrl, void* elem) {
    uint32_t bucket_size;
    uint32_t next_index;
    uint32_t bucket_index;
    uint32_t index;
    uint32_t start_index;

    bucket_size = arrl->bucket_size;
    start_index = 0;
    size = arrl->list[arrl->size / arrl->bucket_size][arrl->size
            % arrl->bucket_size - 1];

    index = __find_next(arrl, start_index, elem);
    if (index == -1) {
        return;
    }

    do {
        next_index = __find_next(arrl, index, elem);
        if (next_index == 0) {
            arrl_remove(arrl, elem);
            return;
        }
        for (uint32_t i = index; i < next_index / arrl->bucket_size; ++i) {
            os_memcpy(arrl->list[i] + ,
                            arrl->list[list_index][bucket_index],
                            (bucket_size - bucket_index - 1) * sizeof(void*));
        }

        bucket_index = 0;
        arrl->list[list_index][bucket_size - 1] = arrl->list[list_index + 1][0];
        ++list_index;

        os_memcpy(arrl->list[list_index][bucket_index + 1],
                arrl->list[list_index][bucket_index],
                ((arrl->size % bucket_index) - 1) * sizeof(void*));
        --arrl->size;
    } while (list_index != list size);
}

*/

uint32_t arrl_contains(arrl_handle* arrl, void* elem) {
    uint32_t bucket_size = arrl->bucket_size;
    uint32_t list_size = arrl->size / bucket_size;

    for (uint32_t list_index = 0; list_index < list_size; ++list_index) {
        for (uint32_t bucket_index = 0; bucket_index < bucket_size;
                ++bucket_index) {
            if (elem == arrl->list[list_index][bucket_index]) {
                return 1;
            }
        }
    }
    return 0;
}

uint32_t arrl_index_of(arrl_handle* arrl, void* elem) {
    uint32_t bucket_size = arrl->bucket_size;
    uint32_t list_size = arrl->size / bucket_size;

    for (int list_index = 0; list_index < list_size; ++list_index) {
        for (int bucket_index = 0; bucket_index < bucket_size; ++bucket_index) {
            if (elem == arrl->list[list_index][bucket_index]) {
                return list_index * arrl->bucket_size + bucket_index;
            }
        }
    }
    return -1;
}

uint32_t arrl_count(arrl_handle* arrl) {
    return arrl->size;
}

uint32_t __find_next(arrl_handle* arrl, uint32_t start, void* elem) {
 //   uint32_t list_index;
    uint32_t list_size;
    uint32_t bucket_size;

    for (uint32_t list_index = start / arrl->bucket_size;
            list_index < list_size; ++list_index) {
        for (uint32_t bucket_index = start % arrl->bucket_size;
                bucket_index < bucket_size; ++bucket_index) {
            if (arrl->list[list_index][bucket_index] == elem) {
                return list_index * arrl->bucket_size + bucket_index;
            }
        }
    }
    return -1;
}

