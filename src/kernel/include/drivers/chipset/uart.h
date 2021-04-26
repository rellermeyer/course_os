#ifndef _GENERIC_UARTH
#define _GENERIC_UARTH

#include <stdint.h>
#include <stdlib.h>

typedef void (*UartCallback)(char c);
typedef volatile struct UartInterface UartInterface;

typedef struct UartDevice {
    size_t length;
    // Amount of UART interfaces
    volatile UartInterface *devices[99];  // The devices

    // Methods to interface with UART devices
    // Prints a character to a uart channel.
    // The definition of a channel is pretty open to interpretation
    // but what must be guaranteed is that channel 0 is the default channel
    // and in normal operation should always just work. Other channels might not exist.
    // If a user requests to print on a non existent channel, the output must be printed on
    // channel 0.
    void (*putc)(volatile struct UartDevice *self, char c, int uartchannel);
    char (*getc)(volatile struct UartDevice *self, int uartchannel);

    // The callback given to this function is called any time a uart message
    // is received on the channel. The same rules apply as for puts. When a channel
    // is requested that does not exist, input from channel zero shall be given to this callbac
    void (*on_message)(volatile struct UartDevice *self, UartCallback callback, int uartchannel);
} UartDevice;
#endif
