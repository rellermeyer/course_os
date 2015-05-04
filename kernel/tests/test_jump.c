#include "../include/tests.h"
#include "../include/klibc.h"
#include "../include/tests/test_jump.h"
#include <global_defs.h>

#define NUM_TESTS 1

int test_jump_1() {

	jmp_buf buf;

	/*int x = 1;
	while(x);*/

	int retval = jmp_set(&buf);
	os_printf("retval=%d\n",retval);
	if(retval == 5){
		return TEST_OK;
	}

	jmp_goto(&buf, 5);

	return TEST_FAIL;
}

void run_jump_tests() {
	Test *tests[NUM_TESTS];
	tests[0] = create_test("test_jump_1", &test_jump_1);
	run_tests(tests, NUM_TESTS);
}
