#include <test.h>
#include <ds.h>


TEST_CREATE(test_create_ll, {
    VPSinglyLinkedList * lst = vpsll_create();

    uint32_t num = 10;

    for (uint32_t i = 0; i < num; i++) {
        vpsll_push(lst, (void *)i);
    }

    ASSERT_EQ(vpsll_length(lst), num);

    for (int32_t i = num - 1; i >= 0; i--) {
        ASSERT_EQ((int32_t)vpsll_pop(lst), i);
    }

    vpsll_free(lst, NULL);
})


TEST_CREATE(test_iter_ll, {
    VPSinglyLinkedList * lst = vpsll_create();

    uint32_t num = 10;

    for (uint32_t i = 0; i < num; i++) {
        vpsll_push(lst, (void *)i);
    }

    ASSERT_EQ(vpsll_length(lst), num);

    size_t index = num - 1;
    VPSLL_FOREACH(lst, i) {
        ASSERT_EQ((size_t)i->data, index--);
    }

    vpsll_free(lst, NULL);
})

TEST_CREATE(test_get_ll, {
    VPSinglyLinkedList * lst = vpsll_create();

    uint32_t num = 10;

    for (uint32_t i = 0; i < num; i++) {
        vpsll_push(lst, (void *)i);
    }

    ASSERT_EQ(vpsll_length(lst), num);

    isize_t index = 0;
    for (isize_t i = num - 1; i >= 0; i--)  {
        ASSERT_EQ((isize_t)vpsll_get(lst, index++), i);
    }

    vpsll_free(lst, NULL);
})

#ifdef ENABLE_TESTS
bool __compareInt(void * a, void * b) {
    // ^- me wishing for lambdas in C
    return *(int *)a == *(int *)b;
}
#endif

TEST_CREATE(test_contains_true, {
    VPSinglyLinkedList * lst = vpsll_create();

    int a = 3;
    int b = 5;
    int c = 7;

    int f = 5;

    vpsll_push(lst, &a);
    vpsll_push(lst, &b);
    vpsll_push(lst, &c);

    ASSERT(vpsll_contains(lst, &f, __compareInt));
    ASSERT_EQ(vpsll_length(lst), 3);

    vpsll_free(lst, NULL);
})

TEST_CREATE(test_contains_false, {
    VPSinglyLinkedList * lst = vpsll_create();

    int a = 3;
    int b = 5;
    int c = 7;

    int f = 9;

    vpsll_push(lst, &a);
    vpsll_push(lst, &b);
    vpsll_push(lst, &c);

    ASSERT(!vpsll_contains(lst, &f, __compareInt));
    ASSERT_EQ(vpsll_length(lst), 3);

    vpsll_free(lst, NULL);
})

TEST_CREATE(test_vpsll_remove, {
    VPSinglyLinkedList * lst = vpsll_create();

    int a = 3;
    int b = 5;
    int c = 7;

    int f = 5;

    vpsll_push(lst, &a);
    vpsll_push(lst, &b);
    vpsll_push(lst, &c);

    ASSERT_EQ(vpsll_remove(lst, &f, __compareInt), &b);
    ASSERT_EQ(vpsll_length(lst), 2);

    vpsll_free(lst, NULL);
})

TEST_CREATE(test_vpsll_set, {
    VPSinglyLinkedList * lst = vpsll_create();

    int a = 3;
    int b = 5;
    int c = 7;

    int f = 5;

    vpsll_push(lst, &a);
    vpsll_push(lst, &b);
    vpsll_push(lst, &c);

    vpsll_set(lst, 1, &f);
    ASSERT_EQ(vpsll_get(lst,1), &f);
    ASSERT_EQ(vpsll_length(lst), 3);

    vpsll_free(lst, NULL);
})
