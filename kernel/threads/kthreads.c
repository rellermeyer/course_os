/*
 * kthreads.c
 *
 *  Created on: Apr 23, 2015
 *      Author: mwkurian
 */

#include "../include/kthreads.h"

#include <global_defs.h>

uint32_t kthread_create(kthread_callback_handler cb_handler) {
    kthread_handle * kthread;
    sched_task * task;

    if (!(kthread = kmalloc(sizeof(kthread_handle)))) {
        return STATUS_FAIL;
    }

    kthread->cb_handler = cb_handler;

    if (!(task = sched_create_task(kthread))) {
        kfree(kthread);
        return STATUS_FAIL;
    }

    kthread->task = task;

    return kthread->task->tid;
}

uint32_t kthread_start(uint32_t tid) {
    return sched_start_task(tid);
}

uint32_t kthread_kill(uint32_t tid) {
    return sched_remove_task(tid);
}

uint32_t kthread_save_state(kthread_handle * kthread){
    return STATUS_FAIL;
}

uint32_t kthread_load_state(kthread_handle * kthread){
    return STATUS_FAIL;
}
