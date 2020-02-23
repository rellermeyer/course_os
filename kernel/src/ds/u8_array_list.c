//
// Created by victor on 2/21/20.
//
#include <u8_array_list.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

U8ArrayList * u8a_create(uint32_t initial_cap) {
    U8ArrayList * list = kmalloc(sizeof(U8ArrayList));

    *list = (U8ArrayList) {
        .length = 0,
        .capacity = initial_cap,
        .array = kmalloc(initial_cap * sizeof(uint8_t))
    };

    return list;
}

void u8a_free(U8ArrayList * arr) {
    if(arr->capacity > 0) {
        kfree(arr->array);
    }
    kfree(arr);
}

uint8_t u8a_get(U8ArrayList * list, uint32_t index) {
    return list->array[index];
}

// Sets element to specific data
void u8a_set(U8ArrayList * list, uint32_t index, uint8_t data) {
    list->array[index] = data;
}

// Inserts data at the end of the list. Returns the new size.
uint32_t u8a_push(U8ArrayList * list, uint8_t data) {

    if(list->capacity == 0 || list->length >= list->capacity - 1) {
        uint32_t new_size = max((list->capacity + (list->capacity >> 2)), list->capacity + 2);

        assert(new_size > list->capacity)

        list->capacity = new_size;

        list->array = krealloc(list->array, list->capacity * sizeof(uint8_t));
    }
    list->array[list->length++] = data;

    return list->length;
}

uint32_t u8a_push_string(U8ArrayList * arr, char * data) {
    size_t length = strlen(data);

    u8a_resize(arr, arr->length + length);

    for (int i = 0; i < length; i++) {
        arr->array[arr->length++] = data[i];
    }

    return arr->length;
}

uint8_t u8a_pop(U8ArrayList * list) {
    if(list->length == 0) {
        return 0;
    }

    uint8_t res = list->array[--list->length];

    if(list->length < (list->capacity / 2)) {
        list->capacity /= 2;
        list->array = krealloc(list->array, list->capacity * sizeof(uint8_t));
    }

    return res;
}

// Resizes the list to specified size
void u8a_resize(U8ArrayList * list, uint32_t new_size) {
    list->array = krealloc(list->array, new_size);

    list->capacity = new_size;
    if(list->length > list->capacity) {
        list->length = list->capacity;
    }
}

U8ArrayList * u8a_clone(U8ArrayList * arr) {
    U8ArrayList * newarr = u8a_create(arr->length);

    for (int i = 0; i < arr->length; i++) {
        newarr->array[newarr->length++] = arr->array[i];
    }

    return newarr;
}