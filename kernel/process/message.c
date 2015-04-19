
typedef void (*callback_handler)(uint32_t /* from_pid */, uint32_t /* event*/, uint32_t /**/, uint32_t);

void msg_send(uint32_t pid, uint32_t event, uint32_t data2) {

}

void msg_register_callback_handler() {

}

void msg_deregister_callback_handler(callback_handler cb_handler) {

}

void msg_deregister_callback_handler() {

}
