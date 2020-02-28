#ifndef UART_H
#define UART_H

#include <stdint.h>

void print_uart0(const char *);


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

void UART0_INTERRUPT_HANDLER();
void UART1_INTERRUPT_HANDLER();
void UART2_INTERRUPT_HANDLER();

void uart_init();

#endif // ifndef UART_H
