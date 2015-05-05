#include "../include/tests.h"
#include "../include/klibc.h"
#include "../include/tests/test_jump.h"
#include <global_defs.h>

#define NUM_TESTS 1

int test_umode_1() {
	int x = 1;
	while(x);
	// increment by the next instructions
	asm volatile("ADD R1, PC, #16":::);
	// load CPSR into R0
	asm volatile("MOV LR, R1":::);
	// load CPSR into R0
	asm volatile("MRS R0, CPSR":::);
	// clear mode field
	asm volatile("BIC R0, R0, #0x1F":::);
	// user mode code
	asm volatile("ORR R0, R0, #0x10":::);
	// store modified CPSR into SPSR
	asm volatile("MSR SPSR, R0":::);
	// context switch and branch
	asm volatile("MOVS PC, LR":::);

	return TEST_FAIL;
}

void run_umode_tests() {
	Test *tests[NUM_TESTS];
	tests[0] = create_test("test_umode_1", &test_umode_1);
	run_tests(tests, NUM_TESTS);
}
