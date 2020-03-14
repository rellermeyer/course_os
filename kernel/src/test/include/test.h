
#ifndef TEST_H
#define TEST_H

#define TEST_PASS 1
#define TEST_FAIL 0

#include <allocator.h>
#include <mem_alloc.h>
#include <stdio.h>

// Only compile test definitions if enabled
#ifdef ENABLE_TESTS

size_t global_counter;

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
    #define ASSERT(expr)                                                                       \
        do {                                                                                   \
            if (!expr) {                                                                       \
                kprintf("failed assertion: %s:%i at ASSERT(%s)\n", __FILE__, __LINE__, #expr); \
                return TEST_FAIL;                                                              \
            }                                                                                  \
        } while (0)
    #define ASSERT_EQ(l, r)                                                                        \
        do {                                                                                       \
            if (l != r) {                                                                          \
                kprintf(                                                                           \
                    "failed assertion: %s:%i at ASSERT_EQ(%s, %s)\n", __FILE__, __LINE__, #l, #r); \
                return TEST_FAIL;                                                                  \
            }                                                                                      \
        } while (0)
    #define ASSERT_GT(l, r)                                                                        \
        do {                                                                                       \
            if (l <= r) {                                                                          \
                kprintf(                                                                           \
                    "failed assertion: %s:%i at ASSERT_GT(%s, %s)\n", __FILE__, __LINE__, #l, #r); \
                return TEST_FAIL;                                                                  \
            }                                                                                      \
        } while (0)
    #define ASSERT_GTEQ(l, r)                                               \
        do {                                                                \
            if (l < r) {                                                    \
                kprintf("failed assertion: %s:%i at ASSERT_GTEQ(%s, %s)\n", \
                        __FILE__,                                           \
                        __LINE__,                                           \
                        #l,                                                 \
                        #r);                                                \
                return TEST_FAIL;                                           \
            }                                                               \
        } while (0)
    #define ASSERT_LT(l, r)                                                                        \
        do {                                                                                       \
            if (l >= r) {                                                                          \
                kprintf(                                                                           \
                    "failed assertion: %s:%i at ASSERT_LT(%s, %s)\n", __FILE__, __LINE__, #l, #r); \
                return TEST_FAIL;                                                                  \
            }                                                                                      \
        } while (0)
    #define ASSERT_LTEQ(l, r)                                               \
        do {                                                                \
            if (l > r) {                                                    \
                kprintf("failed assertion: %s:%i at ASSERT_LTEQ(%s, %s)\n", \
                        __FILE__,                                           \
                        __LINE__,                                           \
                        #l,                                                 \
                        #r);                                                \
                return TEST_FAIL;                                           \
            }                                                               \
        } while (0)
    #define ASSERT_NEQ(l, r)                                               \
        do {                                                               \
            if (l == r) {                                                  \
                kprintf("failed assertion: %s:%i at ASSERT_NEQ(%s, %s)\n", \
                        __FILE__,                                          \
                        __LINE__,                                          \
                        #l,                                                \
                        #r);                                               \
                return TEST_FAIL;                                          \
            }                                                              \
        } while (0)
    #define ASSERT_NOT_NULL(e)                                                                     \
        do {                                                                                       \
            if (e == NULL) {                                                                       \
                kprintf(                                                                           \
                    "failed assertion: %s:%i\n at ASSERT_NOT_NULL(%s)\n", __FILE__, __LINE__, #e); \
                return TEST_FAIL;                                                                  \
            }                                                                                      \
        } while (0)
    #define ASSERT_NULL(e)                                                                         \
        do {                                                                                       \
            if (e != NULL) {                                                                       \
                kprintf("failed assertion: %s:%i\n at ASSERT_NULL(%s)\n", __FILE__, __LINE__, #e); \
                return TEST_FAIL;                                                                  \
            }                                                                                      \
        } while (0)

#else

    // If tests are disabled during compile time, all the macros expand to nothing

    #define TEST_CREATE(name, block)
    #define PASS()          0
    #define FAIL()          0
    #define ASSERT(expr)    0
    #define ASSERT_EQ(l, r) d0

#endif

void test_main();

#endif
