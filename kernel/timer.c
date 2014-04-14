#include <stdint.h>
#include "include/timer.h"

void initialize_timers(){
  print_uart0("initializing timers\n");
  volatile rasp_pi_timer *TIMER_0 = (rasp_pi_timer *)0x101e2000; 
  volatile rasp_pi_timer *TIMER_1 = (rasp_pi_timer *)0x101e2020; 
  volatile rasp_pi_timer *TIMER_2 = (rasp_pi_timer *)0x101e3000; 
  volatile rasp_pi_timer *TIMER_3 = (rasp_pi_timer *)0x101e3020; 
  timer_pointers[0] = (rasp_pi_timer *)TIMER_0;
  timer_pointers[1] = TIMER_1;
  timer_pointers[2] = TIMER_2;
  timer_pointers[3] = TIMER_3;
}

void timer_start() {
  print_uart0("Timer driver loaded\n");
  timer_pointers[0]->timer_load_value = 1000;
  os_printf("control address:%x\n", &(timer_pointers[0]->control));
  timer_pointers[0]->control |= 0x1;
  os_printf("control value:%x\n", timer_pointers[0]->control);
  int i = 0;
  for(i; i< 10; i++){
    os_printf("Timer register = %x\n", timer_pointers[0]->timer_actual_value);
  }
}
