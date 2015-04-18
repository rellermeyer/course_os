#include "../include/scheduler.h"

#define MAX_TASKS 100
#define MAX_ACTIVE_TASKS 4
#define MAX_NICENESS -20
#define MIN_NICENESS 20
#define TASK_STATE_FINSIHED 3
#define TASK_STATE_INACTIVE  2
#define TASK_STATE_ACTIVE 1
#define TASK_STATE_NONE 0
#define MAX(a, b) 	a > b ? a : b
#define MIN(a, b)   a < b ? a : b

static char * last_err;
static prq_handle all_tasks;
static prq_handle active_tasks;
static int ignore_interrupt;
static sched_task * active_task;

void sched_init() {
    os_printf("Initializing scheduler\n");
    last_err = "No error";
    init_queue(&all_tasks, MAX_TASKS);
    init_queue(&active_tasks, MAX_ACTIVE_TASKS);
    active_task = 0;
}

sched_task* sched_create_task(uint32_t* file_p, int niceness) {
    if (prq_size(&all_tasks) >= MAX_TASKS) {
        last_err = "Too many tasks";
        return 0;
    }

    ignore_interrupt = 1;

    // use the kernel memory
    if(active_task != 0){
        vm_use_kernel_vas();
    }

    pcb * pcb_pointer = process_create(file_p);

    if (!pcb_pointer) {
        last_err = "Process create error";
        ignore_interrupt = 0;
        return 0;
    }

    struct vas * vas_struct = vm_new_vas();
    sched_task * task = (sched_task*) kmalloc(sizeof(sched_task));

    // FIXME
    // Check this code @lkolbly
    // set up the vas
    // call vm_set_mapping(struct vas *vas, void *vptr, void *pptr, int permission);
    // not sure how to use it

    task->niceness = MAX(MAX_NICENESS, MIN(MIN_NICENESS, niceness));
    task->pcb = pcb_pointer;
    task->state = TASK_STATE_NONE;
    task->node = 0;
    task->vas_struct = vas_struct;

    if(active_task != 0){
        vm_enable_vas(active_task->vas_struct);
    }

    ignore_interrupt = 0;

    return task;

}

void sched_interrupt_handler() {
    if (ignore_interrupt) {
        return;
    }

    ignore_interrupt = 1; // prevent interrupts while handling another interrupt

    if (prq_size(&active_tasks) < MAX_ACTIVE_TASKS) {
        if (prq_size(&all_tasks) > 0) {
            prq_node * node = prq_dequeue(&all_tasks);
            sched_task * task = (sched*) node->data;
            prq_enqueue(&active_tasks, task, task->niceness); // add to active_tasks if the task
        }
    }

    if (prq_size(&active_tasks) == 0) {
        return;
    }

    sched_task * task;

    // check if there is active task
    if (active_task == 0) {
        task = active_task;
    } else {
        prq_node node = prq_peek(&active_tasks);
        task = (sched*) node->data;
    }

    switch (task->state) {
        case TASK_STATE_NONE:
        case TASK_STATE_INACTIVE: 				      // start running task->pcb
            pcb * pcb_pointer = task->pcb;
            task->state = TASK_STATE_ACTIVE;	  // mark task process as active
            active_task = task;					  // set the active_task pointer
            ignore_interrupt = 0;				        // allow interrupts
            execute_process(&task->pcb);   // start running the process function
            ignore_interrupt = 1;				        // disallow interrupts
            task->state = TASK_STATE_FINSIHED;	        // process completed
            free_pcb(&task->pcb);						// free process memory
            prq_dequeue(&active_tasks, *task->node); // remove it from the queue

            // NOTE next interrupt will get the start the process

            break;
        case TASK_STATE_ACTIVE: 							// sleep task->pcb
            if (prq_size(&active_tasks) > 1) { // check if there at least one another task in the queue
                prq_node node = prq_dequeue(&active_tasks, active_task->node); // remove the running task from queue
                prq_enqueue(&active_tasks, node);
                pcb * pcb_pointer = task->pcb;
                save_process_state(pcb_pointer->PID); // save the current state of the process
                load_process_state(pcb_pointer->PID);
            }

            break;
    }

    ignore_interrupt = 0;
}

uint32_t sched_add_task(sched_task * task) {
    if (task) {
        if (task->state == TASK_STATE_NONE) {
            last_err = "Reusing task object not allowed";
            return 0;
        }

        task->node = prq_enqueue(&all_tasks, task, task->niceness);
        return 1;
    }

    last_err = "Invalid sched_task pointer";
    return 0;
}

uint32_t sched_remove_task(sched_task * task) {
    return 1;
}

const char *
sched_last_err() {
    return last_err;
}
