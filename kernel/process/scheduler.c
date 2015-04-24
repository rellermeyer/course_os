#include "../include/scheduler.h"
#include "../include/vm.h"
#include "../include/klibc.h"
#include "../include/process.h"
#include "../include/linked_list.h"

#define MAX_TASKS 100   // in the future, cap will be removed
#define MAX_ACTIVE_TASKS 4  // in the future, will dynamically change based on load
#define MAX_NICENESS -20
#define MIN_NICENESS 20
#define TASK_STATE_FINISHED 3   // task has run by the scheduler and finished
#define TASK_STATE_INACTIVE  2  // task is in the wait queue (about to be executed)
#define TASK_STATE_ACTIVE 1     // task is part of the running tasks; it is being interleaved and executed atm
#define TASK_STATE_NONE 0       // task is just created (no real state)
#define SAFE_NICE(n) MAX(MIN(MAX_NICENESS, n), n)
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
void __sched_interrupt_handler();
sched_task* __sched_find_subtask(sched_task * parent_task, uint32_t pid);
uint32_t __sched_remove_task(sched_task * task);

// Initialize the scheduler. Should be called by the kernel ONLY
uint32_t sched_init() {
    vm_enable_kernel_vas();

    os_printf("Initializing scheduler\n");
    last_err = "No error";
    inactive_tasks = prq_create_fixed(MAX_TASKS);
    active_tasks = prq_create_fixed(MAX_ACTIVE_TASKS);
    active_task = 0;

    _sched_register_timer_irq();

    return STATUS_OK;
}

uint32_t sched_register_callback_handler(callback_handler cb_handler) {
    sched_task * task = hmap_get(all_tasks_map, sched_get_active_pid());
    task->cb_handler = cb_handler;
    return STATUS_OK;
}

uint32_t sched_deregister_callback_handler() {
    sched_task * task = hmap_get(all_tasks_map, sched_get_active_pid());
    task->cb_handler = 0;

    return STATUS_OK;
}

// Free the resources used the scheduler
uint32_t sched_free() {
    vm_enable_kernel_vas();

    // FIXME kill active tasks

    __sched_deregister_timer_irq();

    prq_free(inactive_tasks);
    prq_free(active_tasks);

    return STATUS_OK;
}

// Create a task which can be scheduled. Function is similar to CreateProcess and
// not to be confused with StartProcess
// NOTE file_p must be created by the kernel and is located in the kernel_vas
sched_task* sched_create_task(uint32_t* file_p, int niceness) {
    if (prq_count(inactive_tasks) >= MAX_TASKS) {
        last_err = "Too many tasks";
        return STATUS_FAIL;
    }

    __sched_pause_timer_irq();

    // use the kernel memory
    vm_use_kernel_vas();

    pcb * pcb_pointer = process_create(file_p);

    if (!pcb_pointer) {
        last_err = "Process create error";
        __sched_resume_timer_irq();
        return STATUS_FAIL;
    }

    struct vas * vas_struct = vm_new_vas();
    sched_task * task = (sched_task*) kmalloc(sizeof(sched_task));

    // FIXME
    // Check this code @lkolbly
    // set up the vas
    // call vm_set_mapping(struct vas *vas, void *vptr, void *pptr, int permission);
    // not sure how to use it

    niceness = MAX(MIN(MAX_NICENESS, niceness), niceness);

    task->niceness = niceness;
    task->pcb = pcb_pointer;
    task->state = TASK_STATE_NONE;
    task->node = 0;
    task->vas_struct = vas_struct;
    task->parent_pid = 0;
    task->children_pids = arrl_create();
    task->cb_handler = 0;

    if (active_task) {
        task->parent_pid = active_task->pcb->PID;
        arrl_add(active_task->children_pids, task->pcb->PID);
        vm_enable_vas(active_task->vas_struct);
    }

    __sched_resume_timer_irq();

    return task;
}

// Helper function used by the scheduler internally. It will traverse the parent/child
// list to find a subtask.
sched_task* __sched_find_subtask(sched_task * parent_task, uint32_t pid) {
    if (parent_task && parent_task->pcb->PID == pid) {
        return parent_task;
    }

    int i = 0;
    for (; i < arrl_count(parent_task->children_pids); i++) {
        uint32_t child_pid = arrl_count(parent_task, i);
        sched_task * child_task = hmap_get(all_tasks_map, child_pid);
        if ((child_task = __sched_contains_pid(child_task, pid))) {
            return child_task;
        }
    }

    return 0;
}

//
// NOTE expecting access to kernel global vars
void sched_waitpid(uint32_t pid) {
    while (1) {
        sched_task * task = (sched_task*) hmap_get(&all_tasks_map, pid);
        if (task == 0 || task->state == TASK_STATE_FINISHED) {
            break;
        }
        sleep(500);
    }
}

void __sched_interrupt_handler() {
    // prevent interrupts while handling another interrupt
    __sched_pause_timer_irq();

    // use the kernel memory
    vm_use_kernel_vas();

    if (prq_count(active_tasks) < MAX_ACTIVE_TASKS) {
        if (prq_count(inactive_tasks) > 0) {
            prq_node * node = prq_dequeue(inactive_tasks);
            prq_enqueue(active_tasks, node); // add to active_tasks if the task
            prq_free_node(active_task->node);
        }
    }

    if (prq_count(active_tasks) == 0) {
        __sched_resume_timer_irq();
        return;
    }

    sched_task * task;

    // check if there is active task
    if (active_task) {
        task = active_task;
    } else {
        prq_node * node = prq_peek(active_tasks);
        task = (sched_task*) node->data;
    }

    switch (task->state) {
        case TASK_STATE_INACTIVE: 				      // start running task->pcb
            active_task = task;                   // set the active_task pointer
            active_task->state = TASK_STATE_ACTIVE; // mark task process as active
            vm_enable_vas(active_task->vas_struct);
            __sched_resume_timer_irq();             // allow interrupts
            execute_process_no_vas(active_task->pcb); // start running the process function
            sched_remove_task(active_task);
            active_task = 0;
            // NOTE next interrupt will get the start the process

            break;
            // switch tasks; active task will go to sleep and let another task proceed
        case TASK_STATE_ACTIVE:
            if (prq_count(active_tasks) > 1) { // check if there at least one another task in the queue
                prq_remove(active_tasks, active_task->node); // remove the running task from queue
                prq_enqueue(active_tasks, active_task->node); // add it again to see if its position in the queue
                sched_task * next = (sched_task*) prq_peek(active_tasks)->data;

                if (active_task == next) {
                    vm_enable_vas(active_task->vas_struct);
                    break;
                }

                active_task = next;
                save_process_state(task->pcb->PID); // save the current state of the process
                vm_enable_vas(active_task->vas_struct);

                // issue messages before the process starts
                // works only in a single-threaded environment
                if (active_task->cb_handler) {
                    sched_message_chunk * chunk;
                    while ((chunk = llist_dequeue(active_task)) != 0) {
                        active_task->cb_handler(chunk->src_pid, chunk->event,
                                chunk->data, chunk->chunk_length,
                                chunk->remain_length);
                        if (chunk) {
                            kfree(chunk);
                        }
                    }
                }

                load_process_state(active_task->pcb->PID); // continue with the next process
            }

            break;
    }

    __sched_resume_timer_irq();
}

// start process
uint32_t sched_add_task(sched_task * task) {
    if (task) {
        if (task->state == TASK_STATE_NONE) {
            last_err = "Reusing task object not allowed";
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

        hmap_put(active_task->pcb->PID, active_task);

        if (active_task) {
            vm_enable_vas(active_task->vas_struct);
        }

        return active_task->pcb->PID;
    }

    last_err = "Invalid sched_task pointer";
    return STATUS_FAIL;
}

// contract
// --------
// must disable timer_interrupt
uint32_t __sched_remove_task(sched_task * task) {
    if (task != 0) {
        switch (task->state) {
            // FIXME Not sure if it is recommended to remove the task even before it
            // runs
            case TASK_STATE_INACTIVE:
                prq_remove(&inactive_tasks, task->node);
                prq_free_node(task->node);
                __sched_resume_timer_irq();
                break;
            case TASK_STATE_ACTIVE:
                vm_use_kernel_vas();
                task->state = TASK_STATE_FINISHED;   // process completed
                hmap_remove(task->pcb->PID);
                free_PCB(task->pcb);               // free process memory
                prq_remove(active_tasks, task->node); // remove it from the queue
                prq_free_node(task->node);

                // remove children
                int i = 0;
                for (; i < arrl_count(task->children_pids); i++) {
                    sched_remove_task(arrl_get(task->children_pids, 1)); // not to be confused with __sched_remove_task
                }
                break;
            case TASK_STATE_FINISHED:
                break;
        }

        return task->state;
    }

    return STATUS_FAIL;
}

// essentially a kill process
uint32_t sched_remove_task(uint32_t * pid) {
    sched_task * task = (sched_task*) hmap_get(&all_tasks_map, pid);
    if (task) {
        __sched_pause_timer_irq();
        uint32_t status = __sched_remove_task(task);
        __sched_resume_timer_irq();
        return status;
    }

    return STATUS_FAIL;
}

// get the current process id
uint32_t sched_get_active_pid() {
    if (active_task) {
        return active_task->pcb->PID;
    }

    return STATUS_FAIL;
}

const char * sched_last_err() {
    return last_err;
}

uint32_t sched_set_niceness(uint32_t pid, uint32_t niceness) {

    sched_task * task;

    if (!(task = hmap_get(&all_tasks_map, pid))) {
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
    sched_task * dest_task;

    if (!(dest_task = hmap_get(&all_tasks_map, dest_pid))) {
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
        vm_enable_vas(active_task->vas_struct);
        // copy to stack
        os_memcpy(data, data_cpy, cpy_len);
        // remaining bytes
        len -= cpy_len;
        // increment data pointer
        data = (char*) (data + cpy_len / 4);
        // get the dest_task
        sched_task * dest_task = hmap_get(&all_tasks_map, dest_pid);
        // switch to dest vas
        vm_enable_vas(dest_task->vas_struct);
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
        vm_enable_vas(active_task->vas_struct);
    }

    __sched_resume_timer_irq();

    return STATUS_OK;
}
