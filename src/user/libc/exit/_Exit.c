#include <stdlib.h>
#include "syscall.h"

#define SYS_exit 100

_Noreturn void _Exit(int ec)
{
	//__syscall(SYS_exit_group, ec);
	//for (;;) __syscall(SYS_exit, ec);
	for (;;) __syscall(SYS_exit, ec);
}
