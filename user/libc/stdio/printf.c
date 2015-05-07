#include <stdio.h>
#include <stdarg.h>
#include "../arch/arm/syscall_arch.h"

int printf(const char *restrict fmt, ...) {
	//int ret;
	//va_list ap;
	//va_start(ap, fmt);
	//ret = vfprintf(stdout, fmt, ap);
	//va_end(ap);
	//return ret;
	va_list vl;
	va_start(vl, 1);
	long val = va_arg(vl, long);
	long error = __syscall(SYSCALL_PRINTF, fmt, val);
	return error;
}
