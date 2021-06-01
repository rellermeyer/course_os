#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdbool.h>
#include <stdint.h>
#include "uart.h"

/*
 * Abstract chipset driver interface
 */

void init_chipset();
void uart_putc(char c, int channel);

// A timer handle is an identifier for a timer so it can be descheduled later.
// These handles must be unique.
typedef size_t TimerHandle;
typedef void (*TimerCallback)();

typedef struct ChipsetInterface {
    /// Timer Functions
    TimerHandle (*schedule_timer_periodic)(TimerCallback callback, uint32_t ms);
    TimerHandle (*schedule_timer_once)(TimerCallback callback, uint32_t ms);
    void (*deschedule_timer)(TimerHandle handle);

    volatile UartDevice *uart;
    void (*handle_irq)();
    void (*handle_fiq)();

    // Called for every chipset after interrupts and dynamic memory has been enabled
    // So the chipset can do some more initialization.
    void (*late_init)();
} ChipsetInterface;

extern ChipsetInterface chipset;

#endif
