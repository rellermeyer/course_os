#include "../include/jump.h"
#include "../include/klibc.h"

uint32_t jmp_set(jmp_buf * buffer) {
	int zero = 0;

	// store
	asm volatile("MOV %0, r0":"=r"(buffer->R0)::);
	asm volatile("MOV %0, r2":"=r"(buffer->R2)::);
	asm volatile("MOV %0, r3":"=r"(buffer->R3)::);
	asm volatile("MOV %0, r4":"=r"(buffer->R4)::);
	asm volatile("MOV %0, r5":"=r"(buffer->R5)::);
	asm volatile("MOV %0, r6":"=r"(buffer->R6)::);
	asm volatile("MOV %0, r7":"=r"(buffer->R7)::);
	asm volatile("MOV %0, r8":"=r"(buffer->R8)::);
	asm volatile("MOV %0, r9":"=r"(buffer->R9)::);
	asm volatile("MOV %0, r10":"=r"(buffer->R10)::);
	asm volatile("MOV %0, r11":"=r"(buffer->R11)::);
	asm volatile("MOV %0, r12":"=r"(buffer->R12)::);
	asm volatile("MOV %0, r13":"=r"(buffer->R13)::);
	asm volatile("MOV %0, r14":"=r"(buffer->R14)::);
//	asm volatile("MRS %0, CPSR":"=r"(buffer->CPSR)::);
	asm volatile("MOV r1, %0"::"r"(zero):);
	asm volatile("MOV %0, r15":"=r"(buffer->R15)::);

	int ret;
	asm volatile("MOV %0, r1":"=r"(ret)::);

	return ret;
}

uint32_t jmp_goto(jmp_buf * buffer, uint32_t value) {
	int x = 1;
	while(x);

	// load the resume
	asm volatile("MOV r1, %0"::"r"(value):);

	// load the rest
	asm volatile("MOV r2, %0"::"r"(buffer->R11):);
	asm volatile("MOV r0, %0"::"r"(buffer->R15 + 4):);

	asm volatile("MOV r4, %0"::"r"(buffer->R4):);
	asm volatile("MOV r5, %0"::"r"(buffer->R5):);
	asm volatile("MOV r6, %0"::"r"(buffer->R6):);
	asm volatile("MOV r7, %0"::"r"(buffer->R7):);
	asm volatile("MOV r8, %0"::"r"(buffer->R8):);
	asm volatile("MOV r9, %0"::"r"(buffer->R9):);
	asm volatile("MOV r10, %0"::"r"(buffer->R10):);
	asm volatile("MOV r12, %0"::"r"(buffer->R12):);
	asm volatile("MOV r13, %0"::"r"(buffer->R13):);
	asm volatile("MOV r14, %0"::"r"(buffer->R14):);
//	asm volatile("MRS CPSR, %0":"r"(buffer->CPSR):);

	asm volatile("MOV r11, r2":::);
	asm volatile("MOV r15, r0":::);

	return STATUS_OK;
}

void jmp_print(jmp_buf * buffer) {
	DEBUG("r0 [0x%x]\n", buffer->R0);
	DEBUG("r1 [0x%x]\n", buffer->R1);
	DEBUG("r2 [0x%x]\n", buffer->R2);
	DEBUG("r3 [0x%x]\n", buffer->R3);
	DEBUG("r4 [0x%x]\n", buffer->R4);
	DEBUG("r5 [0x%x]\n", buffer->R5);
	DEBUG("r6 [0x%x]\n", buffer->R6);
	DEBUG("r7 [0x%x]\n", buffer->R7);
	DEBUG("r8 [0x%x]\n", buffer->R8);
	DEBUG("r9 [0x%x]\n", buffer->R9);
	DEBUG("r10 [0x%x]\n", buffer->R10);
	DEBUG("r11 [0x%x]\n", buffer->R11);
	DEBUG("r12 [0x%x]\n", buffer->R12);
	DEBUG("r13 [0x%x]\n", buffer->R13);
	DEBUG("r14 [0x%x]\n", buffer->R14);
	DEBUG("r15 [0x%x]\n", buffer->R15);
}
