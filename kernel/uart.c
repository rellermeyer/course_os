#include <stdint.h>
#include <mmap.h>

volatile uint32_t *const UART = UART_ADDRESS;
 
void print_uart0(const char *s) {
   while(*s != '\0') { 
      *UART = (uint32_t)(*s); 
      s++;
   }
}

/* print the full 32 bits of a word a the given address */
/* NB: (right now) this the absolute address! */
void print_word_bits(uint32_t * c) {
	int i;
	for(i = 31; i >= 0; i--)
		*c & (1 << i) ? print_uart0("1") : print_uart0("0");
	print_uart0("\n");
}

/* print a hexadecimal representation of a word a the given address */
/* NB: this is the absolute address */
void print_word_hex(uint32_t * c){
	int i;
	print_uart0("0x");
	for(i = 7; i >= 0; i--){
		int a = *c & (0xF << (i*4));
		a >>= (i*4);

		// code was crashing when I tried to do this w/o a giant switch
		switch(a){
			case 0:
				print_uart0("0");
				break;
			case 1:
				print_uart0("1");
				break;
			case 2:
				print_uart0("2");
				break;
			case 3:
				print_uart0("3");
				break;
			case 4:
				print_uart0("4");
				break;
			case 5:
				print_uart0("5");
				break;
			case 6:
				print_uart0("6");
				break;
			case 7:
				print_uart0("7");
				break;
			case 8:
				print_uart0("8");
				break;
			case 9:
				print_uart0("9");
				break;
			case 0xa:
				print_uart0("a");
				break;
			case 0xb:
				print_uart0("b");
				break;
			case 0xc:
				print_uart0("c");
				break;
			case 0xd:
				print_uart0("d");
				break;
			case 0xe:
				print_uart0("e");
				break;
			case 0xf:
				print_uart0("f");
				break;
			defaut:
				print_uart0("?");
				break;
		}
	}
	print_uart0("\n");
}
			 
