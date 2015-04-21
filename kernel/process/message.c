
typedef void (*callback_handler)(message);

void msg_send(message msg) {
	sched_post_message(to_pid, data, len);
}

void msg_register_callback_handler() {

}

void msg_deregister_callback_handler(callback_handler cb_handler) {

}

void msg_deregister_callback_handler() {

}
