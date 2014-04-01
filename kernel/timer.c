#include <stdint.h>

volatile uint32_t *const TIMER_1_ADDRESS= (uint32_t *)0x101e2000;
volatile uint32_t *const TIMER_2_ADDRESS= (uint32_t *)0x101e3000;

void timer_start() {
  print_uart0("Timer driver loaded\n");
}
