
#ifndef TEST_H
#define TEST_H

#define TEST_PASS 1
#define TEST_FAIL 0

#include <allocator.h>
#include <mem_alloc.h>
#include <stdio.h>

// Only compile test definitions if enabled
#ifdef ENABLE_TESTS

__attribute__((__common__)) size_t global_counter;

    #define DEBUG_COUNT kprintf("[DEBUG COUNT] %s:%i %i\n", __FILE__, __LINE__, global_counter++);

    #ifdef MEM_DEBUG
        #define TEST_CREATE(name, block)                                         \
            int __internal_test_##name() {                                       \
                heap_t * heap = mem_get_allocator();                             \
                isize_t nbytes = heap->bytes_allocated;                          \
                block;                                                           \
                if (nbytes != (isize_t)heap->bytes_allocated) {                  \
                    kprintf("\e[38;5;160mFAILED (MEMORY LEAK: %i bytes)\e[0m\n", \
                            heap->bytes_allocated - nbytes);                     \
                    return TEST_FAIL;                                            \
                }                                                                \
                return TEST_PASS;                                                \
            }                                                                    \
            int test_##name() {                                                  \
                kprintf("\e[38;5;163m[TEST]\e[0m %s\n", #name);                  \
                int res = __internal_test_##name();                              \
                if (res == TEST_PASS) {                                          \
                    kprintf("└─\e[92mPASSED\e[0m\n", #name);                     \
                    return 1;                                                    \
                } else {                                                         \
                    kprintf("└─\e[38;5;160mFAILED\e[0m\n", #name);               \
                    return 0;                                                    \
                }                                                                \
            }
    #else
        #define TEST_CREATE(name, block)                        \
            int __internal_test_##name() {                      \
                block;                                          \
                return TEST_PASS;                               \
            }                                                   \
            int test_##name() {                                 \
                kprintf("\e[38;5;163m[TEST]\e[0m %s\n", #name); \
                int res = __internal_test_##name();             \
                if (res == TEST_PASS) {                         \
                    kprintf("└─\e[92mPASSED\e[0m\n");           \
                    return 1;                                   \
                } else {                                        \
                    kprintf("└─\e[38;5;160mFAILED\e[0m\n");     \
                    return 0;                                   \
                }                                               \
            }
    #endif

    #define PASS() return TEST_PASS
    #define FAIL() return TEST_FAIL
    #include "assert.h"
#else

    // If tests are disabled during compile time, all the macros expand to nothing

    #define TEST_CREATE(name, block)
    #define PASS()          0
    #define FAIL()          0
    #define ASSERT(expr)    0
    /* #define ASSERT_EQ(l, r) 0 */

#endif

void test_main();

#endif
