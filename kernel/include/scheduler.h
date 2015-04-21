/*
 * scheduler.h
 *
 *  Created on: Apr 16, 2015
 *      Author: mwkurian
 */

#include "priority_queue.h"
#include "array_list.h"
#include "process.h"
#include "klibc.h"

#ifndef KERNEL_INCLUDE_SCHEDULER_H_
#define KERNEL_INCLUDE_SCHEDULER_H_

typedef struct sched_task {
    pcb * pcb;
    prq_node * node;
    uint32_t parent_pid;
    arrl_handle * children_pids;
    llist_handle * message_queue;
    int niceness;
    int state;
    struct vas * vas_struct;
} sched_task;

typedef struct sched_message_chunk {
    uint32_t src_pid;
    int chunk_length;
    int remain_length;
    uint32_t event;
    char * data;
} sched_message_chunk;

typedef void (*callback_handler)(uint32_t src_pid, uint32_t event, char * data, int chunk_length, int remain_length);

STATUS sched_init();
STATUS sched_free();
STATUS sched_add_task(sched_task * task);
STATUS sched_set_niceness(uint32_t pid);
STATUS shed_remove_task(uint32_t pid);
uint32_t sched_get_active_pid();
void sched_waitpid(uint32_t pid);
sched_task* sched_create_task(uint32_t* file_p, int niceness);
sched_task* sched_get_active_task();
STATUS sched_post_message(uint32_t dest_pid, uint32_t event, char * data, int len);
STATUS sched_register_callback_handler(callback_handler cb_handler);
STATUS sched_deregister_callback_handler();

#endif /* KERNEL_INCLUDE_SCHEDULER_H_ */
