#include "testing_suite.h"
#include "test.h"
#include <stdio.h>

int test1() {
    return 1;
}

int test2() {
    return 2;
}

int test3() {
    return 3;
}

int test4() {
    return 4;
}

int test5() {
    return 5;
}

int main () {
    printf("STARTING\n");
    TEST test_1 = create_test("test_1", &test1);
    TEST test_2 = create_test("test_2", &test2);
    TEST test_3 = create_test("test_3", &test3);
    TEST test_4 = create_test("test_4", &test4);
    TEST test_5 = create_test("test_5", &test5);
    printf("CREATED TESTS\n");
    TEST_SUITE tests = create_test_suite();
    printf("CREATED SUITE\n");
    add_test(test_1, &tests);
    add_test(test_2, &tests);
    add_test(test_3, &tests);
    add_test(test_4, &tests);
    add_test(test_5, &tests);
    printf("ADDED TESTS\n");
    run_tests(tests);
    printf("DONE\n");
    return 0;
}
