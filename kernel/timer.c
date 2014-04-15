#include <stdint.h>
#include "include/timer.h"

/* initializes timers as an array. Call this before
 * using any of the timer functions */
void initialize_timers(){
  print_uart0("initializing timers\n");
  volatile rasp_pi_timer *TIMER_0 = (rasp_pi_timer *)0x101e2000; 
  volatile rasp_pi_timer *TIMER_1 = (rasp_pi_timer *)0x101e2020; 
  volatile rasp_pi_timer *TIMER_2 = (rasp_pi_timer *)0x101e3000; 
  volatile rasp_pi_timer *TIMER_3 = (rasp_pi_timer *)0x101e3020; 
  timer_pointers[0] = TIMER_0;
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

/* This function sets the value that the timer will begin at.
 * This operation also resets the timer to this new value.
 * if you want to keep the timer running with its current state
 * use set background load. */
int set_load_value(int timer_index, int value){
  if(timer_index < 3 && timer_index  >= 0){
    timer_pointers[timer_index]->timer_load_value = value;
    return 0;
  }
  return -1;
}

/* Sets the value for the timer to load the next time it reaches
 * 0 and loads the reset value. Does not affect current timer. */
int set_background_load_value(int timer_index, int value){
  if(timer_index < 3 && timer_index  >= 0){
    timer_pointers[timer_index]->background_timer_load_value = value;
    return 0;
  }
  return -1;
}

/* Clear any interrupt data for the timer.
 * note: writing to the clear timer register clears
 * the interrupt status completely. */
int clear_interupt(int timer_index){
  if(timer_index < 3 && timer_index  >= 0){
    timer_pointers[timer_index]->interrupt_clear = 0x1;
    return 0;
  }
  return -1;
}

int start_timer(int timer_index){
  if(timer_index < 3 && timer_index  >= 0){
    timer_pointers[timer_index]->control = 0x1;
    return 0;
  }
  return -1;
}
