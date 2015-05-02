#include "../include/tests.h"
#include "../include/klibc.h"
#include "../include/scheduler.h"
#include "../include/tests/test_kthreads.h"
#include <global_defs.h>

#define NUM_TESTS 1

//uint32_t handler3(uint32_t pid) {
//	INFO("handler3\n");
//	return 0;
//}
//uint32_t handler2(uint32_t pid) {
//	INFO("handler2-start\n");
//	kthr_start(kthr_create(&handler3));
//	kthr_yield();
//	INFO("handler2-mid\n");
//	kthr_start(kthr_create(&handler3));
//	kthr_yield();
//	INFO("handler2-end\n");
//	return 0;
//}
//uint32_t handler1(uint32_t pid) {
//	INFO("handler1-start\n");
//	kthr_start(kthr_create(&handler2));
//	kthr_yield();
//	INFO("handler1-mid\n");
//	kthr_start(kthr_create(&handler3));
//	kthr_yield();
//	INFO("handler1-end\n");
//	return 0;
//}
//
//uint32_t handler0(uint32_t pid) {
//	INFO("handler0-start\n");
//	kthr_start(kthr_create(&handler1));
//	kthr_yield();
//	INFO("handler0-mid-1\n");
//	kthr_start(kthr_create(&handler3));
//	kthr_yield();
//	INFO("handler0-mid-2\n");
//	kthr_yield();
//	INFO("handler0-mid-3\n");
//	kthr_yield();
//	INFO("handler0-end-4\n");
//	return 0;
//}

int msg_received;
uint32_t handler0_b(uint32_t parent_tid, uint32_t tid) { // 12
	char * msg = "whatsup!!!\0";
	uint32_t msg_len = 11;
	uint32_t event_id = 1;
	if(sched_get_message_space() >= msg_len){
		if(sched_post_message(parent_tid, event_id, msg, msg_len)){
			ERROR("Message fail\n");
		}
	}

	return 0;
}

uint32_t handler0_a_message_handler(uint32_t src_tid, uint32_t event, char * data, int length){
	DEBUG("Message received from %d - %s\n", src_tid, data);
	msg_received++;
}

uint32_t handler0_a(uint32_t parent_tid, uint32_t tid) { // 11
	sched_register_callback_handler(&handler0_a_message_handler);
	kthr_start(kthr_create(&handler0_b));
	kthr_yield();
	return 0;
}

int test_kthr_1() {

	int sp_pre, bp_pre;
	asm volatile("MOV %0, r13":"=r"(sp_pre)::);
	asm volatile("MOV %0, r11":"=r"(bp_pre)::);

	msg_received = 0;

	kthr_start(kthr_create(&handler0_a));
	kthr_yield();

	int sp_post, bp_post;
	asm volatile("MOV %0, r13":"=r"(sp_post)::);
	asm volatile("MOV %0, r11":"=r"(bp_post)::);

	if (sp_pre != sp_post || bp_pre != bp_post || msg_received != 1) {
		ERROR("SP: %X, SP_AFTER: %X\n", sp_pre, sp_post);
		ERROR("BP: %X, BP_AFTER: %X\n", bp_pre, bp_post);
		ERROR("msg_received: %d", bp_pre, msg_received);
		return TEST_FAIL;
	}

	return TEST_OK;
}

void run_kthr_tests() {
	Test *tests[NUM_TESTS];
	tests[0] = create_test("test_kthr_1", &test_kthr_1);
	run_tests(tests, NUM_TESTS);
}
