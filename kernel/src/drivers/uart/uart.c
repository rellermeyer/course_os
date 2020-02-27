#include <uart.h>

volatile UartInterface * const UART0_ADDRESS = (volatile UartInterface *)0x101f1000;
volatile UartInterface * const UART1_ADDRESS = (volatile UartInterface *)0x101f2000;
volatile UartInterface * const UART2_ADDRESS = (volatile UartInterface *)0x101f3000;


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
    return 0;
}

void uart_write_byte(volatile UartInterface * interface, volatile uint8_t value) {
    interface->DR = (uint32_t)value;
}