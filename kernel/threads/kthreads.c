/*
 * kthreads.c
 *
 *  Created on: Apr 23, 2015
 *      Author: mwkurian
 */

#include "../include/kthreads.h"

kthread_handle* kthread_create(kthread_callback_handler cb_handler) {
    kthread_handle * kthread;

    if (!(kthread = kmalloc(sizeof(kthread_handle)))) {
        return STATUS_FAIL;
    }

    kthread->cb_handler = cb_handler;

    return kthread;
}

uint32_t kthread_start(kthread_handle * kthread) {
    kthread->cb_handler();
    return STATUS_OK;
}

uint32_t kthread_free(kthread_handle * kthread) {
    if(kthread){
        kfree(kthread);
        return STATUS_OK;
    }

    return STATUS_FAIL;
}

uint32_t kthread_save_state(kthread_handle * kthread){

    asm("MOV %0, r0":"=r"(kthread->R0)::);
    asm("MOV %0, r1":"=r"(kthread->R1)::);
    asm("MOV %0, r2":"=r"(kthread->R2)::);
    asm("MOV %0, r3":"=r"(kthread->R3)::);
    asm("MOV %0, r4":"=r"(kthread->R4)::);
    asm("MOV %0, r5":"=r"(kthread->R5)::);
    asm("MOV %0, r6":"=r"(kthread->R6)::);
    asm("MOV %0, r7":"=r"(kthread->R7)::);
    asm("MOV %0, r8":"=r"(kthread->R8)::);
    asm("MOV %0, r9":"=r"(kthread->R9)::);
    asm("MOV %0, r10":"=r"(kthread->R10)::);
    asm("MOV %0, r11":"=r"(kthread->R11)::);
    asm("MOV %0, r12":"=r"(kthread->R12)::);
    asm("MOV %0, r13":"=r"(kthread->R13)::);
    asm("MOV %0, r14":"=r"(kthread->R14)::);
    asm("MOV %0, r15":"=r"(kthread->R15)::);

    return STATUS_OK;
}

uint32_t kthread_load_state(kthread_handle * kthread){

    asm("MOV r0, %0"::"r"(kthread->R0):);
    asm("MOV r1, %0"::"r"(kthread->R1):);
    asm("MOV r2, %0"::"r"(kthread->R2):);
    asm("MOV r3, %0"::"r"(kthread->R3):);
    asm("MOV r4, %0"::"r"(kthread->R4):);
    asm("MOV r5, %0"::"r"(kthread->R5):);
    asm("MOV r6, %0"::"r"(kthread->R6):);
    asm("MOV r7, %0"::"r"(kthread->R7):);
    asm("MOV r8, %0"::"r"(kthread->R8):);
    asm("MOV r9, %0"::"r"(kthread->R9):);
    asm("MOV r10, %0"::"r"(kthread->R10):);
    asm("MOV r11, %0"::"r"(kthread->R11):);
    asm("MOV r12, %0"::"r"(kthread->R12):);
    asm("MOV r13, %0"::"r"(kthread->R13):);

    return STATUS_OK;
}
