#include <stdio.h>
#include "../libc/arch/arm/syscall_arch.h"
#include <fs_syscalls.h>

int main() {
	__syscall3(99, 0, 0, 0);

	printf("Hello world... from hello.c\n");

	printf("LET'S TEST %d\n", 10);

	int* mem = 0;
	mem = (int*) malloc(100);

	printf("malloc returned %x\n", mem);

	mem[0] = 1;
	mem[10] = 2;



    free(mem);

    printf("success\n");

    while(1);
}
