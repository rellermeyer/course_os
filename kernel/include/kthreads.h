/*
 * kthread.h
 *
 *  Created on: Apr 23, 2015
 *      Author: mwkurian
 */

#ifndef KERNEL_INCLUDE_KTHREADS_H_
#define KERNEL_INCLUDE_KTHREADS_H_

#include <global_defs.h>
#include <stdint.h>

typedef void (*kthread_callback_handler)();

typedef struct kthread_handle {
    kthread_callback_handler cb_handler;

    // TODO move to some register save/load class
    uint32_t R0;
    uint32_t R1;
    uint32_t R2;
    uint32_t R3;
    uint32_t R4;
    uint32_t R5;
    uint32_t R6;
    uint32_t R7;
    uint32_t R8;
    uint32_t R9;
    uint32_t R10;
    uint32_t R11;
    uint32_t R12;
    uint32_t R13;
    uint32_t R14;
    uint32_t R15;

} kthread_handle;

kthread_handle* kthread_create(kthread_callback_handler cb_handler);
uint32_t kthread_start(kthread_handle * kthread);
uint32_t kthread_free(kthread_handle * kthread) ;
uint32_t kthread_save_state(kthread_handle * kthread);
uint32_t kthread_load_state(kthread_handle * kthread);

#endif /* KERNEL_INCLUDE_KTHREADS_H_ */
