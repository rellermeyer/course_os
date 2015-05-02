#include "../include/tests/test_process.h"
#include "../include/tests.h"
#include "../include/klibc.h"
#include "../include/vm.h"
#include "../include/process.h"
#include "../include/file.h"
#include <global_defs.h>

#define NUM_TESTS 1

int test_prcs_1() {

#define START 0x20000
#define PROC_LOCATION 0x9ff00000
#define FILE_LOCATION "/hello"

	struct stats fstats;
	uint32_t start = START + PROC_LOCATION;
	int fd = kopen(FILE_LOCATION, 'r');
	uint32_t len;

	get_stats(FILE_LOCATION, &fstats);

	len = fstats.size;

	for (int i = 0; i < (len / BLOCK_SIZE) + 1; i++) {
		uint32_t *v = (uint32_t*) (start + (i * BLOCK_SIZE));
		vm_allocate_page(KERNEL_VAS, (void*) v, VM_PERM_USER_RW);
	}

	int counter = 0;
	int* location = start;
	while (counter < len) {
		kread(fd, location, 4);
		location += 1;
		counter += 4;
	}

	pcb *test = process_create((uint32_t*) start);

	test->len = len;
	test->start = start;
	test->name = FILE_LOCATION;
	setup_process_vas(test);
	init_proc_stack(test);
	init_proc_heap(test);

	execute_process(test);

	return TEST_OK;
}

void run_prcs_tests() {
	Test *tests[NUM_TESTS];
	tests[0] = create_test("test_prcs_1", &test_prcs_1);
	run_tests(tests, NUM_TESTS);
}
