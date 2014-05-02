#include "include/tests.h"
#include "include/process.h"
#include <stdint.h>
#include "include/global_defs.h"
#include "include/klibc.h"

uint32_t expected;
uint32_t actual;
uint32_t result;


/* Tests if a process is created */
uint32_t verify_process_creation() {
	uint32_t* addr1 = 0x8000; 
	pcb* p1 = process_create(addr1);

	expected = 1;
	actual = p1->PID;
	
	if(expected == actual)
		return 1;
	else
		return 0;	
}

/* Tests to see if a process is destroyed */ 
uint32_t verify_process_destroy() {
	uint32_t addr1 = 0x8000;
	process_destroy(1);

	expected = 0; 
	actual = get_PCB(1);

	if(expected == actual)
		return 1;
	else
		return 0;
}

/* Tests to see if a pcb structure is cleared */
uint32_t verify_free_PCB() {
	uint32_t addr1 = 0x8000;
	pcb* p = process_create(addr1);

	uint32_t tempPID = p->PID;

	free_PCB(p);

	expected = 0;
	actual = p->PID;

	if(expected == actual) {
		process_destroy(tempPID);
		return 1;
	}
	else
		return 0;
}

/* Call this function to kick off the tests */
uint32_t run_process_tests(void) {
	Test* tests[10];
	uint32_t (*func)(void);

	func = &verify_process_creation;
	char* test1 = "Process Test 1";
	Test* t1 = create_test(test1, func); 

	func = &verify_process_destroy;
	char* test2 = "Process Test 2";
	Test* t2 = create_test(test2, func);

	func = &verify_free_PCB;
	char* test3 = "Process Test 3";
	Test* t3 = create_test(test3, func);

	tests[0] = t1;
	tests[1] = t2;
	tests[2] = t3;

	run_tests(tests, 3);
	return 1;
}