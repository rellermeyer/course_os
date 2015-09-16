#include <stdio.h>
#include "../libc/arch/arm/syscall_arch.h"
#include <fs_syscalls.h>

int main() {
	__syscall3(99, 0, 0, 0);

	printf("Hello world... from hello.c\n");

	void* mem = 0;
	mem = malloc(100);

	printf("malloc returned %x\n", mem);

    free(mem);

    while(1);
}
