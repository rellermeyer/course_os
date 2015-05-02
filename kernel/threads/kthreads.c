/*
 * kthreads.c
 *
 *  Created on: Apr 23, 2015
 *      Author: mwkurian
 */

#include "../include/kthreads.h"
#include "../include/klibc.h"

kthread_handle* kthread_create(kthread_callback_handler cb_handler) {
	kthread_handle * kthread;

	if (!(kthread = kmalloc(sizeof(kthread_handle)))) {
		return STATUS_FAIL;
	}

	kthread->cb_handler = cb_handler;
	kthread->RET = 0;

	return kthread;
}

uint32_t kthread_start(kthread_handle * kthread) {
	kthread->cb_handler();
	return STATUS_OK;
}

uint32_t kthread_free(kthread_handle * kthread) {
	if (kthread) {
		kfree(kthread);
		return STATUS_OK;
	}

	return STATUS_FAIL;
}

uint32_t kthread_execute(kthread_handle * kthread) { /// A

	uint32_t stack_start = (uint32_t) (kthread->stack - (128 * sizeof(double) - 1));

	asm volatile("MOV r2, %0"::"r"(stack_start):);
	asm volatile("MOV r13, r2":::);
	// ignore base_pointer
	// asm volatile("MOV r11, r0":::);

	kthread->cb_handler();

	return STATUS_OK;
}

// FIXME move to jmp
uint32_t kthread_save_state(kthread_handle * kthread) {
	int zero = 0;

	// store
	asm volatile("MOV %0, r0":"=r"(kthread->R0)::);
	asm volatile("MOV %0, r2":"=r"(kthread->R2)::);
	asm volatile("MOV %0, r3":"=r"(kthread->R3)::);
	asm volatile("MOV %0, r4":"=r"(kthread->R4)::);
	asm volatile("MOV %0, r5":"=r"(kthread->R5)::);
	asm volatile("MOV %0, r6":"=r"(kthread->R6)::);
	asm volatile("MOV %0, r7":"=r"(kthread->R7)::);
	asm volatile("MOV %0, r8":"=r"(kthread->R8)::);
	asm volatile("MOV %0, r9":"=r"(kthread->R9)::);
	asm volatile("MOV %0, r10":"=r"(kthread->R10)::);
	asm volatile("MOV %0, r11":"=r"(kthread->R11)::);
	asm volatile("MOV %0, r12":"=r"(kthread->R12)::);
	asm volatile("MOV %0, r13":"=r"(kthread->R13)::);
	asm volatile("MOV %0, r14":"=r"(kthread->R14)::);
	asm volatile("MOV r1, %0"::"r"(zero):);  // <------
	asm volatile("MOV %0, r15":"=r"(kthread->R15)::); // <------

	int ret;
	asm volatile("MOV %0, r1":"=r"(ret)::);

	return ret;
}

uint32_t kthread_load_state(kthread_handle * kthread, uint32_t on_resume) {

	// load the resume
	asm volatile("MOV r1, %0"::"r"(on_resume):);

	// load the rest
	asm volatile("MOV r2, %0"::"r"(kthread->R11):);
	asm volatile("MOV r0, %0"::"r"(kthread->R15):);

	asm volatile("MOV r4, %0"::"r"(kthread->R4):);
	asm volatile("MOV r5, %0"::"r"(kthread->R5):);
	asm volatile("MOV r6, %0"::"r"(kthread->R6):);
	asm volatile("MOV r7, %0"::"r"(kthread->R7):);
	asm volatile("MOV r8, %0"::"r"(kthread->R8):);
	asm volatile("MOV r9, %0"::"r"(kthread->R9):);
	asm volatile("MOV r10, %0"::"r"(kthread->R10):);
	asm volatile("MOV r12, %0"::"r"(kthread->R12):);
	asm volatile("MOV r13, %0"::"r"(kthread->R13):);
	asm volatile("MOV r14, %0"::"r"(kthread->R14):);

	asm volatile("MOV r11, r2":::);
	asm volatile("MOV r15, r0":::);

	return STATUS_OK;
}
