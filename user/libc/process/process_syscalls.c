#include <stdint.h>

#include "../../include/process.h"
#include "../arch/arm/syscall_arch.h"

int process_yield() {
	int ret;
	__asm("MOV %0, lr":"=r"(ret)::);

	long res = __syscall1(SYSCALL_PRCS_YIELD, ret);

	__asm("MOV lr, %0"::"r"(res):);
	__asm("pop { r0 }");
	__asm("pop { r0 }");
	__asm("push { lr }");
	__asm("push { r0 }");

	return (int) res;
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

