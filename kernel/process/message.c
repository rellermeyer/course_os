#include "../include/message.h"
#include "../include/klibc.h"

static hmap_handle * pid_handlers;

STATUS msg_init() {
    pid_handlers = hmap_create(pid_handlers);
    return STATUS_OK;
}

STATUS msg_send(message msg) {
    return sched_post_message(msg.to_pid, msg.data, msg.length);
}

STATUS msg_register_callback_handler(callback_handler cb_handler) {
    hmap_put(pid_handlers, sched_get_active_pid(), cb_handler);
    return STATUS_OK;
}

STATUS msg_deregister_callback_handler() {
    if (hmap_contains(pid_handlers, sched_get_active_pid())) {
        hmap_remove(pid_handlers, sched_get_active_pid());
    }

    return STATUS_OK;
}
