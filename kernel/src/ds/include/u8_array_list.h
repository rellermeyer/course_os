#ifndef U8ARRAY_LIST_H
#define U8ARRAY_LIST_H

#include <stdint.h>

typedef struct U8ArrayList {
    uint32_t length;
    uint32_t capacity;
    uint8_t * array;
} U8ArrayList;

U8ArrayList * u8a_create(uint32_t initial_cap);

// Frees the list.
void u8a_free(U8ArrayList * arr);

// Gets element of specified index;
uint8_t u8a_get(U8ArrayList * list, uint32_t index);

// Sets element to specific data
void u8a_set(U8ArrayList * list, uint32_t index, uint8_t data);

// Inserts data at the end of the list
uint32_t u8a_push(U8ArrayList * list, uint8_t data);

// Returns the topmost element and removes it fromhe list.
uint8_t u8a_pop(U8ArrayList * list);

// Truncates the list to specified size
void u8a_resize(U8ArrayList * list, uint32_t new_size);

// appends a null terminated string of data to the end of the list.
// Does not copy the null terminator as this can be found using the array length.
uint32_t u8a_push_string(U8ArrayList * arr, char * data);

// Creates a new u8a with the same contents as another one.
U8ArrayList * u8a_clone(U8ArrayList * arr);

#endif //U8ARRAY_LIST_H
