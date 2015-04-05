#include "../include/testing_suite.h"
#include "../include/test.h"
#include "../include/klibc.h"

int test_os_memcmp_1() {
    char buffer1[] = "DWgaOtP12df0";
    char buffer2[] = "DWGAOTP12DF0";

    int i = os_memcmp(buffer1, buffer2, sizeof(buffer1));

    if (i > 0) {
        return 1;
    }
    else {
        return 0;
    }
}

int test_os_memcmp_2() {
    char buffer1[] = "DWgaOtP12df0";
    char buffer2[] = "DWgaOtP12df0";

    int i = os_memcmp(buffer1, buffer2, sizeof(buffer1));

    if (i == 0) {
        return 1;
    }
    else {
        return 0;
    }
}

int test_os_memcmp_3() {
    char buffer1[] = "DWGAOTP12DF0";
    char buffer2[] = "DWgaOtP12df0";

    int i = os_memcmp(buffer1, buffer2, sizeof(buffer1));

    if (i < 0) {
        return 1;
    }
    else {
        return 0;
    }
}

int main () {
    printf("STARTING\n");

    TEST test_memcmp_1 = create_test("test_os_memcmp_1", &test_os_memcmp_1);
    TEST test_memcmp_2 = create_test("test_os_memcmp_2", &test_os_memcmp_2);
    TEST test_memcmp_3 = create_test("test_os_memcmp_3", &test_os_memcmp_3);

    printf("CREATED TESTS\n");
    TEST_SUITE tests = create_test_suite();
    printf("CREATED SUITE\n");

    add_test(test_memcmp_1, &tests);
    add_test(test_memcmp_2, &tests);
    add_test(test_memcmp_3, &tests);

    printf("ADDED TESTS\n");
    run_tests(tests);
    printf("DONE\n");
    return 0;
}
