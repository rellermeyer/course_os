#include "../include/tests.h"
#include "../include/tests/test_array_list.h"
#include "../include/array_list.h"
#include "klibc.h"

#define NUM_TESTS 5

#define new(a) ((a*) kmalloc(sizeof(a)))

//This is where you define the tests you want to run. They return 1 on success and 0 on failure.

//Tests the create function
int test_arrl_1() {
    arrl_handle* arrl1;
    arrl_handle* arrl2;

    arrl1 = arrl_create();
    if (!arrl1) {
        os_printf("expected value");
        arrl_free(arrl1);
        return TEST_FAIL;
    }

    arrl2 = arrl_create_fixed(3);
    if (!arrl2) {
        os_printf("expected value");
        arrl_free(arrl2);
        return TEST_FAIL;
    }

    //arrl_free(arrl1);
    //arrl_free(arrl2);
    return TEST_OK;
}

// Test put and get functions
int test_arrl_2() {
    arrl_handle* arrl;
    arrl = arrl_create();

    int* data1;
    int* data2;
    int* data3;

    data1 = new(int);
    *data1 = 0;
    arrl_append(arrl, data1);

    data2 = new(int);
    *data2 = 1;
    arrl_append(arrl, data2);

    data3 = new(int);
    *data3 = 2;
    arrl_append(arrl, data3);

    if (arrl_get(arrl, 0) != data1 || arrl_get(arrl, 1) != data2
            || arrl_get(arrl, 2) != data3) {
        kfree(data1);
        kfree(data2);
        kfree(data3);
        arrl_free(arrl);
        return TEST_FAIL;
    }

    kfree(data1);
    kfree(data2);
    kfree(data3);
    //arrl_free(arrl);
    return TEST_OK;
}

// Test remove function
int test_arrl_3() {
    arrl_handle* arrl;
    arrl = arrl_create();

    int* data1;
    int* data2;
    int* data3;

    data1 = new(int);
    *data1 = 0;
    arrl_append(arrl, data1);

    data2 = new(int);
    *data2 = 1;
    arrl_append(arrl, data2);

    data3 = new(int);
    *data3 = 2;
    arrl_append(arrl, data3);

    int* tmp1 = arrl_remove(arrl, data1);
    int* tmp2 = arrl_remove(arrl, data2);
    int* tmp3 = arrl_remove(arrl, data3);

    if (tmp1 != data1 || tmp2 != data2 || tmp3 != data3
            || arrl_count(arrl) != 0) {
        kfree(tmp1);
        kfree(tmp2);
        kfree(tmp3);
        arrl_free(arrl);
        return TEST_FAIL;
    }

    kfree(tmp1);
    kfree(tmp2);
    kfree(tmp3);
    arrl_free(arrl);
    return TEST_OK;
}

// Test contains and index_of functions
int test_arrl_4() {
    arrl_handle* arrl;
        arrl = arrl_create();

        int* data1;
        int* data2;
        int* data3;
        int* data4;

        data1 = new(int);
        *data1 = 0;
        arrl_append(arrl, data1);

        if (!arrl_contains(arrl, data1) || arrl_index_of(arrl, data1) != 0) {
            kfree(data1);
            arrl_free(arrl);
            return TEST_FAIL;
        }

        data2 = new(int);
        *data2 = 1;
        arrl_append(arrl, data2);

        if (!arrl_contains(arrl, data2) || arrl_index_of(arrl, data2) != 1) {
            kfree(data1);
            kfree(data2);
            arrl_free(arrl);
            return TEST_FAIL;
        }

        data3 = new(int);
        *data3 = 2;
        arrl_append(arrl, data3);

        if (!arrl_contains(arrl, data3) || arrl_index_of(arrl, data3) != 2) {
            kfree(data1);
            kfree(data2);
            kfree(data3);
            arrl_free(arrl);
            return TEST_FAIL;
        }

        kfree(arrl_remove(arrl, data2));

        if (arrl_contains(arrl, data1) || arrl_index_of(arrl, data1) != -1) {
            kfree(data1);
                        kfree(data3);
                        arrl_free(arrl);
                        return TEST_FAIL;
        }

        if (!arrl_contains(arrl, data3) || arrl_index_of(arrl, data3) != 1) {
            kfree(data1);
            kfree(data3);
            arrl_free(arrl);
            return TEST_FAIL;
        }

        data4 = new(int);
        *data4 = 3;
        arrl_append(arrl, data4);

        if (!arrl_contains(arrl, data3) || arrl_index_of(arrl, data3) != 2) {
            kfree(data1);
            kfree(data3);
            kfree(data4);
            arrl_free(arrl);
            return TEST_FAIL;
        }

        kfree(data1);
        kfree(data3);
        kfree(data4);
        arrl_free(arrl);
        return TEST_OK;
}

// Test count function
int test_arrl_5() {
    arrl_handle* arrl;
    arrl = arrl_create();

    int* data1;
    int* data2;
    int* data3;
    int* data4;

    data1 = new(int);
    *data1 = 0;
    arrl_append(arrl, data1);

    if (arrl_count(arrl) != 1) {
        kfree(data1);
        arrl_free(arrl);
        return TEST_FAIL;
    }

    data2 = new(int);
    *data2 = 1;
    arrl_append(arrl, data2);

    if (arrl_count(arrl) != 2) {
        kfree(data1);
        kfree(data2);
        arrl_free(arrl);
        return TEST_FAIL;
    }

    data3 = new(int);
    *data3 = 2;
    arrl_append(arrl, data3);

    if (arrl_count(arrl) != 3) {
        kfree(data1);
        kfree(data2);
        kfree(data3);
        arrl_free(arrl);
        return TEST_FAIL;
    }

    kfree(arrl_remove(arrl, data2));

    if (arrl_count(arrl) != 2) {
        kfree(data1);
        kfree(data3);
        arrl_free(arrl);
        return TEST_FAIL;
    }

    data4 = new(int);
    *data4 = 3;
    arrl_append(arrl, data4);

    if (arrl_count(arrl) != 3) {
        kfree(data1);
        kfree(data3);
        kfree(data4);
        arrl_free(arrl);
        return TEST_FAIL;
    }

    kfree(data1);
    kfree(data3);
    kfree(data4);
    arrl_free(arrl);
    return TEST_OK;
}

//function running tests
void run_arrl_tests() {
    Test *tests[NUM_TESTS];
    tests[0] = create_test("test_arrl_1", &test_arrl_1);
    tests[1] = create_test("test_arrl_2", &test_arrl_2);
    tests[2] = create_test("test_arrl_3", &test_arrl_3);
    tests[3] = create_test("test_arrl_4", &test_arrl_4);
    tests[4] = create_test("test_arrl_5", &test_arrl_5);

    run_tests(tests, NUM_TESTS);
}

