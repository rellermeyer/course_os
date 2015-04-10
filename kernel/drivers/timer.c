#include <stdint.h>
#include "klibc.h"
#include "drivers/timer.h"
/* initializes timers as an array. Call this before
 * using any of the timer functions */
void initialize_timers(){
  os_printf("initializing timers\n");
  volatile rasp_pi_timer *TIMER_0 = (rasp_pi_timer *)0x101e2000; 
  volatile rasp_pi_timer *TIMER_1 = (rasp_pi_timer *)0x101e2020; 
  volatile rasp_pi_timer *TIMER_2 = (rasp_pi_timer *)0x101e3000; 
  volatile rasp_pi_timer *TIMER_3 = (rasp_pi_timer *)0x101e3020; 
  timer_pointers[0] =(rasp_pi_timer*)TIMER_0;
  timer_pointers[1] =(rasp_pi_timer*)TIMER_1;
  timer_pointers[2] =(rasp_pi_timer*)TIMER_2;
  timer_pointers[3] =(rasp_pi_timer*)TIMER_3;
}

void timer_start() {
  os_printf("Timer driver loaded\n");
  timer_pointers[0]->timer_load_value = 1000;
  os_printf("control address:%x\n", &(timer_pointers[0]->control));
  timer_pointers[0]->control |= 0x1;
  os_printf("control value:%x\n", timer_pointers[0]->control);
  for(int i=0; i< 10; i++){
    os_printf("Timer register = %x\n", timer_pointers[0]->timer_actual_value);
  }
}

/* This function sets the value that the timer will begin at.
 * This operation also resets the timer to this new value.
 * if you want to keep the timer running with its current state
 * use set background load. */
int set_load_value(int timer_index, int value){
  if(timer_index < 4 && timer_index  >= 0){
    timer_pointers[timer_index]->timer_load_value = value;
    return 0;
  }
  return -1;
}

int get_timer_control_value(int timer_index){
  if(timer_index < 4 && timer_index  >= 0){
    return timer_pointers[timer_index]->control;
  }
  return -1;
}

/* Sets the value for the timer to load the next time it reaches
 * 0 and loads the reset value. Does not affect current timer. */
int set_background_load_value(int timer_index, int value){
  if(timer_index < 4 && timer_index  >= 0){
    timer_pointers[timer_index]->background_timer_load_value = value;
    return 0;
  }
  return -1;
}

/* Clear any interrupt data for the timer.
 * note: writing to the clear timer register clears
 * the interrupt status completely. */
int clear_interupt(int timer_index){
  if(timer_index < 4 && timer_index  >= 0){
    timer_pointers[timer_index]->interrupt_clear = 0x1;
    return 0;
  }
  return -1;
}

int set_32_bit_mode(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    timer_pointers[timer_index]->control |= 0x20;
    return 0;
  }
  return -1;
}

int get_current_timer_value(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    return timer_pointers[timer_index]->timer_actual_value;
  }
  return -1;
}

int set_periodic_mode(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    timer_pointers[timer_index]->control &= 0xFFFFFFFD;
    return 0;
  }
  return -1;
}

int enable_timer_interrupt(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    timer_pointers[timer_index]->control |= 0x40;
    return 0;
  }
  return -1;
}

int disable_timer_interrupt(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    timer_pointers[timer_index]->control &= 0xFFFFFFFE;
    return 0;
  }
  return -1;
}


int set_free_running_mode(int timer_index){
  //Documentation is wrong
  //control register bit 1 is for one shot or wrapping mode (assuming 0 index counting)
  if(timer_index < 4 && timer_index  >= 0){
    timer_pointers[timer_index]->control |= 0x2;
    return 0;
  }
  return -1;
}

int start_timer(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    timer_pointers[timer_index]->control |= 0x40;
    return 0;
  }
  return -1;
}

//testing timer code
void timer_test(){
	initialize_timers();
	timer_start();
}
