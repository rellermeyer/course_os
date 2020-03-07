#include <uart.h>
#include <bcm2836.h>
#include <chipset.h>
#include <interrupt.h>
#include <klibc.h>
#include <limits.h>
#include <stdint.h>

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
#define RI     (1<<8)  // Ring Indicator
#define TXFE   (1<<7)  // Transmit fifo empty
#define RXFF   (1<<6)  // Receive fifo full
#define TXFF   (1<<5)  // Transmit fifo full
#define RXFE   (1<<4)  // Receive fifo empty
#define BUSY   (1<<3)  // UART busy, set while uart is sending something
#define DCD    (1<<2)  // Data carrier detect
#define DSR    (1<<1)  // Data set ready
#define CTS    (1<<0)  // Clear to send

// Uart interrupt masks. When a mask is set, this action causes an interrupt
// (IMSC)
#define OEIM   (1<<10) // Overrun error
#define BEIM   (1<<9)  // Break error
#define PEIM   (1<<8)  // Parity error
#define FEIM   (1<<7)  // Framing error
#define RTIM   (1<<6)  // Receive timeout
#define TXIM   (1<<5)  // Transmit
#define RXIM   (1<<4)  // Receive
#define DSRMIM (1<<3)  // nUARTDSR Modem
#define DCDMIM (1<<2)  // nUARTDCD Modem
#define CTCMIM (1<<1)  // nUARTCTS Modem
#define RIMIM  (1<<0)  // nUARTRI Modem

// (CR)
//#define CTSEn  ()    // CTS Hardware Control Enable


static BCM2836UartInterface* BCM2836_UART0_ADDRESS;
static BCM2836UartInterface* BCM2836_UART1_ADDRESS;
static BCM2836UartInterface* BCM2836_UART2_ADDRESS;
static BCM2836UartInterface* BCM2836_UART3_ADDRESS;

void bcm2836_uart_init()
{
    BCM2836_UART0_ADDRESS = (BCM2836UartInterface*)(BCM2836_peripheral_base + 0x201000);
    BCM2836_UART1_ADDRESS = (BCM2836UartInterface*)(BCM2836_peripheral_base + 0x202000);
    BCM2836_UART2_ADDRESS = (BCM2836UartInterface*)(BCM2836_peripheral_base + 0x203000);
    BCM2836_UART3_ADDRESS = (BCM2836UartInterface*)(BCM2836_peripheral_base + 0x204000);
}

void uart_write_byte(BCM2836UartInterface* interface, volatile uint8_t value)
{
//    while (interface->FR & TXFF ) {
//        asm volatile ("nop");
//    }
    interface->DR = (uint32_t)value;
}

void bcm2836_uart_putc(char c, int uartchannel)
{
    if (uartchannel >= 4) {
        uartchannel = 0;
    }

    switch (uartchannel) {
        case 0: return uart_write_byte(BCM2836_UART0_ADDRESS, c);
        case 1: return uart_write_byte(BCM2836_UART1_ADDRESS, c);
        case 2: return uart_write_byte(BCM2836_UART2_ADDRESS, c);
        case 3: return uart_write_byte(BCM2836_UART3_ADDRESS, c);
        default: panic();
    }
}

void bcm2836_uart_on_message(UartCallback callback, int uartchannel) {}
