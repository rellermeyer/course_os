#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

typedef struct  {
  uint32_t timer_load_value; 			// read/write
  uint32_t timer_actual_value; 			// read only
  uint32_t control; 					// read/write
  uint32_t interrupt_clear; 			// write only
  uint32_t interrupt_status; 			// read only
  uint32_t masked_interrupt_status; 	// read only
  uint32_t background_timer_load_value; // read/write
} rasp_pi_timer;

_Static_assert (sizeof (rasp_pi_timer) == 28, "rasp_pi_timer check");

void initialize_timers();
int set_load_value(int timer_index, int value);
int set_background_load_value(int timer_index, int value);
int clear_interupt(int timer_index);
int set_32_bit_mode(int timer_index);
int get_current_timer_value(int timer_index);
int set_periodic_mode(int timer_index);
int set_free_running_mode(int timer_index);
int start_timer(int timer_index);
int set_prescale(int timer_index, int mode);
int start_timer_interrupts( int timer_index, int star_val);
int conversion(int timer_index, int milliseconds);
int enable_timer_interrupt(int timer_index);
int disable_timer(int timer_index);
void timer_start(int timer_index);


#endif
