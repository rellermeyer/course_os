#include "../include/tests/test_sched_process.h"
#include "../include/tests.h"
#include "../include/scheduler.h"
#include <global_defs.h>

#define NUM_TESTS 1

int test_sched_prcs_1() {
	sched_start_task(sched_create_task_from_process("/hello", -5));
	sched_start_task(sched_create_task_from_process("/hello", -5));
//	sched_start_task(sched_create_task_from_process("/hello", -3));
//	sched_start_task(sched_create_task_from_process("/hello", -2));
	sched_yield();

	return TEST_OK;
}

void run_sched_prcs_tests() {
	Test *tests[NUM_TESTS];
	tests[0] = create_test("test_sched_prcs_1", &test_sched_prcs_1);
	run_tests(tests, NUM_TESTS);
}
