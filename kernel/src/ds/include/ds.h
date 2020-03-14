#ifndef DS_H
#define DS_H

#include <stdbool.h>
#include <stdint.h>

// used across datastructures
typedef void (*FreeFunc)(void * data);
typedef bool (*CompareFunc)(void * in, void * other);

#include <HashMap.h>
#include <qstr.h>
#include <u8_array_list.h>
#include <vp_array_list.h>
#include <vp_singly_linked_list.h>

#endif
