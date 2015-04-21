#include "scheduler.h"
#include "message.h"
#include "klibc.h"

// Process / Scheduler API

STATUS prcs_exec(uint32_t file_p, int niceness) {
    sched_task * task = sched_create_task(file_p, niceness);
    return sched_add_task(task);
}

// kill the process and its subprocesses
// can only be called if the process tree contains
// the request pid
STATUS prcs_kill(uint32_t pid) {
    return sched_remove_task(pid);
}

// sends a message to a process (ipc)
STATUS prcs_send_message(message msg) {
    return sched_send_message(msg);
}

// receives messages from processes
STATUS prcs_register_message_callback_handler(callback_handler cb_handler) {
    return sched_register_callback_handler(cb_handler);
}

// receives messages from processes
STATUS prcs_deregister_message_callback_handler() {
    return sched_deregister_callback_handler();
}

// set the niceness of a process
STATUS prcs_set_niceness(uint32_t pid) {
    return sched_set_niceness(pid);
}

STATUS prcs_fork(); // under construction
