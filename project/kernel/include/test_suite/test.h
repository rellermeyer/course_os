
#ifndef TEST_H
#define TEST_H

#include <allocator.h>
#include <mem_alloc.h>
#include <stdbool.h>
#include <stdio.h>

#define TEST_PASS true
#define TEST_FAIL false

// Only compile test definitions if enabled
#ifdef ENABLE_TESTS
    #include "units.h"
    #include "assert.h"
#else

    // If tests are disabled during compile time, all the macros expand to nothing
    #define TEST_CREATE(name, block)
    #define PASS()          0
    #define FAIL()          0
    #define ASSERT(expr)    0
    #define ASSERT_EQ(l, r) 0

#endif

void test_main();

#endif
