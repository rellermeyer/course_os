#include <stdint.h>
#include <mmap.h>
#include "include/klibc.h"
#include <stdarg.h>
#include <stdint.h>

#define LOWER_CASE 0
#define UPPER_CASE 1
#define NO_CASE 2

static char lower_case_digits[16] = "0123456789abcdef";
static char upper_case_digits[16] = "0123456789ABCDEF";

#define UARTV_ADDRESS	(uint32_t *const) 0x801f1000	/* UART  0 virtual base address */
volatile uint32_t *const VUART = UARTV_ADDRESS;


/* display memory at given address */
/* format is: "[address]: word1 word2 word3\n", etc. */
/* displays 30 words (10 lines) */
void v_md(uint32_t * start){
	int i, j;
	uint32_t *addr = start;
	for(i = 0; i < 10; i++) {
		print_vuart0("0x"); print_word_hex((uint32_t *)&addr); print_vuart0(": ");
		for(j = 0; j < 3; j++) {
			print_word_hex(addr);
			print_vuart0("  ");
			addr++;
		}
		print_vuart0("\n");
	}
}

/* Call this function after MMU is enabled to use UART0 */
void print_vuart0(const char *s) {
   //[BUG?]:
   //Arg copied in r0 after function prologue
   //but finds it from .rodata at low memory
   //which is eventually unmapped so doesn't print
   //fixed by adding high kernel offset to s
   //s = s + 0xf0000000;
   while(*s != '\0') { 
      *VUART = (uint32_t)(*s); 
      s++;
   }
}


void print_char_vuart0(char c) {
  *UARTV_ADDRESS = (uint32_t) c;
}


void print_hex_v(int val, int CASE)
{
  int temp = val;
  int count_digits = 0;
  char buf[100];
  int CHAR_MASK = 0xF;
  if (temp == 0){
    //printf("0");
    print_char_vuart0('0');
  } 
  while((temp != 0) && (count_digits < 8))
  {
    int index = temp & CHAR_MASK;
    if(CASE == UPPER_CASE){
      buf[count_digits] = upper_case_digits[index];
    } else {
      buf[count_digits] = lower_case_digits[index];
    }
    temp = temp >> 4;
    count_digits += 1;
  }
  while(count_digits > 0){
    //printf("%c", buf[count_digits-1]);
    print_char_vuart0(buf[count_digits - 1]);
    count_digits--;
  }
}

print_dec_v(int val){
  int temp = val;
  int count_digits = 0;
  char buf[100];
  if (temp == 0){
    //printf("0");
    print_vuart0("0");
  } else if (temp < 0) {
    //printf("-");
    print_vuart0("-");
    temp = -temp;
  }
  while(temp != 0){
    int index = temp % 10;
    buf[count_digits] = upper_case_digits[index];
    temp = temp / 10;
    count_digits += 1;
  }
  while(count_digits > 0){
    //printf("%c", buf[count_digits-1]);
    print_char_vuart0(buf[count_digits - 1]);
    count_digits--;
  }
}

int v_printf(const char *str_buf, ...) {
  va_list args;
  int t_arg;
  va_start(args, str_buf);
  int count_args_found = 0;
  while(*str_buf != '\0') {
    if(*str_buf == '%') {
      str_buf++;
      switch (*str_buf) {
        case 'X':
          t_arg = va_arg(args,int);
          print_hex_v(t_arg, UPPER_CASE);
          break;
        case 'x':
          t_arg = va_arg(args,int);
          print_hex_v(t_arg, LOWER_CASE);
          break;
        case 'd':
          t_arg = va_arg(args,int);
          print_dec_v(t_arg);
          break;
        case 'c':
          t_arg = va_arg(args,int);
          print_char_vuart0(t_arg);
          //printf("%c", t_arg);
          break;
        case '%':
          print_vuart0("%");
          //printf("%%");
          break;
      }
    } else {
      print_char_vuart0(*str_buf);
      //printf("%c", *str_buf);
    }
    str_buf++;
  }
  va_end(args);
}

void print_regs(void){

  int pc, lr, sp, fp;
  asm volatile("mov %0, pc" : "=r" (pc));
  asm volatile("mov %0, lr" : "=r" (lr));
  asm volatile("mov %0, sp" : "=r" (sp));
  asm volatile("mov %0, fp" : "=r" (fp));
  v_printf("pc=%x, lr=%x, sp=%x, fp=%x\n", pc, lr, sp, fp);  
}