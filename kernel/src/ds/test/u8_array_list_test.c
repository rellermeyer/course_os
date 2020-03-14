#include <test.h>
#include <u8_array_list.h>


TEST_CREATE(u8a_push_get_pop_test, {
    uint32_t length = 30;

    U8ArrayList * arr = u8a_create(1);
    for (int i = 0; i < length; i++) { u8a_push(arr, i % 0xff); }

    ASSERT_EQ(arr->length, length);
    ASSERT_GTEQ(arr->capacity, length);

    for (int i = 0; i < length; i++) {
        uint8_t a = u8a_get(arr, i);
        ASSERT_EQ(a, i % 0xff);
    }

    ASSERT_EQ(arr->length, length);
    ASSERT_GTEQ(arr->capacity, length);

    for (int i = length - 1; i >= 0; i--) { ASSERT_EQ(u8a_pop(arr), i % 0xff); }

    ASSERT_EQ(arr->length, 0);
    ASSERT_LT(arr->capacity, length);

    u8a_free(arr);
})

TEST_CREATE(u8a_push_set_pop_test, {
    U8ArrayList * arr = u8a_create(1);
    for (int i = 0; i < 1000; i++) { u8a_push(arr, i % 0xff); }

    ASSERT_EQ(arr->length, 1000);
    ASSERT_GTEQ(arr->capacity, 1000);

    for (int i = 0; i < 1000; i++) { u8a_set(arr, i, 42); }

    ASSERT_EQ(arr->length, 1000);
    ASSERT_GTEQ(arr->capacity, 1000);

    for (int i = 999; i >= 0; i--) { ASSERT_EQ(u8a_pop(arr), 42); }

    ASSERT_EQ(arr->length, 0);
    ASSERT_LT(arr->capacity, 1000);

    u8a_free(arr);
})

TEST_CREATE(u8a_resize_test, {
    U8ArrayList * list = u8a_create(8);
    ASSERT_EQ(list->length, 0);
    ASSERT_EQ(list->capacity, 8);

    u8a_resize(list, 4);
    ASSERT_EQ(list->length, 0);
    ASSERT_EQ(list->capacity, 4);

    u8a_free(list);
})

TEST_CREATE(u8a_push_string_test, {
    U8ArrayList * list = u8a_create(1);

    size_t length = u8a_push_string(list, "TESTSTRING");

    ASSERT_EQ(length, list->length);
    ASSERT_EQ(length, 10);

    ASSERT_GTEQ(list->capacity, 10);

    ASSERT_EQ(u8a_get(list, 0), (uint8_t)'T');
    ASSERT_EQ(u8a_get(list, 1), (uint8_t)'E');
    ASSERT_EQ(u8a_get(list, 2), (uint8_t)'S');
    ASSERT_EQ(u8a_get(list, 3), (uint8_t)'T');
    ASSERT_EQ(u8a_get(list, 4), (uint8_t)'S');
    ASSERT_EQ(u8a_get(list, 5), (uint8_t)'T');
    ASSERT_EQ(u8a_get(list, 6), (uint8_t)'R');
    ASSERT_EQ(u8a_get(list, 7), (uint8_t)'I');
    ASSERT_EQ(u8a_get(list, 8), (uint8_t)'N');
    ASSERT_EQ(u8a_get(list, 9), (uint8_t)'G');

    u8a_free(list);
})

TEST_CREATE(u8a_clone_test, {
    U8ArrayList * list = u8a_create(1);
    u8a_push_string(list, "TESTSTRING");

    U8ArrayList * list2 = u8a_clone(list);


    ASSERT_EQ(list2->length, list->length);
    ASSERT_EQ(list2->length, 10);

    ASSERT_GTEQ(list2->capacity, 10);

    ASSERT_EQ(u8a_get(list2, 0), (uint8_t)'T');
    ASSERT_EQ(u8a_get(list2, 1), (uint8_t)'E');
    ASSERT_EQ(u8a_get(list2, 2), (uint8_t)'S');
    ASSERT_EQ(u8a_get(list2, 3), (uint8_t)'T');
    ASSERT_EQ(u8a_get(list2, 4), (uint8_t)'S');
    ASSERT_EQ(u8a_get(list2, 5), (uint8_t)'T');
    ASSERT_EQ(u8a_get(list2, 6), (uint8_t)'R');
    ASSERT_EQ(u8a_get(list2, 7), (uint8_t)'I');
    ASSERT_EQ(u8a_get(list2, 8), (uint8_t)'N');
    ASSERT_EQ(u8a_get(list2, 9), (uint8_t)'G');

    u8a_free(list);
    u8a_free(list2);
})
