#include <uart.h>
#include <interrupt.h>

void UART0_INTERRUPT_HANDLER() {
//    UART_ClearITPendingBit(UART0, UART_IT_Receive);
//    UART_ClearITPendingBit(UART0,  UART_IT_Transmit);
//    while(UART_GetFlagStatus(UART0, UART_FLAG_TxFIFOFull) != RESET);
//    UART_SendData(UART0, UART_ReceiveData(UART0));
//    VIC1->VAR=0xFF;

}

uint32_t uart_read_int(volatile UartInterface * interface) {
    return 0;
}

void uart_write_int(volatile UartInterface * interface, volatile uint32_t value) {
    interface->DR = value;
}

uint8_t uart_read_byte(volatile UartInterface * interface) {
    // Wait for UART to have received something.
    while ( interface->FR & RXFE ) {
        asm volatile ("nop");
    }
    return interface->DR;
}

void uart_write_byte(volatile UartInterface * interface, volatile uint8_t value) {
    while (interface->FR & TXFF ) {
        asm volatile ("nop");
    }
    interface->DR = (uint32_t)value;
}

char uart0_getc() {
    return uart_read_byte(UART0_ADDRESS);
}

void uart0_puts(const char *s) {
    while (*s != '\0') {
        uart0_putc(*s);
        s++;
    }
}

void uart0_putc(char c) {
    uart_write_byte(UART0_ADDRESS, c);
}
