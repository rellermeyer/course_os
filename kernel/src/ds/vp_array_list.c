#include <vp_array_list.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

struct VPArrayList * vpa_create(uint32_t initial_cap) {
    VPArrayList * list = kmalloc(sizeof(VPArrayList));
    *list = (VPArrayList) {
            .length = 0,
            .capacity = initial_cap,
            .array = kmalloc(initial_cap * sizeof(void *))
    };

    return list;
}

void vpa_free(VPArrayList * arr, FreeFunc freefunc) {

    if (freefunc != NULL) {
        for (uint32_t i = 0; i < arr->length; i++){
            freefunc(arr->array[i]);
        }
    }

    kfree(arr->array);
    kfree(arr);
}

void * vpa_get(VPArrayList * list, uint32_t index) {
    return list->array[index];
}

// Sets element to specific data
void vpa_set(VPArrayList * list, uint32_t index, void * data) {
    list->array[index] = data;
}

// Inserts data at the end of the list. Returns the new size.
uint32_t vpa_push(VPArrayList * list, void * data) {

    list->array[list->length++] = data;

    if(list->length >= list->capacity) {
        uint32_t new_size = max((list->capacity + (list->capacity >> 2u)), list->capacity + 2);

        assert(new_size > list->capacity)

        list->capacity = new_size;

        list->array = krealloc(list->array, list->capacity * sizeof(void *));

    }

    return list->length;
}

void * vpa_pop(VPArrayList * list) {
    if(list->length == 0) {
        return 0;
    }

    void * res = list->array[--list->length];

    if(list->length < (list->capacity / 2)) {
        list->capacity /= 2;
        list->array = krealloc(list->array, list->capacity * sizeof(void *));
    }

    return res;
}

// Resizes the list to specified size
void vpa_resize(VPArrayList * list, uint32_t new_size, FreeFunc freeFunc) {
    list->capacity = new_size;

    if(list->length > list->capacity) {
        if(freeFunc != NULL){
            for(uint32_t i = list->length; i > list->capacity; i--) {
                freeFunc(list->array[i]);
            }
        }

        list->length = list->capacity;
    }

    list->array = krealloc(list->array, new_size);
}

void * vpa_remove(VPArrayList * list, size_t index);
