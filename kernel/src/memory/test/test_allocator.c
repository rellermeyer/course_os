#include <test.h>
#include <stdlib.h>

TEST_CREATE(test_alloc_free, {
    uint32_t * a = kmalloc(sizeof(uint32_t));
    *a = 42;
    kfree(a);
})

TEST_CREATE(test_alloc_free_large, {
    uint32_t * a = kmalloc(100 * sizeof(uint32_t));
    for (int i = 0; i < 100; i++) {
        a[i] = 42;
    }
    kfree(a);
})

TEST_CREATE(test_alloc_realloc_free, {
    uint32_t * a = kmalloc(5 * sizeof(uint32_t));
    for (int i = 0; i < 5; i++) {
        a[i] = 42;
    }

    a = krealloc(a, 100 * sizeof(uint32_t));
    for (int i = 5; i < 100; i++) {
        a[i] = 42;
    }

    kfree(a);
})