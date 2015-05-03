#include <stdint.h>

#include "../../include/process.h"
#include "../arch/arm/syscall_arch.h"

int process_yield(){
	long error = __syscall0(SYSCALL_PRCS_YIELD);

	return (int)error;
}
