#include <stdint.h>
#include "drivers/uart.h"
#include "mmap.h" 

#define UARTDR (UART0_ADDRESS_PI+0x00) // Data Register
#define UARTRSR (UART0_ADDRESS_PI+0x04) //Recieve status register
#define UARTFR (UART0_ADDRESS_PI+0x18) // Flag Register
#define UARTILPR (UART0_ADDRESS_PI+0x20) //IrDA low power counter register
#define UARTIBRD (UART0_ADDRESS_PI+0x24) // Integer baud rate
#define UARTFBRD (UART0_ADDRESS_PI+0x28) //Fractional baud rate register
#define UARTLCR_H (UART0_ADDRESS_PI+0x2C) // Line control register
#define UARTCR (UART0_ADDRESS_PI+0x30) //Control Register
#define UARTIFLS (UART0_ADDRESS_PI+0x34) // Interrupt FIFO level select register
#define UARTIMSC (UART0_ADDRESS_PI+0x38) //Interrupt mask set/clear register
#define UARTRIS (UART0_ADDRESS_PI+0x3C) //Raw interrupt status register
#define UARTMIS (UART0_ADDRESS_PI+0x40) //masked interrupt status register
#define UARTICR (UART0_ADDRESS_PI+0x44) // Interrupt clear register
#define UARTDMACR (UART0_ADDRESS_PI+0x48) // DMA controlled register
#define GPPUD (GPIO_ADDRESS_PI + 0x94) //Controls pull up/down on GPIO pins
#define GPPUDCLK0 (GPIO_ADDRESS_PI + 0x98) //GPIO Clock


void init_uart()
{
	//Disable UART
	mmio_write(UARTCR, 0x00000000);

	mmio_write(GPPUD, 0x00000000);
	for(int i = 0;i<150;i++){
		asm volatile ("nop");
	}
	
	mmio_write(GPPUDCLK0, (1<<14) | (1<<15));
	for(int i = 0;i<150;i++){
		asm volatile ("nop");
	}
	
	mmio_write(GPPUDCLK0,0x00000000);

	mmio_write(UARTICR, 0x7FF);
	//Enable DMA memory reads & writes with UART
	//mmio_write(UARTDMACR, (1<<0) | (1<<1));

	//Enable FIFO and set word length to 8
	mmio_write(UARTLCR_H, (1<<4) | (1<<5) | (1<<6));
	//Set the baud transfer rate at 115200 
	mmio_write(UARTIBRD, 1);
	mmio_write(UARTFBRD, 40);

	//Enable interrupts
	mmio_write(UARTIMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));
	//Enable UART and UART transfer
	mmio_write(UARTCR, (1<<0) | (1<<8));
}

void print_char_uart0(char c) {
	//Wait for room in the FIFO queue
	while(mmio_read(UARTFR) & (1<<5)) {}
	mmio_write(UART0_ADDRESS_PI, c); //Changed
}

void print_uart0(const char *s) {
	while(*s != '\0') {
		while(mmio_read(UARTFR) & (1<<5)) {}

		print_char_uart0(*s); //Altering for testing purposes, don't kill me!
		s++;
	}
}



/* print the full 32 bits of a word at the given address */
/* trailing newline... */
void print_word_bits(uint32_t * c) {
	int i;
	for(i = 31; i >= 0; i--)
		*c & (1 << i) ? print_uart0("1") : print_uart0("0");
	print_uart0("\n");
}

/* print the full 8-digit hex code of a word at the given address */
/* no '0x' prefix, NO trailing newline */
void print_word_hex(uint32_t * c){
	int i;
	uint32_t a;
	for(i = 0x7; i >= 0x0; i--){
		a = *c & (0xf << (i*0x4));
		a >>= (i*0x4);
		while(mmio_read(UARTFR) & (1<<5)) {}
		if(a <= 9)
			mmio_write(UART0_ADDRESS_PI, (uint32_t)(a + (uint32_t )'0')); //Changed
		else if(a <= 0xf)
			mmio_write(UART0_ADDRESS_PI, (uint32_t)((a - 0xa) + (uint32_t )'a'));//Changed
		else
			mmio_write(UART0_ADDRESS_PI, (uint32_t)('?')); //Changed
	}
}

/* display memory at given address */
/* format is: "[address]: word1 word2 word3\n", etc. */
/* displays 30 words (10 lines) */
void md(uint32_t * start){
	int i, j;
	uint32_t *addr = start;
	for(i = 0; i < 10; i++) {
		print_uart0("0x"); print_word_hex((uint32_t *)&addr); print_uart0(": ");
		for(j = 0; j < 3; j++) {
			print_word_hex(addr);
			print_uart0("  ");
			addr++;
		}
		print_uart0("\n");
	}
}

/*void print_uart0(const char *s) {*/
/*while (uart->dd->uart0_inter_val == 0) {}*/
/*if(uart.UARTCR & RXE > 0) */
/*{*/
/*while(*s != '\0') */
/*{*/
/**UART0 = (uint32_t)(*s);*/
/*s++;*/
/*}*/
/*}*/
/*}*/

/*  We need to implement a lock here.  klibc will be implementing the buffer
 *    we just need to ensure the FIFO isn't read out of order.
 */
/*char *read_uart0() */
/*{*/
/*uint32_t buffer[STD_IN_BUFFER_SIZE] = {0};*/
/*while (uart->dd->uart0_inter_val == 0) {}*/
/*uint32_t *iterator = buffer;*/
/*do */
/*{*/
/*if(uart.UARTCR & TXE > 0) */
/*{*/
/*break;*/
/*} */
/*else */
/*{*/
/**iterator = uart.UARTDR;*/
/*iterator++;*/
/*}*/
/*} while (*iterator != '\0');*/
/*return buffer;*/
/*}*/

