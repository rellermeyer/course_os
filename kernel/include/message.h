/*
 * message.h
 *
 *  Created on: Apr 20, 2015
 *      Author: mwkurian
 */

#ifndef KERNEL_INCLUDE_MESSAGE_H_
#define KERNEL_INCLUDE_MESSAGE_H_

#include "klibc.h"

typedef void (*callback_handler)(message);

typedef struct message {
    uint32_t to_pid;
    char * data;
    int length;
} message;

STATUS msg_init();
STATUS msg_send(message msg);
STATUS msg_register_callback_handler(callback_handler cb_handler);
STATUS msg_deregister_callback_handler();

#endif /* KERNEL_INCLUDE_MESSAGE_H_ */
