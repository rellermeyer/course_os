#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

typedef struct {
  uint32_t timer_load_value; //read/write
  uint32_t timer_actual_value; //read only
  uint32_t control; //read/write
  uint32_t interrupt_clear; // write only
  uint32_t interrupt_status; //read only
  uint32_t masked_interrupt_status; //read only
  uint32_t background_timer_load_value; //read/write
} rasp_pi_timer;

void initialize_timers();
void timer_start();

rasp_pi_timer *timer_pointers[4];

#endif
