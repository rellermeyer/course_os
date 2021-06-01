#include <chipset.h>
#include <klibc.h>
#include <stdint.h>
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


// http://infocenter.arm.com/help/topic/com.arm.doc.ddi0183f/DDI0183.pdf
// uart flag register masks
// (FR)
#define RI   (1 << 8)  // Ring Indicator
#define TXFE (1 << 7)  // Transmit fifo empty
#define RXFF (1 << 6)  // Receive fifo full
#define TXFF (1 << 5)  // Transmit fifo full
#define RXFE (1 << 4)  // Receive fifo empty
#define BUSY (1 << 3)  // UART busy, set while uart is sending something
#define DCD  (1 << 2)  // Data carrier detect
#define DSR  (1 << 1)  // Data set ready
#define CTS  (1 << 0)  // Clear to send

// Uart interrupt masks. When a mask is set, this action causes an interrupt
// (IMSC)
#define OEIM   (1 << 10)  // Overrun error
#define BEIM   (1 << 9)   // Break error
#define PEIM   (1 << 8)   // Parity error
#define FEIM   (1 << 7)   // Framing error
#define RTIM   (1 << 6)   // Receive timeout
#define TXIM   (1 << 5)   // Transmit
#define RXIM   (1 << 4)   // Receive
#define DSRMIM (1 << 3)   // nUARTDSR Modem
#define DCDMIM (1 << 2)   // nUARTDCD Modem
#define CTCMIM (1 << 1)   // nUARTCTS Modem
#define RIMIM  (1 << 0)   // nUARTRI Modem

// (CR)
//#define CTSEn  ()    // CTS Hardware Control Enable




void pl011_uart_write_byte(volatile UartInterface * interface,
                     volatile uint8_t value) {
    //    while (interface->FR & TXFF ) {
    //        asm volatile ("nop");
    //    }
    while (interface->FR & (1 << 5))
        ;

    interface->DR = (uint32_t)value;
}


uint8_t pl011_uart_get_byte(volatile UartInterface *interface) {
    while (interface->FR & (1 << 4))
        ;
    return interface->DR;
}

char pl011_uart_getc(volatile UartDevice *device, int uartchannel) {
    return pl011_uart_get_byte(device->devices[uartchannel]);
}

void pl011_uart_putc(volatile UartDevice *device, char c, int uartchannel) {
    return pl011_uart_write_byte(device->devices[uartchannel], c);
}

void pl011_uart_on_message(volatile UartDevice *device,
                             UartCallback callback,
                             int uartchannel) {}
