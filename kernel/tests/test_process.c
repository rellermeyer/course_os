#include "../include/tests/test_process.h"
#include "../include/tests.h"
#include "../include/process.h"
#include "../include/klibc.h"
#include <global_defs.h>

#define NUM_TESTS 1

int test_prcs_1() {
	DEBUG("Creating first process...\n");
	pcb *test = process_create_from_file("/hello", "whats up!");
	process_init(test);

	/*{
		os_printf("Testing kmalloc 2...\n");
		void *p = kmalloc(14);
		os_printf("p=%X\n",p);
		process_execute(test);
		}*/

	// DEBUG("Creating second process...\n");
	// pcb *test2 = process_create_from_file("/hello", "whats up!");
	// process_init(test2);
	process_execute(test);

	/*{
	os_printf("Testing kmalloc 1...\n");
	void *p = kmalloc(14);
	os_printf("p=%X\n",p);
	}*/

	return TEST_OK;
}

void run_prcs_tests() {
	Test *tests[NUM_TESTS];
	tests[0] = create_test("test_prcs_1", &test_prcs_1);
	run_tests(tests, NUM_TESTS);
}
