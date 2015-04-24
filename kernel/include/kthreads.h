/*
 * kthread.h
 *
 *  Created on: Apr 23, 2015
 *      Author: mwkurian
 */

#ifndef KERNEL_INCLUDE_KTHREADS_H_
#define KERNEL_INCLUDE_KTHREADS_H_

#include "scheduler.h"

typedef void (*kthread_callback_handler)();

typedef struct kthread_handle {
    sched_task * task;
    kthread_callback_handler cb_handler;
} kthread_handle;

uint32_t kthread_create(kthread_callback_handler cb_handler);
uint32_t kthread_start(uint32_t tid);
uint32_t kthread_kill(uint32_t tid);
uint32_t kthread_save_state(kthread_handle * kthread);
uint32_t kthread_load_state(kthread_handle * kthread);

#endif /* KERNEL_INCLUDE_KTHREADS_H_ */
