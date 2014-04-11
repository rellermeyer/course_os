#include <stdint.h>

volatile uint32_t *const TIMER_0_ADDRESS= (uint32_t *)0x101e2000;
volatile uint32_t *const TIMER_1_ADDRESS= (uint32_t *)0x101e2020;
volatile uint32_t *const TIMER_2_ADDRESS= (uint32_t *)0x101e3000;
volatile uint32_t *const TIMER_3_ADDRESS= (uint32_t *)0x101e3020;

void timer_start() {
  print_uart0("Timer driver loaded\n");
  *TIMER_1_ADDRESS = 1000;
  int i = 0;
  for(i; i< 10; i++){
    os_printf("Timer register = %x\n", *TIMER_1_ADDRESS);
  }

}
