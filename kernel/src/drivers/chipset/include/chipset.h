#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdbool.h>
#include <stdint.h>

/*
 * Abstract chipset driver interface
 */

void init_chipset();
void uart_putc(char c, int channel);

// A timer handle is an identifier for a timer so it can be descheduled later.
// These handles must be unique.
typedef size_t TimerHandle;
typedef void (*TimerCallback)();

typedef void (*UartCallback)(char c);

typedef struct ChipsetInterface {
    /// Timer Functions
    TimerHandle (*schedule_timer_periodic)(TimerCallback callback, uint32_t ms);
    TimerHandle (*schedule_timer_once)(TimerCallback callback, uint32_t ms);
    void (*deschedule_timer)(TimerHandle handle);

    /// UART Functions
    // Prints a character to a uart channel.
    // The definition of a channel is pretty open to interpretation
    // but what must be guaranteed is that channel 0 is the default channel
    // and in normal operation should always just work. Other channels might not exist.
    // If a user requests to print on a non existent channel, the output must be printed on
    // channel 0.
    void (*uart_putc)(char c, int uartchannel);

    // The callback given to this function is called any time a uart message
    // is received on the channel. The same rules apply as for puts. When a channel
    // is requested that does not exist, input from channel zero shall be given to this callback.
    void (*uart_on_message)(UartCallback callback, int uartchannel);

    void (*handle_irq)();
    void (*handle_fiq)();

    // Called for every chipset after interrupts and dynamic memory has been enabled
    // So the chipset can do some more initialization.
    void (*late_init)();
} ChipsetInterface;

ChipsetInterface chipset;

#endif
