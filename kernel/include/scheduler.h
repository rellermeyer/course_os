/*
 * scheduler.h
 *
 *  Created on: Apr 16, 2015
 *      Author: mwkurian
 */

#include "priority_queue.h"
#include "process.h"

#ifndef KERNEL_INCLUDE_SCHEDULER_H_
#define KERNEL_INCLUDE_SCHEDULER_H_

typedef struct sched_task {
	pcb * pcb;
    prq_node * node;
	int niceness;
	int state;
	struct vas * vas_struct;
} sched_task;

typedef struct sched_message_chunk {
	int chunk_length;
	int remain_length;
	uint32_t event;
	char * data;
} sched_message_chunk;

uint32_t sched_init();
uint32_t sched_add_task(sched_task * task);
uint32_t shed_remove_task(uint32_t pid);
void sched_waitpid(uint32_t pid);
uint32_t sched_get_active_pid();
sched_task* sched_create_task(uint32_t* file_p, int niceness);
sched_task* sched_get_active_task();

#endif /* KERNEL_INCLUDE_SCHEDULER_H_ */
