#ifndef TIMER_BCM2835_H
#define TIMER_BCM2835_H

#include <chipset.h>
#include <stdint.h>
#define SYSTEM_TIMER_FREQ 1000000

/*
 * System Timer driver for BCM2835
 */

typedef struct BCM2835SystemTimerRegisters {

  uint32_t ControlStatus;  
  uint32_t CounterLower;
  uint32_t CounterHigher;
  uint32_t Compare0;
  uint32_t Compare1;
  uint32_t Compare2;
  uint32_t Compare3;
  
} BCM2835SystemTimerRegisters;

enum ControlStatus {
  Match3 = 0x1 << 3,
  Match2 = 0x1 << 2,
  Match1 = 0x1 << 1,
  Match0 = 0x1 << 0,
};

void bcm2835_timer_init();

void bcm2835_timer_handle_interrupt();

TimerHandle bcm2835_schedule_timer_once(TimerCallback callback, uint32_t delay_ms);

TimerHandle bcm2835_schedule_timer_periodic(TimerCallback callback, uint32_t delay_ms);

void bcm2835_deschedule_timer(TimerHandle handle);

__attribute__((__common__)) volatile struct BCM2835SystemTimerRegisters * bcm2835_timer_registers_base;

#endif
