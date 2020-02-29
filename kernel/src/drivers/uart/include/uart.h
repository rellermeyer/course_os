#ifndef UART_H
#define UART_H

#include <stdint.h>


// http://infocenter.arm.com/help/topic/com.arm.doc.ddi0183f/DDI0183.pdf
// Section 3.2: Summary of registers
// https://balau82.wordpress.com/2010/11/30/emulating-arm-pl011-serial-ports/
// This struct is memory mappend. I call them registers but they all live in memory.
typedef volatile struct UartInterface {
    uint32_t DR;                // Data Register
    uint32_t RSR_ECR;           // Receive Status Register / Error Clear Register
    uint8_t reserved1[0x10];    // Reserved
    const uint32_t FR;          // Flag Register
    uint8_t reserved2[0x4];     // Reserved
    uint32_t LPR;               // IrDA low power counter Register
    uint32_t IBRD;              // Integer Baud Rate Register
    uint32_t FBRD;              // Fractional Baud Rate Register
    uint32_t LCR_H;             // Line Control Register
    uint32_t CR;                // Control Register
    uint32_t IFLS;              // Interrupt FIFO Level Select Register
    uint32_t IMSC;              // Interrupt Mask Set/Clear Register
    const uint32_t RIS;         // Raw Interrupt Status Register
    const uint32_t MIS;         // Masked Interrupt Status Register
    uint32_t ICR;               // Interrupt Clear Register
    uint32_t DMACR;             // DMA Control Register
} UartInterface;

volatile UartInterface * const UART0_ADDRESS;
volatile UartInterface * const UART1_ADDRESS;
volatile UartInterface * const UART2_ADDRESS;

uint32_t uart_read_int(volatile UartInterface * interface);
void uart_write_int(volatile UartInterface * interface, volatile uint32_t value);
uint8_t uart_read_byte(volatile UartInterface * interface);
void uart_write_byte(volatile UartInterface * interface, volatile uint8_t value);

void uart0_puts(const char * s);
void uart0_putc(char c);
char uart0_getc();

void UART0_INTERRUPT_HANDLER();
void UART1_INTERRUPT_HANDLER();
void UART2_INTERRUPT_HANDLER();

// Called when the processor first boots
void uart_early_init();

// Called when interrupts have been enabled. Can be used to enable UART interrupts
void uart_late_init();

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
#endif // ifndef UART_H
