#include <bcm2835.h>
#include <chipset.h>
#include <klibc.h>
#include <stdint.h>
#include <bcm2835_uart.h>
#include <pl011.h>

static UartInterface * BCM2835_UART0_ADDRESS;
static UartInterface * BCM2835_UART1_ADDRESS;
static UartInterface * BCM2835_UART2_ADDRESS;
static UartInterface * BCM2835_UART3_ADDRESS;

void bcm2835_uart_init(volatile UartDevice *device) {
    BCM2835_UART0_ADDRESS = (UartInterface *)(bcm2835_peripheral_base + 0x201000);
    BCM2835_UART1_ADDRESS = (UartInterface *)(bcm2835_peripheral_base + 0x202000);
    BCM2835_UART2_ADDRESS = (UartInterface *)(bcm2835_peripheral_base + 0x203000);
    BCM2835_UART3_ADDRESS = (UartInterface *)(bcm2835_peripheral_base + 0x204000);

    device->length  = 4;
    device->devices[0] = BCM2835_UART0_ADDRESS;
    device->devices[1] = BCM2835_UART1_ADDRESS;
    device->devices[2] = BCM2835_UART2_ADDRESS;
    device->devices[3] = BCM2835_UART3_ADDRESS;

    device->getc = pl011_uart_getc;
    device->putc = pl011_uart_putc;
    device->on_message = pl011_uart_on_message;


}
