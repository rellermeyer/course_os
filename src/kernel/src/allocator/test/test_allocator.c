#include <stdlib.h>
#include <test.h>

TEST_CREATE(test_alloc_free, {
    uint32_t * a = kmalloc(sizeof(uint32_t));
    *a = 42;
    kfree(a);
})

TEST_CREATE(test_alloc_free_large, {
    uint32_t * a = kmalloc(100 * sizeof(uint32_t));
    for (int i = 0; i < 100; i++) { a[i] = 42; }
    kfree(a);
})

TEST_CREATE(test_alloc_realloc_free, {
    uint32_t * a = kmalloc(5 * sizeof(uint32_t));
    for (int i = 0; i < 5; i++) { a[i] = 42; }

    a = krealloc(a, 100 * sizeof(uint32_t));
    for (int i = 5; i < 100; i++) { a[i] = 42; }

    kfree(a);
})

TEST_CREATE(test_free_null, { kfree(NULL); })

TEST_CREATE(test_calloc, {
    int * test = kcalloc(100, sizeof(int));

    for (int i = 0; i < 100; i++) { ASSERT_EQ(test[i], 0); }

    kfree(test);
})

TEST_CREATE(test_expand_heap, {
    uint32_t initial_end = heap->end;

    expand(heap);

    ASSERT_EQ(heap->end, initial_end + 0x1000);

    uint8_t * yolo = (void *)(heap->end - 300);
    uint8_t prev = *yolo;
    *yolo = 42;
    ASSERT_EQ(*yolo, 42);
    *yolo = prev;

    contract(heap);

    ASSERT_EQ(heap->end, initial_end);
})
