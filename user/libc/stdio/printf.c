#include <stdio.h>
#include <stdarg.h>
#include "../arch/arm/syscall_arch.h"

int printf(const char *restrict fmt, ...)
{
	//int ret;
	//va_list ap;
	//va_start(ap, fmt);
	//ret = vfprintf(stdout, fmt, ap);
	//va_end(ap);
    //return ret;
    long error = __syscall1(SYSCALL_PRINTF, (long)fmt);
	return error;
}
