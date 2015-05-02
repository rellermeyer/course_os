#include "../include/tests.h"
#include "../include/klibc.h"
#include "../include/scheduler.h"
#include "../include/tests/test_kthreads.h"
#include <global_defs.h>

#define NUM_TESTS 1

uint32_t handler3(uint32_t pid) {
	INFO("handler3\n");
	return 0;
}
uint32_t handler2(uint32_t pid) {
	INFO("handler2-start\n");
	kthr_start(kthr_create(&handler3));
	sched_yield();
	INFO("handler2-mid\n");
	kthr_start(kthr_create(&handler3));
	sched_yield();
	INFO("handler2-end\n");
	return 0;
}
uint32_t handler1(uint32_t pid) {
	INFO("handler1-start\n");
	kthr_start(kthr_create(&handler2));
	sched_yield();
	INFO("handler1-mid\n");
	kthr_start(kthr_create(&handler3));
	sched_yield();
	INFO("handler1-end\n");
	return 0;
}

uint32_t handler0(uint32_t pid) {
	INFO("handler0-start\n");
	kthr_start(kthr_create(&handler1));
	sched_yield();
	INFO("handler0-mid-1\n");
	kthr_start(kthr_create(&handler3));
	sched_yield();
	INFO("handler0-mid-2\n");
	sched_yield();
	INFO("handler0-mid-3\n");
	sched_yield();
	INFO("handler0-end-4\n");
	return 0;
}

uint32_t handler0_b(uint32_t pid) { // 12
	INFO("handler1\n");
	return 0;
}

uint32_t handler0_a(uint32_t pid) { // 11
	INFO("handler0-start\n");
	kthr_start(kthr_create(&handler0_b));
	sched_yield();
	INFO("handler0-end\n");
	return 0;
}

int test_kthr_1() {
	kthr_start(kthr_create(&handler0_a));
	sched_yield();
	return TEST_OK;
}

void run_kthr_tests() {
	Test *tests[NUM_TESTS];
	tests[0] = create_test("test_kthr_1", &test_kthr_1);
	run_tests(tests, NUM_TESTS);
}
