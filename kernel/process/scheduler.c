#include "../include/scheduler.h"
#include "../include/vm.h"
#include "../include/klibc.h"
#include "../include/process.h"
#include "../include/linked_list.h"
#include "../include/array_list.h"
#include "../include/hash_map.h"
#include "../include/jump.h"
#include "../include/kthreads.h"

#define MAX_TASKS 100   // in the future, cap will be removed
#define MAX_ACTIVE_TASKS 4  // in the future, will dynamically change based on load
#define MAX_NICENESS -20
#define MIN_NICENESS 20
#define TASK_RESUME 5
#define TASK_STATE_FINISHED 4   // task has run by the scheduler and finished
#define TASK_STATE_INACTIVE  3  // task is in the wait queue (about to be  executed)
#define TASK_STATE_ACTIVE 2     // task is part of the running tasks; it is being interleaved and executed atm
#define TASK_STATE_NONE 1       // task is just created (no real state)
#define SAFE_NICE(n) MAX(MIN(MAX_NICENESS, n), n)
#define KTHREAD 0x88
#define PROCESS 0x99
#define MAX_TASK_MSG_SPACE 2048 // bytes

#define AS_PROCESS(a) ((pcb*) a->task)
#define AS_KTHREAD(a) ((kthread_handle*) a->task)
#define IS_PROCESS(a) (a->type == PROCESS)
#define IS_KTHREAD(a) (a->type == KTHREAD)

char last_err[200];
prq_handle * inactive_tasks;
prq_handle * active_tasks;
sched_task * active_task;
hmap_handle * all_tasks_map;
jmp_buf start_buf;
uint32_t running;

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
	running = 0;

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

// defer process create
uint32_t sched_create_task_from_process(char * file, int niceness) {
	return __sched_create_task(file, niceness, PROCESS);
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
	task->parent_tid = active_task ? active_task->tid : 0;
	task->children_tids = arrl_create();
	task->message_queue = llist_create();
	task->cb_handler = 0;
	task->available_space = MAX_TASK_MSG_SPACE;

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

void __sched_print_queues() {
	int i;
	DEBUG("active_tasks: [ ");
	for (i = 0; i < prq_count(active_tasks); i++)
		DEBUG("%d ", ((sched_task* ) prq_get(active_tasks, i)->data)->tid);
	DEBUG("]\n");
	DEBUG("inactive_tasks: [ ");
	for (i = 0; i < prq_count(inactive_tasks); i++)
		DEBUG("%d ", ((sched_task* ) prq_get(inactive_tasks, i)->data)->tid);
	DEBUG("]\n");
}

void  __sched_dispatch() {
	// prevent interrupts while handling another interrupt
	__sched_pause_timer_irq();

	// use the kernel memory
	vm_use_kernel_vas();

	if (active_task) {
		if (jmp_set(&active_task->jmp_buffer)) {
			return;
		}

		jmp_print(&active_task->jmp_buffer);

		prq_enqueue(active_tasks, active_task->node);
	}

	if (!running) {
		if (jmp_set(&start_buf)) {
			active_task = 0;
			running = 0;
			return;
		}
		running = 1;
	} else if (prq_count(inactive_tasks) == 0 && prq_count(active_tasks) == 0) {
		jmp_goto(&start_buf, TASK_RESUME);
	}

	// add task from inactive -> active
	if (prq_count(active_tasks) < MAX_ACTIVE_TASKS) {
		if (prq_count(inactive_tasks) > 0) {
			prq_enqueue(active_tasks, prq_dequeue(inactive_tasks)); // add to active_tasks if the task
		}
	}

	__sched_print_queues();

	// if queue is empty don't dispatch anything
	if (prq_count(active_tasks) == 0) {
		__sched_resume_timer_irq();
		return;
	}

	// logging
	// FIXME ensure that another task is running if priorities are the same
	active_task = (sched_task*) prq_dequeue(active_tasks)->data;

	if (IS_KTHREAD(active_task)) {
		if (active_task->state == TASK_STATE_ACTIVE) {
			__sched_emit_messages();
			jmp_print(&active_task->jmp_buffer);
			jmp_goto(&active_task->jmp_buffer, TASK_RESUME);
		} else if (active_task->state == TASK_STATE_INACTIVE) {
			active_task->state = TASK_STATE_ACTIVE;
			__sched_emit_messages();
			kthread_execute(AS_KTHREAD(active_task), active_task->parent_tid,
					active_task->tid);
		} else {
			WARN("Task %d has unexpected state %d", active_task->tid,
					active_task->state);
		}
	} else if (IS_PROCESS(active_task)) {
		LOG("Active_task is a process!\n");
		if (active_task->state == TASK_STATE_ACTIVE) {
			__sched_emit_messages();
			jmp_buf jmp_buffer_cpy = active_task->jmp_buffer; // move to kernel stack
			vm_enable_vas(AS_PROCESS(active_task)->stored_vas);
			__sched_resume_timer_irq();
			jmp_goto(&jmp_buffer_cpy, TASK_RESUME);
		} else if (active_task->state == TASK_STATE_INACTIVE) {
			active_task->state = TASK_STATE_INACTIVE;
			active_task->task = process_create_from_file(active_task->task,
					"hey whats up!!!");
			process_init(AS_PROCESS(active_task));
			// FIXME delay timer irq
			__sched_resume_timer_irq();
			// NOTE vm_enable_vas called inside process_execute
			process_execute(AS_PROCESS(active_task));
		} else {
			WARN("Task %d has unexpected state %d", active_task->tid,
					active_task->state);
		}
	}

	// FIXME jump to main and remove
	__sched_remove_task(active_task);
	__sched_dispatch();
	__sched_resume_timer_irq();
}

// FIXME stack will be that of the kernel; protect it
// issue messages for the active task
void __sched_emit_messages() {
	if (active_task->cb_handler) {
		sched_message_chunk * chunk;
		while ((chunk = llist_dequeue(active_task->message_queue)) != 0) {
			active_task->cb_handler(chunk->src_tid, chunk->event, chunk->data,
					chunk->length);
			if (chunk) {
				kfree(chunk->data);
				kfree(chunk);
			}
			sched_task * sending_task = hmap_get(all_tasks_map, chunk->src_tid);
			sending_task->available_space += chunk->length;
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
// must disable timer_interruptU
uint32_t __sched_remove_task(sched_task * task) {
	if (task == NULL) {
		return STATUS_FAIL;
	}

	uint32_t state = task->state;

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
				process_free_PCB(AS_PROCESS(task));
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

uint32_t sched_get_message_space() {
	if (active_task) {
		return active_task->available_space;
	}

	return 0;
}

// contract: callee must delete the data that is being passed
#define SHARED_MEMORY_START 0x12000000
uint32_t sched_post_message(uint32_t dest_pid, uint32_t event, uint32_t * data,
		int len) {

	sched_task * task = hmap_get(all_tasks_map, dest_pid);

	if ((active_task->available_space - len) < 0) {
		return STATUS_FAIL;
	}

	if (!task || task->state == TASK_STATE_FINISHED) {
		return STATUS_FAIL;
	}

	__sched_pause_timer_irq();

	vm_use_kernel_vas();

	uint32_t * source;

	if (IS_PROCESS(active_task)) {
		for (int i = 0; i < (len / BLOCK_SIZE); i++) {
			vm_map_shared_memory(AS_PROCESS(active_task)->stored_vas,
					(void*) (data + (i * BLOCK_SIZE)), // FIXME char to uint32_t issue
					KERNEL_VAS,
					(void*) (SHARED_MEMORY_START + (i * BLOCK_SIZE)),
					VM_PERM_USER_RW);
		}

		source = (uint32_t*) SHARED_MEMORY_START;
	} else if (IS_KTHREAD(active_task)) {
		source = data;
	}

	char * data_cpy = kmalloc(len);
	os_memcpy((uint32_t*) source, (uint32_t*) data_cpy, len);

	sched_message_chunk * chunk = kmalloc(sizeof(sched_message_chunk));
	chunk->data = data_cpy;
	chunk->length = len;
	chunk->event = event;
	chunk->src_tid = active_task ? active_task->tid : 0;

	active_task->available_space -= len;

	llist_enqueue(task->message_queue, chunk);

	if (IS_PROCESS(active_task)) {
		for (int i = 0; i < (len / BLOCK_SIZE); i++) {
			vm_free_mapping(KERNEL_VAS,
					(void*) (SHARED_MEMORY_START + (i * BLOCK_SIZE)));
		}

		vm_enable_vas(AS_PROCESS(active_task)->stored_vas);
	}

	__sched_resume_timer_irq();

	return STATUS_OK;
}

uint32_t sched_yield() {
	__sched_dispatch();

	return STATUS_OK;
}
