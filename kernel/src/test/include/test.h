
#ifndef TEST_H
#define TEST_H

#define TEST_PASS 1
#define TEST_FAIL 0

#include <stdio.h>
#include <allocator.h>
#include <mem_alloc.h>

// Only compile test definitions if enabled
#ifdef ENABLE_TESTS

#ifdef MEM_DEBUG
#define TEST_CREATE(name, block)                        \
    int __internal_test_##name() {                      \
        heap_t * heap = mem_get_allocator();            \
        isize_t nbytes = heap->bytes_allocated;         \
        block;                                          \
        if (nbytes != (isize_t)heap->bytes_allocated) { \
            kprintf(                                    \
                    "FAILED (MEMORY LEAK: %i bytes) \n",\
                    heap->bytes_allocated - nbytes      \
            );                                          \
            return TEST_FAIL;                           \
        }                                               \
        return TEST_PASS;                               \
    }                                                   \
    int test_##name() {                                 \
        kprintf("Testing %s\n", #name);                 \
        int res = __internal_test_##name();             \
        if (res == TEST_PASS) {                         \
            kprintf("PASSED\n");                        \
            return 1;                                   \
        } else {                                        \
            kprintf("FAILED\n");                        \
            return 0;                                   \
        }                                               \
    }
#else
#define TEST_CREATE(name, block)                \
    int __internal_test_##name() {              \
        block;                                  \
        return TEST_PASS;                       \
    }                                           \
    int test_##name() {                         \
        kprintf("Testing %s\n", #name);         \
        int res = __internal_test_##name();     \
        if (res == TEST_PASS) {                 \
            kprintf("PASSED\n");                \
            return 1;                           \
        } else {                                \
            kprintf("FAILED\n");                \
            return 0;                           \
        }                                       \
    }
#endif

#define PASS() return TEST_PASS
#define FAIL() return TEST_FAIL
#define ASSERT(expr) do { if(!expr) { kprintf("failed assertion: %s at ASSERT(%s)\n", __FILE__,  #expr); return TEST_FAIL; } } while(0)
#define ASSERT_EQ(l, r) do { if(l != r) { kprintf("failed assertion: %s at ASSERT_EQ(%s, %s)\n", __FILE__,  #l, #r);  return TEST_FAIL; } } while(0)
#define ASSERT_GT(l, r) do { if(l <= r) { kprintf("failed assertion: %s at ASSERT_GT(%s, %s)\n", __FILE__, #l, #r);  return TEST_FAIL; } } while(0)
#define ASSERT_GTEQ(l, r) do { if(l < r) { kprintf("failed assertion: %s at ASSERT_GTEQ(%s, %s)\n", __FILE__, #l, #r);  return TEST_FAIL; } } while(0)
#define ASSERT_LT(l, r) do { if(l >= r) { kprintf("failed assertion: %s at ASSERT_LT(%s, %s)\n", __FILE__, #l, #r);  return TEST_FAIL; } } while(0)
#define ASSERT_LTEQ(l, r) do { if(l > r) { kprintf("failed assertion: %s at ASSERT_LTEQ(%s, %s)\n", __FILE__, #l, #r);  return TEST_FAIL; } } while(0)
#define ASSERT_NEQ(l, r) do { if(l == r) { kprintf("failed assertion: %s at ASSERT_NEQ(%s, %s)\n", __FILE__, #l, #r); return TEST_FAIL; } } while(0)
#define ASSERT_NOT_NULL(e) do { if(e == NULL) { kprintf("failed assertion: %s\n at ASSERT_NOT_NULL(%s)", __FILE__, #e); return TEST_FAIL; } } while(0)

#else

// If tests are disabled during compile time, all the macros expand to nothing

#define TEST_CREATE(name, block)
#define PASS() 0
#define FAIL() 0
#define ASSERT(expr)  0
#define ASSERT_EQ(l, r) d0

#endif

void test_main();

#endif

