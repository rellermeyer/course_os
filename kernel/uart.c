#include <stdint.h>
#include <mmap.h>

#define STD_IN_BUFFER_SIZE 4096

/* Contributors: David Denton, Michael Brennan */

enum {
/* UART inter: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0224i/I1042232.html */
PICINTENABLE = 0x10140010,
UART0INT_BIT = 0x0800,
UART1INT_BIT = 0x1000,
UART2INT_BIT = 0x2000
};


typedef volatile struct {
uint32_t uart0_inter_val = *PICINTENABLE | UART0INT_BIT;
uint32_t uart1_inter_val = *PICINTENABLE | UART1INT_BIT;
uint32_t uart2_inter_val = *PICINTENABLE | UART2INT_BIT;
} device_driver;

typedef volatile struct {
/* Register info: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0183g/I18381.html */
device_driver *dd;
 uint32_t UARTDR; 		// Data Register
 uint32_t UARTRSR_UARTECR; 	// Receive Status Register/Error Clear Register
 uint8_t reserved1[0x10];	// Reserved Space
 const uint32_t UARTFR; 	// Flag Register
 uint8_t reserved2[0x4];	// Reserved Space
 uint32_t UARTLPR; 		// Low-Power Counter Register
 uint32_t UARTIBRD; 		// Integer Baud Rate Register
 uint32_t UARTFBRD; 		// Fractional Baud Rate Register
 uint32_t UARTLCR_H; 		// Line Control Register
 uint32_t UARTCR; 		// Control Register
 uint32_t UARTIFLS; 		// Interrupt FIFO Level Select Register
 uint32_t UARTIMSC; 		// Interrupt Mask Set/Clear Register
 const uint32_t UARTRIS; 	// Raw Interrupt Status Register
 const uint32_t UARTMIS; 	// Masked Interrupt Status Register
 uint32_t UARTICR; 		// Interrupt Clear Register
 uint32_t UARTDMACR; 		// DMA Control Register
} uart;

// UARTFR reg. enums
enum {
RXFE = 0x10,
RXFF = 0x40, 
TXFF = 0x20,
TXFE = 0x80,
BUSY = 0x08,
};

// UARTCF reg. enums
enum {
TXE = 0x100,
RXE = 0x200
}

uart * const UART0 = (uart *)0x101f1000;
uart * const UART1 = (uart *)0x101f2000;
uart * const UART2 = (uart *)0x101f3000;

void print_uart0(const char *s) {
	while (uart->dd->uart0_inter_val == 0) {}
	if(uart.UARTCR & RXE > 0) 
	{
		while(*s != '\0') 
		{
			*UART0 = (uint32_t)(*s);
			s++;
		}
	}
}

/*  We need to implement a lock here.  klibc will be implementing the buffer
 *    we just need to ensure the FIFO isn't read out of order.
 */
char *read_uart0() 
{
uint32_t buffer[STD_IN_BUFFER_SIZE] = {0};
while (uart->dd->uart0_inter_val == 0) {}
uint32_t *iterator = buffer;
	do 
	{
		if(uart.UARTCR & TXE > 0) 
		{
			break;
		} 
		else 
		{
			*iterator = uart.UARTDR;
			iterator++;
		}
	} while (*iterator != '\0');
return buffer;
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

		if(a <= 9)
			*UART = (uint32_t)(a + (uint32_t )'0');
		else if(a <= 0xf)
			*UART = (uint32_t)((a - 0xa) + (uint32_t )'a');
		else
			*UART = (uint32_t)('?');
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

void print_char_uart0(char c) {
  *UART_ADDRESS = (uint32_t) c;
}
