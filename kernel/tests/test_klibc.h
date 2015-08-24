#include "tests.h"
#include "klibc.h"

#define NUM_TESTS 3

int test_os_memcmp_1() {
    char buffer1[] = "DWgaOtP12df0";
    char buffer2[] = "DWGAOTP12DF0";

    int i = os_memcmp(buffer1, buffer2, sizeof(buffer1));

    if (i > 0) {
        return TEST_OK;
    } else {
        return TEST_FAIL;
    }
}

int test_os_memcmp_2() {
    char buffer1[] = "DWgaOtP12df0";
    char buffer2[] = "DWgaOtP12df0";

    int i = os_memcmp(buffer1, buffer2, sizeof(buffer1));

    if (i == 0) {
        return TEST_OK;
    } else {
        return TEST_FAIL;
    }
}

int test_os_memcmp_3() {
    char buffer1[] = "DWGAOTP12DF0";
    char buffer2[] = "DWgaOtP12df0";

    int i = os_memcmp(buffer1, buffer2, sizeof(buffer1));

    if (i < 0) {
        return TEST_OK;
    } else {
        return TEST_FAIL;
    }
}

void run_klibc_tests() {
    Test *tests[NUM_TESTS];
    tests[0] = create_test("test_os_memcmp_1", &test_os_memcmp_1);
    tests[1] = create_test("test_os_memcmp_2", &test_os_memcmp_2);
    tests[2] = create_test("test_os_memcmp_3", &test_os_memcmp_3);

    run_tests(tests, NUM_TESTS);
}
