#include <stdint.h>

#include "../../include/process.h"
#include "../arch/arm/syscall_arch.h"

int process_yield() {
	return (int) __syscall0(SYSCALL_PRCS_YIELD);
}

int process_listen(
		void (*listener)(uint32_t src_tid, uint32_t event, char * data,
				int length)) {
	long error = __syscall1(SYSCALL_PRCS_LISTEN, (uint32_t) listener);

	return (int) error;
}

int process_emit(uint32_t dest_pid, uint32_t event, char * data, int len) {
	long error = __syscall4(SYSCALL_PRCS_EMIT, (uint32_t) dest_pid,
			(uint32_t) event, (uint32_t) data, (uint32_t) len);

	return (int) error;
}

int process_exit(){
	return (int) __syscall0(SYSCALL_PRCS_EXIT);
}
