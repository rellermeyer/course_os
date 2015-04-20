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

uint32_t sched_init();
sched_task* sched_create_task(uint32_t* file_p, int niceness);
uint32_t sched_add_task(sched_task * task);
uint32_t shed_remove_task(sched_task * task);
sched_task* sched_get_active_task();
uint32_t sched_get_active_pid();

#endif /* KERNEL_INCLUDE_SCHEDULER_H_ */
