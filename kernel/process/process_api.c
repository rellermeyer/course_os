#include "scheduler.h"
#include "klibc.h"

// Process / Scheduler API

uint32_t prcs_exec(uint32_t file_p, int niceness) {
    sched_task * task = sched_create_task(file_p, niceness);
    return sched_add_task(task);
}

// kill the process and its subprocesses
// can only be called if the process tree contains
// the request pid
uint32_t prcs_kill(uint32_t pid) {
    return sched_remove_task(pid);
}

// sends a message to a process (ipc)
uint32_t prcs_send_message(uint32_t dest_pid, uint32_t event, char * data, int len) {
    return sched_send_message(dest_pid, event, data, len);
}

// receives messages from processes
uint32_t prcs_register_message_callback_handler(sched_callback_handler cb_handler) {
    return sched_register_callback_handler(cb_handler);
}

// receives messages from processes
uint32_t prcs_deregister_message_callback_handler() {
    return sched_deregister_callback_handler();
}

// set the niceness of a process
uint32_t prcs_set_niceness(uint32_t pid) {
    return sched_set_niceness(pid);
}

uint32_t prcs_fork(); // under construction
