/*
 * scheduler.h
 *
 *  Created on: Apr 16, 2015
 *      Author: mwkurian
 */



#ifndef KERNEL_INCLUDE_SCHEDULER_H_
#define KERNEL_INCLUDE_SCHEDULER_H_

#include "process.h"
#include "klibc.h"
#include "kthreads.h"
#include "priority_queue.h"
#include "array_list.h"
#include "linked_list.h"

typedef void (*sched_msg_callback_handler)(uint32_t src_tid, uint32_t event, char * data, int length);

typedef struct sched_task {
    uint32_t parent_tid;
    uint32_t tid;
    uint32_t state;
    uint32_t type;
    int available_space;
    int niceness;
    void * task;
    prq_node * node;
    arrl_handle * children_tids;
    sched_msg_callback_handler cb_handler;
    llist_handle * message_queue;
} sched_task;

typedef struct sched_message_chunk {
    uint32_t src_tid;
    int length;
    int remain_length;
    uint32_t event;
    char * data;
} sched_message_chunk;

uint32_t sched_init();
uint32_t sched_free();
uint32_t sched_add_task(sched_task * task);
uint32_t sched_set_niceness(uint32_t pid, uint32_t niceness);
uint32_t shed_remove_task(uint32_t pid);
uint32_t sched_get_active_tid();
void sched_waitpid(uint32_t pid);
uint32_t sched_create_task_from_kthread(kthread_handle * kthread, int niceness);
uint32_t sched_create_task_from_process(pcb * pcb_pointer, int niceness);
sched_task* sched_get_active_task();
uint32_t sched_post_message(uint32_t dest_pid, uint32_t event, char * data, int len);
uint32_t sched_register_callback_handler(sched_msg_callback_handler cb_handler);
uint32_t sched_deregister_callback_handler();
uint32_t sched_yield();
uint32_t sched_get_message_space();

#endif /* KERNEL_INCLUDE_SCHEDULER_H_ */
