#ifndef TIMER_BCM2836_H
#define TIMER_BCM2836_H

#include <chipset.h>
#include <stdint.h>

/*
 * ARM Generic Timer driver for BCM2836
 */

void bcm2836_timer_init();

void timer_handle_interrupt();

TimerHandle bcm2836_schedule_timer_once(TimerCallback callback, uint32_t delay_ms);

TimerHandle bcm2836_schedule_timer_periodic(TimerCallback callback, uint32_t delay_ms);

void bcm2836_deschedule_timer(TimerHandle handle);

#endif
