#ifndef UNITS_TEST_H
#define UNITS_TEST_H
#include <stdlib.h>
#include <allocator.h>
#include <stdbool.h>
#include <ansi_escape.h>

__attribute__((__common__)) size_t global_counter;

#define DEBUG_COUNT kprintf("[DEBUG COUNT] %s:%i %i\n", __FILE__, __LINE__, global_counter++);

#define TEST_CREATE(name, block, ...) __VA_ARGS__

#define PASS() return TEST_PASS
#define FAIL() return TEST_FAIL
#endif
