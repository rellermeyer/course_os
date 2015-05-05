#include "../include/tests/test_sched_process.h"
#include "../include/tests.h"
#include "../include/scheduler.h"
#include <global_defs.h>

#define NUM_TESTS 1

int test_sched_prcs_1() {

#define PRIORITY -5

	char * argv_1[2];
	argv_1[0] = "Process 11";
	argv_1[1] = "How are you?";

	sched_start_task(sched_create_task_from_process("/hello", PRIORITY, argv_1, 2));
	sched_start_task(sched_create_task_from_process("/hello", PRIORITY, argv_1, 2));

	sched_yield(0);

	return TEST_OK;
}

void run_sched_prcs_tests() {
	Test *tests[NUM_TESTS];
	tests[0] = create_test("test_sched_prcs_1", &test_sched_prcs_1);
	run_tests(tests, NUM_TESTS);
}
