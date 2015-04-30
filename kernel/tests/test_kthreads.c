#include "../include/tests.h"
#include "../include/klibc.h"
#include "../include/tests/test_kthreads.h"
#include <global_defs.h>

#define NUM_TESTS 1

uint32_t handler() {
	LOG("hello!\n");
	return 0;
}

int test_kthr_1() {
	uint32_t tid = kthr_create(&handler);
	LOG("tid: %d\n", tid);
	return TEST_OK;
}

void run_kthr_tests() {
	Test *tests[NUM_TESTS];
	tests[0] = create_test("test_kthr_1", &test_kthr_1);
	run_tests(tests, NUM_TESTS);
}
