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
#include "jump.h"

typedef uint32_t (*kthread_callback_handler)(uint32_t parent_tid, uint32_t tid);

typedef struct kthread_handle {
    kthread_callback_handler cb_handler;

    int stack_set;
    double stack[128];

    jmp_buf jmp_buffer;

} kthread_handle;

kthread_handle* kthread_create(kthread_callback_handler cb_handler);
uint32_t kthread_free(kthread_handle * kthread) ;
uint32_t kthread_execute(kthread_handle * kthread, uint32_t parent_tid, uint32_t tid);

#endif /* KERNEL_INCLUDE_KTHREADS_H_ */
