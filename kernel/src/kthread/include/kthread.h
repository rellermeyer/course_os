/*
 * kthread.h
 *
 *  Created on: Apr 23, 2015
 *      Author: mwkurian
 */

#ifndef KERNEL_INCLUDE_KTHREAD_H_
#define KERNEL_INCLUDE_KTHREAD_H_

#include <stdint.h>

typedef void (*kthread_callback_handler)();

typedef struct kthread_handle {
    uint32_t parent_pid;
    int niceness;
    int state;
    kthread_callback_handler cb_handler;
} kthread_handle;



#endif /* KERNEL_INCLUDE_KTHREAD_H_ */
