#ifndef UART_BCM2835_H
#define UART_BCM2835_H

// Generic UART
#include "../../include/uart.h"


void bcm2835_uart_init(volatile UartDevice *device);

#endif
