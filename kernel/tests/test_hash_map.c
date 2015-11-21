#include "tests.h"
#include "tests/test_hash_map.h"
#include "data_structures/hash_map.h"
#include "klibc.h"

#define NUM_TESTS 4

#define new(a) ((a*) kmalloc(sizeof(a)))

//This is where you define the tests you want to run. They return 1 on success and 0 on failure.

//Tests the create function
int test_hmap_1() {
    hmap_handle* hmap1 = hmap_create();
    hmap_handle* hmap2 = hmap_create_fixed(3);

    if (!hmap1) {
        os_printf("expected value");
        return TEST_FAIL;
    }

    if (!hmap2) {
        os_printf("expected value");
        return TEST_FAIL;
    }

    return TEST_OK;
}

// Test put and get functions
int test_hmap_2() {
    hmap_handle* hmap;
    hmap = hmap_create();

    int* data1;
    int* data2;
    int* data3;

    data1 = new(int);
    *data1 = 0;
    hmap_put(hmap, (long) (*data1), data1);

    data2 = new(int);
    *data2 = 1;
    hmap_put(hmap, (long) (*data2), data2);

    data3 = new(int);
    *data3 = 2;
    hmap_put(hmap, (long) (*data3), data3);

    if (hmap_get(hmap, (long) (*data1)) != data1
            || hmap_get(hmap, (long) (*data2)) != data2
            || hmap_get(hmap, (long) (*data3)) != data3) {
        kfree(data1);
        kfree(data2);
        kfree(data3);
        hmap_free(hmap);
        return TEST_FAIL;
    }

    return TEST_OK;
}

// Test remove function
int test_hmap_3() {
    hmap_handle* hmap;
    hmap = hmap_create();

    int* data1;
    int* data2;
    int* data3;

    data1 = new(int);
    *data1 = 0;
    hmap_put(hmap, (long) (*data1), data1);

    data2 = new(int);
    *data2 = 1;
    hmap_put(hmap, (long) (*data2), data2);

    data3 = new(int);
    *data3 = 2;
    hmap_put(hmap, (long) (*data3), data3);

    int* tmp1 = hmap_remove(hmap, (long) (*data1));
    int* tmp2 = hmap_remove(hmap, (long) (*data2));
    int* tmp3 = hmap_remove(hmap, (long) (*data3));

    if (tmp1 != data1 || tmp2 != data2 || tmp3 != data3
            || hmap_count(hmap) != 0) {
        kfree(tmp1);
        kfree(tmp2);
        kfree(tmp3);
        hmap_free(hmap);
        return TEST_FAIL;
    }

    kfree(tmp1);
    kfree(tmp2);
    kfree(tmp3);
    hmap_free(hmap);
    return TEST_OK;
}

// Test count function
int test_hmap_4() {
    hmap_handle* hmap;
    hmap = hmap_create();

    int* data1;
    int* data2;
    int* data3;
    int* data4;

    data1 = new(int);
    *data1 = 0;
    hmap_put(hmap, (long) (*data1), data1);

    if (hmap_count(hmap) != 1) {
        kfree(data1);
        hmap_free(hmap);
        return TEST_FAIL;
    }

    data2 = new(int);
    *data2 = 1;
    hmap_put(hmap, (long) (*data2), data2);

    if (hmap_count(hmap) != 2) {
        kfree(data1);
        kfree(data2);
        hmap_free(hmap);
        return TEST_FAIL;
    }

    data3 = new(int);
    *data3 = 2;
    hmap_put(hmap, (long) (*data3), data3);

    if (hmap_count(hmap) != 3) {
        kfree(data1);
        kfree(data2);
        kfree(data3);
        hmap_free(hmap);
        return TEST_FAIL;
    }

    kfree(hmap_remove(hmap, (long) (*data2)));

    if (hmap_count(hmap) != 2) {
        kfree(data1);
        kfree(data3);
        hmap_free(hmap);
        return TEST_FAIL;
    }

    data4 = new(int);
    *data4 = 3;
    hmap_put(hmap, (long) (*data4), data4);

    if (hmap_count(hmap) != 3) {
        kfree(data1);
        kfree(data3);
        kfree(data4);
        hmap_free(hmap);
        return TEST_FAIL;
    }

    kfree(data1);
    kfree(data2);
    kfree(data3);
    kfree(data4);
    hmap_free(hmap);
    return TEST_OK;
}

//function running tests
void run_hmap_tests() {
    Test *tests[NUM_TESTS];
    tests[0] = create_test("test_hmap_1", &test_hmap_1);
    tests[1] = create_test("test_hmap_2", &test_hmap_2);
    tests[2] = create_test("test_hmap_3", &test_hmap_3);
    tests[3] = create_test("test_hmap_4", &test_hmap_4);

    run_tests(tests, NUM_TESTS);
}

