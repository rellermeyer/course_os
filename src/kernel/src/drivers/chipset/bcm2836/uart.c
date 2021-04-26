#include <bcm2836.h>
#include <chipset.h>
#include <klibc.h>
#include <stdint.h>
#include <bcm2836_uart.h>
#include <pl011.h>

/*
// raspberry pi zero, 1, b+ etc
volatile UartInterface * const UART0_ADDRESS = (volatile UartInterface *)0x20201000;
volatile UartInterface * const UART1_ADDRESS = (volatile UartInterface *)0x20202000;
volatile UartInterface * const UART2_ADDRESS = (volatile UartInterface *)0x20203000;

// raspberry pi 4:
volatile UartInterface * const UART0_ADDRESS = (volatile UartInterface *)0xfe201000;
volatile UartInterface * const UART1_ADDRESS = (volatile UartInterface *)0xfe202000;
volatile UartInterface * const UART2_ADDRESS = (volatile UartInterface *)0xfe203000;
 VersatilePB
volatile UartInterface * const UART0_ADDRESS = (volatile UartInterface *)0x101f1000;
volatile UartInterface * const UART1_ADDRESS = (volatile UartInterface *)0x101f2000;
volatile UartInterface * const UART2_ADDRESS = (volatile UartInterface *)0x101f3000;
*/

static UartInterface * BCM2836_UART0_ADDRESS;
static UartInterface * BCM2836_UART1_ADDRESS;
static UartInterface * BCM2836_UART2_ADDRESS;
static UartInterface * BCM2836_UART3_ADDRESS;

void bcm2836_uart_init(volatile UartDevice *device) {
    BCM2836_UART0_ADDRESS = (UartInterface *)(bcm2836_peripheral_base + 0x201000);
    BCM2836_UART1_ADDRESS = (UartInterface *)(bcm2836_peripheral_base + 0x202000);
    BCM2836_UART2_ADDRESS = (UartInterface *)(bcm2836_peripheral_base + 0x203000);
    BCM2836_UART3_ADDRESS = (UartInterface *)(bcm2836_peripheral_base + 0x204000);

    device->length  = 4;
    device->devices[0] = BCM2836_UART0_ADDRESS;
    device->devices[1] = BCM2836_UART1_ADDRESS;
    device->devices[2] = BCM2836_UART2_ADDRESS;
    device->devices[3] = BCM2836_UART3_ADDRESS;

    device->getc = pl011_uart_getc;
    device->putc = pl011_uart_putc;
    device->on_message = pl011_uart_on_message;


}
