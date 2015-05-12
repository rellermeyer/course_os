/*
 *
 * Task Scheduler
 *
 * A round-robin timer-enabled scheduler that runs tasks in descending priority
 * with the help of a heap-based priority queue. Tasks include kthreads and processes
 *
 * contact: Mathew Kurian <bluejamesbond@gmail.com>
 *
 * TODO
 * - optimize
 * - add error messages
 * - remove a task (and children)
 *
 */

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
#define TASK_RESUME 5
#define TASK_FINISHED 10
#define TASK_CREATE_PROCESS 11
#define TASK_RESUME_PROCESS 12
#define TASK_STATE_FINISHED 4   // task has run by the scheduler and finished
#define TASK_STATE_INACTIVE  3  // task is in the wait queue (about to be  executed)
#define TASK_STATE_ACTIVE 2     // task is part of the running tasks; it is being interleaved and executed atm
#define TASK_STATE_NONE 1       // task is just created (no real state)
#define SAFE_NICE(n) MAX(MIN(MAX_NICENESS, n), n)
#define KTHREAD 0x88
#define PROCESS 0x99
#define MAX_TASK_MSG_SPACE 2048 // bytes
#define NO_ARG 0
#define SHARED_MSG_MEMORY_START 0x12000000

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

void __sched_dispatch();
sched_task* __sched_find_subtask(sched_task * parent_task, uint32_t pid);
uint32_t __sched_remove_task(sched_task * task);
uint32_t __sched_create_task(void * task_data, int niceness, uint32_t type,
		int argc, char ** argv);
void __sched_emit_messages();

/*
 * Initialize the scheduler. Should be called by the kernel ONLY
 */
uint32_t sched_init() {
	vm_use_kernel_vas();

	LOG("Initializing scheduler\n");

	// last_err = "No error";
	inactive_tasks = prq_create_fixed(MAX_TASKS);
	active_tasks = prq_create_fixed(MAX_ACTIVE_TASKS);
	all_tasks_map = hmap_create();
	active_task = 0;
	running = 0;

	return STATUS_OK;
}

/**
 * Registers a message listener (abstracted through streams.c)
 *
 * Allows a task to register a function which will be used to
 *
 * @param cb_handler callback handler
 *
 * @return STATUS_OK or STATUS_FAIL
 */
uint32_t sched_register_callback_handler(sched_msg_callback_handler cb_handler) {
	sched_task * task = hmap_get(all_tasks_map, sched_get_active_tid());
	task->cb_handler = cb_handler;
	return STATUS_OK;
}

/**
 * Deregister a message listener (abstracted through streams.c)
 *
 * @param cb_handler callback handler
 *
 * @return STATUS_OK or STATUS_FAIL
 */
uint32_t sched_deregister_callback_handler() {
	sched_task * task = hmap_get(all_tasks_map, sched_get_active_tid());
	task->cb_handler = 0;

	return STATUS_OK;
}

/*
 * Free the resources used the scheduler
 */
uint32_t sched_free() {
	vm_use_kernel_vas();

	// FIXME kill active tasks
	prq_free(inactive_tasks);
	prq_free(active_tasks);

	return STATUS_OK;
}

static uint32_t sched_tid = 10;

/**
 *
 * Creates a kthread task (refer to kthread.c)
 *
 * @param cb_handler callback handler
 *
 * @returns task id (tid)
 */
uint32_t sched_create_task_from_kthread(kthread_handle * kthread, int niceness) {
	return __sched_create_task(kthread, niceness, KTHREAD, NO_ARG, NO_ARG);
}

/**
 *
 * Creates a process task (refer to process.c)
 *
 * @param cb_handler callback handler
 *
 * @returns task id (tid)
 */

// defer process create
uint32_t sched_create_task_from_process(char * file, int niceness, int argc,
		char ** argv) {
	return __sched_create_task(file, niceness, PROCESS, argc, argv);
}

/*
 * Internal helper function to create a task
 */
uint32_t __sched_create_task(void * task_data, int niceness, uint32_t type,
		int argc, char ** argv) {

	if (prq_count(inactive_tasks) >= MAX_TASKS) {
		// last_err = "Too many tasks";
		return STATUS_FAIL;
	}

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
	task->argv = argv;
	task->argc = argc;

	hmap_put(all_tasks_map, task->tid, task);

	if (active_task) {
		task->parent_tid = active_task->tid;
		arrl_append(active_task->children_tids, (void*) task->tid);
	}

	return task->tid;

}

/*
 * Helper function used by the scheduler internally. It will traverse the parent/child
 * list to find a subtask.
 */
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

/**
 * Waits until a child task has terminated
 *
 * @param tid of child task
 *
 * @return does not return a value;
 */
void sched_waittid(uint32_t tid) {
	// TODO check if child or children or otherwise, return out of this
	while (1) {
		sched_task * task = (sched_task*) hmap_get(all_tasks_map, tid);
		if (task == 0 || task->state == TASK_STATE_FINISHED) {
			break;
		}

		// priority donation
		if (active_task->niceness < task->niceness) {
			sched_set_niceness(task->tid, active_task->niceness);
		}

		// dirty solution
		sched_yield();

		// sleep(500);
	}
}

/**
 * Internal helper function to print priority queues
 */
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

/**
 * Internal helper function to be used switch to the next task
 */
void __sched_dispatch() {
	// use the kernel memory
	vm_use_kernel_vas();

	if (active_task) {
		if (IS_KTHREAD(active_task)) {
			if (jmp_set(&active_task->jmp_buffer)) {
				return;
			}
		}

		prq_enqueue(active_tasks, active_task->node);
	}

	// Uncomment this line to print out the inactive/active task queues
	// inactive_tasks: tasks that are waiting in line
	// active_tasks: tasks that are being interleaved

	// __sched_print_queues();

	if (!running) {
		int ret = jmp_set(&start_buf);
		if (ret == TASK_FINISHED) {
			active_task = 0;
			running = 0;
			return;
		} else if (ret == TASK_CREATE_PROCESS) {
			active_task->task = process_create_from_file(active_task->task,
					active_task->argc, active_task->argv);
			process_execute(AS_PROCESS(active_task));
			return;
		} else if (ret == TASK_RESUME_PROCESS) {
			jmp_buf jmp_buffer_cpy = active_task->jmp_buffer;
			vm_enable_vas(AS_PROCESS(active_task)->stored_vas);
			process_set_sysetm_mode_sp(jmp_buffer_cpy.R13);
			process_load_state(&jmp_buffer_cpy);
			return;
		}

		running = 1;

	} else if (prq_count(inactive_tasks) == 0 && prq_count(active_tasks) == 0) {
		jmp_goto(&start_buf, TASK_FINISHED);
	}

	// add task from inactive -> active
	if (prq_count(active_tasks) < MAX_ACTIVE_TASKS) {
		if (prq_count(inactive_tasks) > 0) {
			prq_enqueue(active_tasks, prq_dequeue(inactive_tasks)); // add to active_tasks if the task
		}
	}

	// if queue is empty don't dispatch anything
	if (prq_count(active_tasks) == 0) {
		return;
	}

	// alternate between equal priorities
	if (prq_count(active_tasks) > 1) {
		sched_task * next_task = (sched_task*) prq_dequeue(active_tasks)->data;
		sched_task * foll_task = (sched_task*) prq_peek(active_tasks)->data;
		if (next_task->niceness == foll_task->niceness
				&& next_task == active_task) {
			active_task = (sched_task*) prq_dequeue(active_tasks)->data;
			prq_enqueue(active_tasks, next_task->node);
		} else {
			active_task = next_task;
		}
	} else {
		active_task = (sched_task*) prq_dequeue(active_tasks)->data;
	}

	LOG("active_task %d\n", active_task->tid);

	if (IS_KTHREAD(active_task)) {
		if (active_task->state == TASK_STATE_ACTIVE) {
			__sched_emit_messages();
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
		if (active_task->state == TASK_STATE_ACTIVE) {
			__sched_emit_messages();
			jmp_goto(&start_buf, TASK_RESUME_PROCESS); // jump to kernel context and start the process
		} else if (active_task->state == TASK_STATE_INACTIVE) {
			active_task->state = TASK_STATE_ACTIVE;
			jmp_goto(&start_buf, TASK_CREATE_PROCESS); // jump to kernel context and start the process
		} else {
			WARN("Task %d has unexpected state %d", active_task->tid,
					active_task->state);
		}
	}

	// FIXME jump to main and remove
	__sched_remove_task(active_task);
	__sched_dispatch();
}

/**
 * Internal helper function to emit messages into a task
 */

// FIXME stack will be that of the kernel; protect it
// issue messages for the active task
void __sched_emit_messages() {
	if (active_task->cb_handler) {
		sched_message_chunk * chunk;
		uint32_t length;
		uint32_t * source;
		while ((chunk = llist_dequeue(active_task->message_queue)) != 0) {

			length = chunk->length;

			if (IS_PROCESS(active_task)) {
				// FIXME this code is wrong!!!!
				// You must allocate memory at SHARED_MSG_MEMORY_START in user process before that process
				// is executed. Also, no need a for loop, just make shared memory at SHARED_MSG_MEMORY_START
				for (int i = 0; i < (length / BLOCK_SIZE); i++) {
					vm_map_shared_memory(KERNEL_VAS,
							(void*) (SHARED_MSG_MEMORY_START + (i * BLOCK_SIZE)),
							AS_PROCESS(active_task)->stored_vas,
							(void*) (SHARED_MSG_MEMORY_START + (i * BLOCK_SIZE)),
							VM_PERM_USER_RW);
				}

				source = (uint32_t*) SHARED_MSG_MEMORY_START;

				os_memcpy(chunk->data, (uint32_t*) SHARED_MSG_MEMORY_START,
						chunk->length);
			} else if (IS_KTHREAD(active_task)) {
				source = (uint32_t*) chunk->data;
			}

			active_task->cb_handler(chunk->src_tid, chunk->event, source,
					chunk->length);

			if (chunk) {
				kfree(chunk->data);
				kfree(chunk);
			}

			sched_task * sending_task = hmap_get(all_tasks_map, chunk->src_tid);
			sending_task->available_space += length;

			if (IS_PROCESS(active_task)) {
				for (int i = 0; i < (length / BLOCK_SIZE); i++) {
					vm_free_mapping(KERNEL_VAS,
							(void*) (SHARED_MSG_MEMORY_START + (i * BLOCK_SIZE)));
				}
			}
		}
	}
}

/**
 * Start a task from a task id (tid)
 *
 * @param uint32_t task id (tid)
 *
 * @return STATUS_OK or STATUS_FAIL
 */

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

/**
 * Kill/remove a task
 *
 * @param uint32_t task id (tid)
 *
 * @return STATUS_OK or STATUS_FAIL
 */

// TODO check if child or sub child of active_process before allowing this operation
// NOTE access children through active_task->children_tids, possibly leverage __sched_find_subtask
uint32_t sched_remove_task(uint32_t tid) {
	sched_task * task = (sched_task*) hmap_get(all_tasks_map, tid);
	if (task) {
		uint32_t status = __sched_remove_task(task);
		return status;
	}

	return STATUS_FAIL;
}

/**
 * Internal helper function to remove a task
 */
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
				process_free_pcb(AS_PROCESS(task));
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

	return state;
}

/**
 * Get the active task id
 *
 * @return uint32_t active_task->tid
 */
uint32_t sched_get_active_tid() {
	if (active_task) {
		return active_task->tid;
	}

	return STATUS_FAIL;
}

/**
 * Returns the scheduler's last error (needs a lot of work)
 *
 * @return const char* error message
 */
const char * sched_last_err() {
	return last_err;
}
/**
 * Set the priority of a task (-20 to 20)
 *
 * @param uint32_t pid
 * @param uint32_t niceness (the new niceness)
 *
 * @return STATUS_OK or STATUS_FAIL
 */

// TODO check if child or sub child of active_process before allowing this operation
// access children through active_task->children_tids, possibly leverage __sched_find_subtask
uint32_t sched_set_niceness(uint32_t pid, uint32_t niceness) {

	sched_task * task;

	if (!(task = (sched_task*) hmap_get(all_tasks_map, pid))) {
		return STATUS_FAIL;
	}

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

	return STATUS_OK;
}

/**
 * Gets the active tasks's available space for messaging (refer to streams.c)
 *
 * @return # of bytes available
 */
uint32_t sched_get_message_space() {
	if (active_task) {
		return active_task->available_space;
	}

	return 0;
}

/**
 * Post/emit a message to another task  (refer to streams.c).
 * CONTRACT: callee must delete the data that is being passed
 *
 * @param uint32_t dest_pid
 * @param uint32_t event
 * @param uint32_t* data
 * @param int len data length in bytes
 *
 * @return STATUS_OK or STATUS_FAIL
 */
uint32_t sched_post_message(uint32_t dest_pid, uint32_t event, uint32_t * data,
		int len) {

	sched_task * task = hmap_get(all_tasks_map, dest_pid);

	if ((active_task->available_space - len) < 0) {
		return STATUS_FAIL;
	}

	if (!task || task->state == TASK_STATE_FINISHED) {
		return STATUS_FAIL;
	}

	vm_use_kernel_vas();

	uint32_t * source;

	if (IS_PROCESS(active_task)) {
		for (int i = 0; i < (len / BLOCK_SIZE); i++) {
			vm_map_shared_memory(AS_PROCESS(active_task)->stored_vas,
					(void*) (data + (i * BLOCK_SIZE)), // FIXME char to uint32_t issue
					KERNEL_VAS,
					(void*) (SHARED_MSG_MEMORY_START + (i * BLOCK_SIZE)),
					VM_PERM_USER_RW);
		}

		source = (uint32_t*) SHARED_MSG_MEMORY_START;
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
					(void*) (SHARED_MSG_MEMORY_START + (i * BLOCK_SIZE)));
		}

		vm_enable_vas(AS_PROCESS(active_task)->stored_vas);
	}

	return STATUS_OK;
}
/**
 * Yields a task so another task can run
 *
 * @return STATUS_OK or STATUS_FAIL
 */

// TODO shouldn't return anything; it makes no sense to do so
uint32_t sched_yield() {
	__sched_dispatch();
	return STATUS_OK;
}

/**
 * Force set the a task state so that when a task is resumed, it will
 * resume in set state. Invoked/referenced in hw_handlers.c.
 *
 * @param cb_handler tid
 * @param cb_handler jmp_buffer
 *
 * @return does not return a value;
 */

void sched_update_task_state(uint32_t tid, jmp_buf * jmp_buffer) {
	if (active_task) {
		sched_task * task = (sched_task*) hmap_get(all_tasks_map, tid);
		if (task) {
			active_task->jmp_buffer = *jmp_buffer;
		}
	}
}
