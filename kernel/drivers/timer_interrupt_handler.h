#ifndef __TIMER_INTERRUPT_HANDLER_H__
#define __TIMER_INTERRUPT_HANLDER_H__
#include "../include/interrupt.h"
#include "klibc.h"
#include "drivers/timer.h"
void  _schedule_register_timer_irq();
void timer_timer_interrupt_handler_q(void(*callback_function)(void *args),int time);
void timer_interrupt_handler();
#endif
