/*
 * scheduler.h
 *
 *  Created on: Apr 16, 2015
 *      Author: mwkurian
 */

#include "process.h"
#include "klibc.h"
#include "data_structures/priority_queue.h"
#include "data_structures/linked_list.h"
#include "data_structures/array_list.h"

#ifndef KERNEL_INCLUDE_SCHEDULER_H_
#define KERNEL_INCLUDE_SCHEDULER_H_

typedef void (*sched_callback_handler)(uint32_t src_pid, uint32_t event, char * data, int chunk_length, int remain_length);

typedef struct sched_task {
    uint32_t parent_tid;
    uint32_t tid;
    uint32_t state;
    uint32_t type;
    int niceness;
    void * task;
    prq_node * node;
    arrl_handle * children_tids;
    sched_callback_handler cb_handler;
    llist_handle * message_queue;
} sched_task;

typedef struct sched_message_chunk {
    uint32_t src_pid;
    int chunk_length;
    int remain_length;
    uint32_t event;
    char * data;
} sched_message_chunk;

uint32_t sched_init(void);
void sched_start(void);
uint32_t sched_free();
uint32_t sched_add_task(sched_task * task);
uint32_t sched_set_niceness(uint32_t pid, uint32_t niceness);
uint32_t shed_remove_task(uint32_t pid);
uint32_t sched_get_active_pid();
void sched_waitpid(uint32_t pid);
sched_task* sched_create_task(uint32_t* file_p, int niceness);
sched_task* sched_create_task_from_kthread(kthread_handle * kthread, int niceness);
sched_task* sched_create_task_from_process(pcb * pcb_pointer, int niceness);
sched_task* sched_get_active_task();
uint32_t sched_post_message(uint32_t dest_pid, uint32_t event, char * data, int len);
uint32_t sched_register_callback_handler(sched_callback_handler cb_handler);
uint32_t sched_deregister_callback_handler();

#endif /* KERNEL_INCLUDE_SCHEDULER_H_ */
