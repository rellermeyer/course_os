// Only compile this file when **not** compiling for a raspberry pi
#ifndef RASPBERRY_PI

#include <stdint.h>
#include <uart.h>
#include <interrupt.h>
#include <stdio.h>

// 0x3F200000 + 0x1000

/*
// raspberry pi 2 + 3
volatile UartInterface * const UART0_ADDRESS = (volatile UartInterface *)0x3f201000;
volatile UartInterface * const UART1_ADDRESS = (volatile UartInterface *)0x3f202000;
volatile UartInterface * const UART2_ADDRESS = (volatile UartInterface *)0x3f203000;

// raspberry pi zero, 1, b+ etc
volatile UartInterface * const UART0_ADDRESS = (volatile UartInterface *)0x20201000;
volatile UartInterface * const UART1_ADDRESS = (volatile UartInterface *)0x20202000;
volatile UartInterface * const UART2_ADDRESS = (volatile UartInterface *)0x20203000;

// raspberry pi 4:
volatile UartInterface * const UART0_ADDRESS = (volatile UartInterface *)0xfe201000;
volatile UartInterface * const UART1_ADDRESS = (volatile UartInterface *)0xfe202000;
volatile UartInterface * const UART2_ADDRESS = (volatile UartInterface *)0xfe203000;
 */
// VersatilePB
volatile UartInterface * const UART0_ADDRESS = (volatile UartInterface *)0x101f1000;
volatile UartInterface * const UART1_ADDRESS = (volatile UartInterface *)0x101f2000;
volatile UartInterface * const UART2_ADDRESS = (volatile UartInterface *)0x101f3000;



static inline void delay(int32_t count)
{
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
    : "=r"(count): [count]"0"(count) : "cc");
}

void set_baud_rate(volatile UartInterface * interface, uint32_t baud_rate) {
    const int UART_CLOCK_SPEED = 4000000;
    float divider = UART_CLOCK_SPEED / (16.0f * baud_rate);
    uint16_t integer_divider = (uint16_t)divider;
    uint8_t fractional_divider = ((divider - integer_divider) * 64) + 0.5;

    interface->IBRD = integer_divider;        // Integer baud rate divider
    interface->FBRD = fractional_divider;     // Fractional baud rate divider
};

void uart_early_init() {
    //Disable UART
    UART0_ADDRESS->CR = 0x00000000;

    #define GPPUD (GPIO_ADDRESS_PI + 0x94) //Controls pull up/down on GPIO pins
    #define GPPUDCLK0 (GPIO_ADDRESS_PI + 0x98) //GPIO Clo

    mmio_write(GPPUD, 0x00000000);
    delay(150);

    mmio_write(GPPUDCLK0, (1<<14) | (1<<15));
    delay(150);

    mmio_write(GPPUDCLK0,0x00000000);

    UART0_ADDRESS->ICR = 0x7ff;
    //Enable DMA memory reads & writes with UART
    //mmio_write(UARTDMACR, (1<<0) | (1<<1));

    //Enable FIFO and set word length to 8
    UART0_ADDRESS->LCR_H = (1<<4) | (1<<5) | (1<<6);

    set_baud_rate(UART0_ADDRESS, 115200);

    //Enable interrupts
    UART0_ADDRESS->IMSC = CTCMIM | RXIM | TXIM | RTIM | FEIM | PEIM | BEIM | OEIM;
    //Enable UART and UART transfer
    UART0_ADDRESS->CR = (1<<0) | (1<<8) | (1 << 9);


    kprintf("Baud rate: %i:%i\n", UART0_ADDRESS->IBRD, UART0_ADDRESS->FBRD);
}

void uart_late_init() {
    // Register the uart interrupt handler.
    //    register_interrupt_handler(UART0_IRQ, UART0_INTERRUPT_HANDLER)

    //    kprintf("uart: 0x%x\n", UART0_ADDRESS->IMSC);
    //    UART0_ADDRESS->IMSC = RXIM;
    //    kprintf("uart: 0x%x\n", UART0_ADDRESS->IMSC);
}



#endif // RASPBERRY_PI
