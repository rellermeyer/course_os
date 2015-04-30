#include "../include/scheduler.h"
#include "../include/vm.h"
#include "../include/klibc.h"
#include "../include/process.h"
#include "../include/linked_list.h"
#include "../include/array_list.h"
#include "../include/hash_map.h"
#include "../include/kthreads.h"

#define MAX_TASKS 100   // in the future, cap will be removed
#define MAX_ACTIVE_TASKS 4  // in the future, will dynamically change based on load
#define MAX_NICENESS -20
#define MIN_NICENESS 20
#define TASK_STATE_FINISHED 4   // task has run by the scheduler and finished
#define TASK_STATE_INACTIVE  3  // task is in the wait queue (about to be  executed)
#define TASK_STATE_ACTIVE 2     // task is part of the running tasks; it is being interleaved and executed atm
#define TASK_STATE_NONE 1       // task is just created (no real state)
#define SAFE_NICE(n) MAX(MIN(MAX_NICENESS, n), n)
#define KTHREAD 0
#define PROCESS 1

#define AS_PROCESS(a) ((pcb*) a->task)
#define AS_KTHREAD(a) ((kthread_handle*) a->task)
#define IS_PROCESS(a) (a->type == PROCESS)
#define IS_KTHREAD(a) (a->type == KTHREAD)

static char last_err[200];
static prq_handle * inactive_tasks;
static prq_handle * active_tasks;
static sched_task * active_task;
static hmap_handle * all_tasks_map;

// NOTE
// scheduler logic only. not tested

// FIXME
// - add comments
// - register interrupt
// - deregister interrupts instead of flag
// - optimize
// - add error messages
// - remove a task (and children)
// - emitting and receiving messages!

// scheduler
// ---------
// round-robin timer-enabled scheduler that runs tasks in descending priority
// with the help of a heap-based priority queue

// supported syscalls
// ---------
// create: create a process (not execute)
// exec: start a process which you created before
// waitpid: wait for a process (i.e. child) to finish
// kill: kill a process and its children processes
//

void __sched_register_timer_irq();
void __sched_deregister_timer_irq();
void __sched_pause_timer_irq();
void __sched_resume_timer_irq();
void __sched_dispatch();
sched_task* __sched_find_subtask(sched_task * parent_task, uint32_t pid);
uint32_t __sched_remove_task(sched_task * task);
uint32_t __sched_create_task(void * task_data, int niceness, uint32_t type);
void __sched_emit_messages();
void __sched_register_timer_irq() {
}
void __sched_deregister_timer_irq() {
}
void __sched_pause_timer_irq() {
}
void __sched_resume_timer_irq() {
}

// Initialize the scheduler. Should be called by the kernel ONLY
uint32_t sched_init() {
	vm_use_kernel_vas();

	os_printf("Initializing scheduler\n");
	// last_err = "No error";
	inactive_tasks = prq_create_fixed(MAX_TASKS);
	active_tasks = prq_create_fixed(MAX_ACTIVE_TASKS);
	all_tasks_map = hmap_create();
	active_task = 0;

	__sched_register_timer_irq();

	return STATUS_OK;
}

uint32_t sched_register_callback_handler(sched_msg_callback_handler cb_handler) {
	sched_task * task = hmap_get(all_tasks_map, sched_get_active_tid());
	task->cb_handler = cb_handler;
	return STATUS_OK;
}

uint32_t sched_deregister_callback_handler() {
	sched_task * task = hmap_get(all_tasks_map, sched_get_active_tid());
	task->cb_handler = 0;

	return STATUS_OK;
}

// Free the resources used the scheduler
uint32_t sched_free() {
	vm_use_kernel_vas();

	// FIXME kill active tasks

	__sched_deregister_timer_irq();

	prq_free(inactive_tasks);
	prq_free(active_tasks);

	return STATUS_OK;
}

static uint32_t sched_tid = 10;

uint32_t sched_create_task_from_kthread(kthread_handle * kthread, int niceness) {
	return __sched_create_task(kthread, niceness, KTHREAD);
}

uint32_t sched_create_task_from_process(pcb * pcb_pointer, int niceness) {
	return __sched_create_task(pcb_pointer, niceness, PROCESS);
}

uint32_t __sched_create_task(void * task_data, int niceness, uint32_t type) {

	if (prq_count(inactive_tasks) >= MAX_TASKS) {
		// last_err = "Too many tasks";
		return STATUS_FAIL;
	}

	__sched_pause_timer_irq();

	sched_task * task = (sched_task*) kmalloc(sizeof(sched_task));

	niceness = SAFE_NICE(niceness);

	task->tid = ++sched_tid;
	task->niceness = niceness;
	task->task = task_data;
	task->type = type;
	task->state = TASK_STATE_NONE;
	task->node = 0;
	task->parent_tid = 0;
	task->children_tids = arrl_create();
	task->message_queue = llist_create();
	task->cb_handler = 0;

	hmap_put(all_tasks_map, task->tid, task);

	if (active_task) {
		task->parent_tid = active_task->tid;
		arrl_append(active_task->children_tids, (void*) task->tid);
	}

	__sched_resume_timer_irq();

	return task->tid;

}

// Helper function used by the scheduler internally. It will traverse the parent/child
// list to find a subtask.
sched_task* __sched_find_subtask(sched_task * parent_task, uint32_t tid) {
	if (parent_task && parent_task->tid == tid) {
		return parent_task;
	}

	int i = 0;
	for (; i < arrl_count(parent_task->children_tids); i++) {
		uint32_t child_tid = (uint32_t) arrl_get(parent_task->children_tids, i);
		sched_task * child_task = hmap_get(all_tasks_map, child_tid);
		if ((child_task = __sched_find_subtask(child_task, tid))) {
			return child_task;
		}
	}

	return 0;
}

//
// NOTE expecting access to kernel global vars
void sched_waittid(uint32_t tid) {
	while (1) {
		sched_task * task = (sched_task*) hmap_get(all_tasks_map, tid);
		if (task == 0 || task->state == TASK_STATE_FINISHED) {
			break;
		}

		// sleep(500);
	}
}

void __sched_dispatch() {
	// prevent interrupts while handling another interrupt
	__sched_pause_timer_irq();

	// use the kernel memory
	vm_use_kernel_vas();

	if (prq_count(active_tasks) < MAX_ACTIVE_TASKS) {
		if (prq_count(inactive_tasks) > 0) {
			prq_enqueue(active_tasks, prq_dequeue(inactive_tasks)); // add to active_tasks if the task
		}
	}

	if (prq_count(active_tasks) == 0) {
		__sched_resume_timer_irq();
		return;
	}

	int i;
	DEBUG("active_tasks: [ ");
	for (i = 0; i < prq_count(active_tasks); i++) {
		DEBUG("%d ", ((sched_task* )prq_get(active_tasks, i)->data)->tid);
	}
	DEBUG("]\n");

	DEBUG("inactive_tasks: [ ");
	for (i = 0; i < prq_count(inactive_tasks); i++) {
		DEBUG("%d ", ((sched_task* )prq_get(inactive_tasks, i)->data)->tid);
	}
	DEBUG("]\n");

	sched_task * last_task;

	// check if there is active task
	if (active_task) {
		last_task = active_task;
	} else {
		prq_node * node = prq_peek(active_tasks);
		last_task = (sched_task*) node->data;
	}

	DEBUG("Last task %d has state %d\n", last_task->tid, last_task->state);

	switch (last_task->state) {
		case TASK_STATE_INACTIVE: {
			active_task = last_task;
			last_task->state = TASK_STATE_ACTIVE;

			DEBUG("Starting task %d\n", last_task->tid);

			if (IS_PROCESS(last_task)) {
				vm_enable_vas(AS_PROCESS(last_task)->stored_vas);
				__sched_resume_timer_irq();
				execute_process_no_vas(AS_PROCESS(last_task));
			} else if (IS_KTHREAD(last_task)) {
				kthread_execute(AS_KTHREAD(last_task));
			}

			__sched_pause_timer_irq();
			__sched_remove_task(last_task);

			// NOTE next interrupt will get the start the process

			break;
		}
		case TASK_STATE_ACTIVE: {
			if (prq_count(active_tasks) > 1) {
				if (active_task) {
					prq_remove(active_tasks, active_task->node);
					prq_enqueue(active_tasks, active_task->node);
				}

				sched_task * next_task =
						(sched_task*) prq_peek(active_tasks)->data;

				DEBUG("Next task %d\n", next_task->tid);

				if (next_task == active_task) {
					break;
				}

				if (active_task) {
					// old task
					if (IS_PROCESS(active_task)) {
						if (active_task == next_task) {
							vm_enable_vas(AS_PROCESS(active_task)->stored_vas);
							break;
						}

						save_process_state(AS_PROCESS(last_task)->PID);
					} else if (IS_KTHREAD(active_task)) {
						if (active_task == next_task) {
							break;
						}

						DEBUG("Saving task state %d\n", active_task->tid);

						kthread_save_state(AS_KTHREAD(active_task));
					}
				}

				active_task = next_task;

				// new task
				if (IS_PROCESS(active_task)) {
					vm_enable_vas(AS_PROCESS(active_task)->stored_vas);
					__sched_emit_messages();
					load_process_state(AS_PROCESS(active_task)->PID); // continue  with the next process
				} else if (IS_KTHREAD(active_task)) {
					// __sched_emit_messages();
					if (next_task->state == TASK_STATE_INACTIVE) {
						next_task->state = TASK_STATE_ACTIVE;
						DEBUG("Starting task %d\n", next_task->tid);
						kthread_execute(AS_KTHREAD(next_task));
						__sched_remove_task(next_task);
					} else if (active_task->state == TASK_STATE_ACTIVE) {
						kthread_load_state(AS_KTHREAD(active_task));
					}
				}
			}
			break;
		}
	}

	__sched_resume_timer_irq();
}

// issue messages for the active task
void __sched_emit_messages() {
	if (active_task->cb_handler) {
		sched_message_chunk * chunk;
		while ((chunk = llist_dequeue(active_task->message_queue)) != 0) {
			active_task->cb_handler(chunk->src_pid, chunk->event, chunk->data,
					chunk->chunk_length, chunk->remain_length);
			if (chunk) {
				kfree(chunk);
			}
		}
	}
}

// start process
uint32_t sched_start_task(uint32_t tid) {
	sched_task * task = hmap_get(all_tasks_map, tid);

	if (task) {
		if (task->state != TASK_STATE_NONE) {
			// last_err = "Reusing task object not allowed";
			return STATUS_FAIL;
		}

		// use the kernel memory
		vm_use_kernel_vas();

		prq_node * new_node = (prq_node*) kmalloc(sizeof(prq_node));
		new_node->data = task;
		new_node->priority = task->niceness;

		task->state = TASK_STATE_INACTIVE;
		task->node = new_node;

		prq_enqueue(inactive_tasks, new_node);

		DEBUG("Added task %d\n", task->tid);

		if (IS_PROCESS(active_task)) {
			vm_enable_vas(AS_PROCESS(active_task)->stored_vas);
		} else if (IS_KTHREAD(active_task)) {
			// ignore
		}

		return STATUS_OK;
	}

	// last_err = "Invalid sched_task pointer";
	return STATUS_FAIL;
}

// essentially a kill process
uint32_t sched_remove_task(uint32_t tid) {
	sched_task * task = (sched_task*) hmap_get(all_tasks_map, tid);
	if (task) {
		__sched_pause_timer_irq();
		uint32_t status = __sched_remove_task(task);
		__sched_resume_timer_irq();
		return status;
	}

	return STATUS_FAIL;
}

// contract
// --------
// must disable timer_interrupt
uint32_t __sched_remove_task(sched_task * task) {
	if (task == NULL) {
		return STATUS_FAIL;
	}

	uint32_t state = task->state;

	DEBUG("Removing task %d\n", task->tid);

	if (active_task == task) {
		active_task = 0;
	}

	switch (state) {
		case TASK_STATE_INACTIVE: {
			task->state = TASK_STATE_FINISHED;

			prq_remove(inactive_tasks, task->node);
			hmap_remove(all_tasks_map, task->tid);
			prq_free_node(task->node);
			break;
		}
		case TASK_STATE_ACTIVE: {

			task->state = TASK_STATE_FINISHED;

			if (IS_PROCESS(task)) {
				vm_use_kernel_vas();
				free_PCB(AS_PROCESS(task));
			} else if (IS_KTHREAD(task)) {
				// FIXME add later
			}

			hmap_remove(all_tasks_map, task->tid);
			prq_remove(active_tasks, task->node);
			prq_free_node(task->node);

			int i = 0;
			for (; i < arrl_count(task->children_tids); i++) {
				sched_task * child_task = (sched_task*) hmap_get(all_tasks_map,
						(uint32_t) arrl_get(task->children_tids, i));
				if (child_task) {
					DEBUG("Removing child_task %d\n", child_task->tid);
					__sched_remove_task(child_task);
				}
			}

			break;
		}
		case TASK_STATE_FINISHED: {
			// ignore
			break;
		}
	}

	__sched_resume_timer_irq();

	return state;
}

// get the current process id
uint32_t sched_get_active_tid() {
	if (active_task) {
		return active_task->tid;
	}

	return STATUS_FAIL;
}

const char * sched_last_err() {
	return last_err;
}

uint32_t sched_set_niceness(uint32_t pid, uint32_t niceness) {

	sched_task * task;

	if (!(task = (sched_task*) hmap_get(all_tasks_map, pid))) {
		return STATUS_FAIL;
	}

	__sched_pause_timer_irq();

	prq_handle * tasks;

	switch (task->state) {
		case TASK_STATE_ACTIVE:
			tasks = active_tasks;
			break;
		case TASK_STATE_INACTIVE:
			tasks = inactive_tasks;
			break;
		case TASK_STATE_FINISHED:
			break;
	}

	if (tasks) {
		prq_remove(tasks, task->node); // remove the running task from queue
		task->node->priority = SAFE_NICE(niceness);
		prq_enqueue(tasks, task->node); // add it again to see if its position in the queue
	}

	__sched_resume_timer_irq();

	return STATUS_OK;
}

uint32_t sched_post_message(uint32_t dest_pid, uint32_t event, char * data,
		int len) {
	/*sched_task * dest_task;

	 if (!(dest_task = hmap_get(all_tasks_map, dest_pid))) {
	 return STATUS_FAIL;
	 }
	 __sched_pause_timer_irq();

	 vm_use_kernel_vas();

	 // create space on kernel stack
	 char data_cpy[512];
	 while (len > 0) {
	 // get length to cpy
	 int cpy_len = MIN(len, 512);
	 // enable orignal struct
	 vm_enable_vas(active_task->pcb->stored_vas);
	 // copy to stack
	 os_memcpy(data, data_cpy, cpy_len);
	 // remaining bytes
	 len -= cpy_len;
	 // increment data pointer
	 data = (char*) (data + cpy_len / 4);
	 // get the dest_task
	 sched_task * dest_task = hmap_get(all_tasks_map, dest_pid);
	 // switch to dest vas
	 vm_enable_vas(dest_task->pcb->stored_vas);
	 // copy from global kernel stack to process heap
	 char* task_mem_data_cpy = kmalloc(cpy_len);
	 os_memcpy(data_cpy, task_mem_data_cpy, cpy_len);

	 vm_use_kernel_vas();

	 // FIXME create the message object and pass it in
	 // messages will be broken up into chunks since we have to
	 // copy into kernel stack first.
	 sched_message_chunk * chunk = kmalloc(sizeof(sched_message_chunk));
	 chunk->data = task_mem_data_cpy;
	 chunk->chunk_length = cpy_len;
	 chunk->remain_length = len;
	 chunk->event = event;
	 chunk->src_pid = active_task ? active_task->pcb->PID : 0;

	 llist_enqueue(dest_task->message_queue, chunk);
	 }

	 if (active_task) {
	 vm_enable_vas(active_task->pcb->stored_vas);
	 }

	 __sched_resume_timer_irq();*/

	return STATUS_OK;
}

uint32_t sched_yield() {
	__sched_dispatch();
	return STATUS_OK;
}
