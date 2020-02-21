
#ifndef TEST_H
#define TEST_H

#define TEST_PASS 1
#define TEST_FAIL 0

#include <stdio.h>

// Only compile test definitions if enabled
#if ENABLE_TESTS

#define TEST_CREATE(name, block)            \
    int __internal_test_##name() block      \
    int test_##name() {                     \
        os_printf("Testing %s\n", #name);   \
        int res = __internal_test_##name(); \
        if (res == TEST_PASS) {             \
            os_printf("PASSED\n");          \
            return 1;                       \
        } else {                            \
            os_printf("FAILED\n");          \
            return 0;                       \
        }                                   \
    }

#define PASS() return TEST_PASS
#define FAIL() return TEST_FAIL
#define ASSERT(expr) do { if(!expr) { return TEST_FAIL } } while(0)
#define ASSERT_EQ(l, r) do { if(l != r) { return TEST_FAIL } } while(0)


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

