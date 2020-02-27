#ifndef DS_H
#define DS_H

#include <stdbool.h>
#include <stdint.h>

// used across datastructures
typedef void (*FreeFunc)(void * data);
typedef bool (*CompareFunc)(void * in, void * other);
typedef uint32_t (*HashFunc)(void * key);

#include <vp_array_list.h>
#include <vp_singly_linked_list.h>
#include <HashMap.h>
#include <u8_array_list.h>
#include <qstr.h>

#endif