#include <test.h>
#include <vp_array_list.h>
#include <stdio.h>

void freefunc(void * data) {
    if ((*(int *)data != 42)) {
        panic();
    }
}

int data[] = {1, 2, 3, 4, 5, 6, 7, 8};

TEST_CREATE(vpa_push_get_pop_test, {
    uint32_t length = 30;

    VPArrayList * arr = vpa_create(1);
    for (int i = 0; i < length; i++){
        vpa_push(arr, &data[i % 8]);
    }

    ASSERT_EQ(arr->length, length);
    ASSERT_GTEQ(arr->capacity, length);

    for (int i = 0; i < length; i++){
        int * a = vpa_get(arr, i);

        ASSERT_EQ(*a, data[i % 8]);
    }

    ASSERT_EQ(arr->length, length);
    ASSERT_GTEQ(arr->capacity, length);

    for (int i = length - 1; i >= 0; i--){
        ASSERT_EQ(*(int *)vpa_pop(arr), data[i % 8]);
    }

    ASSERT_EQ(arr->length, 0);
    ASSERT_LT(arr->capacity, length);

    vpa_free(arr, NULL);
})

TEST_CREATE(vpa_push_set_pop_test, {
    VPArrayList * arr = vpa_create(1);
    for (int i = 0; i < 1000; i++){
        vpa_push(arr, &data[i % 8]);
    }

    ASSERT_EQ(arr->length, 1000);
    ASSERT_GTEQ(arr->capacity, 1000);

    int intdata = 42;

    for (int i = 0; i < 1000; i++){
        vpa_set(arr, i, &intdata);
    }

    ASSERT_EQ(arr->length, 1000);
    ASSERT_GTEQ(arr->capacity, 1000);

    vpa_free(arr, freefunc);
})

TEST_CREATE(vpa_resize_test, {
    VPArrayList * list = vpa_create(8);
    ASSERT_EQ(list->length,0);
    ASSERT_EQ(list->capacity, 8);

    vpa_resize(list, 4, NULL);
    ASSERT_EQ(list->length,0);
    ASSERT_EQ(list->capacity, 4);

    vpa_free(list, NULL);
})