#ifndef UART_BCM2836_H
#define UART_BCM2836_H

#include <uart.h>
#include <stdint.h>

void bcm2836_uart_init(volatile UartDevice *device);

#endif
