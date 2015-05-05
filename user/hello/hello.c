#include <stdio.h>
#include <process.h>

//char * msg = "whatsup!!!\0";
//uint32_t msg_len = 11;
//uint32_t event_id = 1;
//
//uint32_t listener(uint32_t parent_tid) {
//	// TODO @MarkDaniels. make sure to handle this from streams
//	// if (sched_get_message_space() >= msg_len) {
//	if (sched_post_message(parent_tid, event_id, msg, msg_len)) {
//		printf("Message fail\n");
//	}
//	// }
//
//	return 0;
//}

int main(int argc, char ** argv) {
//	printf("Process started\n", 99);
//	for(int i = 0; i < argc; i++){
//		printf(argv[i], i);
//	}

	// long data;

	__asm ("mov r0, #0x15":::);
	__asm ("msr CPSR, r0":::);
	__asm ("msr SPSR, r0":::);
	// __asm("msr %0, CPSR_fc":"=r"(data)::);

	printf("Process started\n", 99);

	process_yield();
	printf("Process continued\n", 99);

	__asm ("mov r0, #0x15":::);
	__asm ("msr CPSR, r0":::);
	__asm ("msr SPSR, r0":::);

	process_yield();

	printf("Process finishing\n", 99);
//	printf(arg);
//	printf("Whats up from process RESUMED!!!!\n");
//	process_yield();
//	printf(arg);
//	printf("Whats up from process RESUMED 2!!!!\n");
	while (1)
		;
}
