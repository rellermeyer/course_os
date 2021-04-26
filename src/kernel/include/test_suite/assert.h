#ifndef ASSERT_TEST_H
#define ASSERT_TEST_H
#include <string.h>
#include <ansi_escape.h>

#define _ASSERT_INTERNAL(name, expr)                                    \
    do {                                                                \
        if (!expr) {                                                    \
            ANSI_256(160);                                              \
            kprintf("├─failed assertion: %s:%i at %s(%s)\n", __FILE__, __LINE__, name, #expr); \
            ANSI(RESET);                                                \
            return TEST_FAIL;                                           \
        }                                                               \
    } while (0)

#define ASSERT(e) _ASSERT_INTERNAL("ASSERT", e)
#define ASSERT_EQ(l, r) _ASSERT_INTERNAL("ASSERT_EQ", ((l) == (r)))
#define ASSERT_GT(l, r) _ASSERT_INTERNAL("ASSERT_GT", ((l) > (r)))
#define ASSERT_GTEQ(l, r) _ASSERT_INTERNAL("ASSERT_GTEQ", ((l) >= (r)))
#define ASSERT_LT(l, r) _ASSERT_INTERNAL("ASSERT_LT", ((l) < (r)))
#define ASSERT_LTEQ(l, r) _ASSERT_INTERNAL("ASSERT_LTEQ", ((l) <= (r)))
#define ASSERT_NEQ(l, r) _ASSERT_INTERNAL("ASSERT_NEQ", ((l) != (r)))

#define ASSERT_STRNCMP(l, s) _ASSERT_INTERNAL("ASSERT_STRNCMP", (strcmp((l), (s)) != 0))
// Unneeded since there is a nicer function, but provides for some API symmetry
#define ASSERT_STRCMP(l, s) _ASSERT_INTERNAL("ASSERT_STRNCMP", (strcmp((l), (s)) == 0))

// Checking whether something is equal to NULL is the same as checking
// the expression itself, but it avoids having to do a tricky
// conversion. if (e) => if (e != NULL), if (!e) => (e == NULL)
#define ASSERT_NOT_NULL(e) _ASSERT_INTERNAL("ASSERT_NOT_NULL", (e))
#define ASSERT_NULL(e) _ASSERT_INTERNAL("ASSERT_NULL", (!e))


// Semityped assertions
#define _ASSERT_TYPED(name, expr, print, l, r)                          \
    do {                                                                \
        if (!expr) {                                                    \
            ANSI_256(160);                                              \
            kprintf("├─failed assertion: %s:%i at %s(%s)\n", __FILE__, __LINE__, name, #expr); \
            ANSI(RESET);                                                \
            kprintf("├─expected: " print "\n", l, r);                     \
            return TEST_FAIL;                                           \
        }                                                               \
    } while (0)

// TODO: Implement kprintf handlers for float, double, and longs
#define ASSERT_DBL_EQ(l, r) _ASSERT_TYPED("ASSERT_DBL_EQ", ((l) == (r)), "%f got %f", l, r)
#define ASSERT_FLT_EQ(l, r) _ASSERT_TYPED("ASSERT_FLT_EQ", ((l) == (r)), "%f got %f", l, r)
#define ASSERT_CHR_EQ(l, r) _ASSERT_TYPED("ASSERT_CHR_EQ", ((l) == (r)), "%c got %c", l, r)
#define ASSERT_PTR_EQ(l, r) _ASSERT_TYPED("ASSERT_PTR_EQ", ((l) == (r)), "%p got %p", l, r)
#define ASSERT_INT_EQ(l, r) _ASSERT_TYPED("ASSERT_INT_EQ", ((l) == (r)), "%d got %d", l, r)
#define ASSERT_LONG_EQ(l, r) _ASSERT_TYPED("ASSERT_LONG_EQ",( (l) == (r)), "%ld got %ld", l, r)
#define ASSERT_STR_EQ(l, s) _ASSERT_TYPED("ASSERT_STR_EQ", (strcmp((l), (s)) == 0), \
                                          "'%s' got '%s'", l, s)
#endif
