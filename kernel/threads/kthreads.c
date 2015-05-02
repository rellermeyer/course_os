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
		return 0;
	}

	kthread->cb_handler = cb_handler;

	return kthread;
}

uint32_t kthread_free(kthread_handle * kthread) {
	if (kthread) {
		kfree(kthread);
		return STATUS_OK;
	}

	return STATUS_FAIL;
}

uint32_t kthread_execute(kthread_handle * kthread, uint32_t parent_tid, uint32_t tid) { /// A

	uint32_t stack_start = (uint32_t) (kthread->stack - (128 * sizeof(double) - 1));

	asm volatile("MOV r13, %0"::"r"(stack_start));

	// ignore the base pointer

	kthread->cb_handler(parent_tid, tid);

	return STATUS_OK;
}
