/*
 * kthreads.c
 *
 *  Created on: Apr 23, 2015
 *      Author: mwkurian
 */

#include <kthread.h>
#include <stdlib.h>
#include <scheduler.h>

kthread_handle* kthread_create(kthread_callback_handler cb_handler)
{
	kthread_handle * kthread = kmalloc(sizeof(kthread_handle));
	kthread->cb_handler = cb_handler;
	return kthread;
}

uint32_t kthread_start(kthread_handle * kthread)
{
	sched_task * task = sched_create_task_from_kthread(kthread, 0);
	return sched_add_task(task);
}

