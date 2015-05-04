#include "../include/tests/test_sched_process.h"
#include "../include/tests.h"
#include "../include/scheduler.h"
#include <global_defs.h>

#define NUM_TESTS 1

int test_sched_prcs_1() {

#define PRIORITY -5

	char ** argv_1 = { "PROCESS_ARG: Process 11\n", "How are you?\n" };

	if(!sched_start_task(sched_create_task_from_process("/hello", PRIORITY, argv_1, 2))){
		return STATUS_FAIL;
	}

	char ** argv_2 = { "PROCESS_ARG: Process 12\n", "I'm doing well!\n" };

	if(!sched_start_task(sched_create_task_from_process("/hello", PRIORITY, argv_2, 2))){
		return STATUS_FAIL;
	}

	return TEST_OK;
}

void run_sched_prcs_tests() {
	Test *tests[NUM_TESTS];
	tests[0] = create_test("test_sched_prcs_1", &test_sched_prcs_1);
	run_tests(tests, NUM_TESTS);
}
