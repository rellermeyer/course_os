#include "interrupt.h"
#include "klibc.h"
#include "drivers/timer.h"

static void (*function)(void *args);

void timer_interrupt_handler_q( void (*callback_function)(void *args),int time)
{
	function=callback_function;		
	start_timer_interrupts(0,time);
}

void timer_interrupt_handler(){
	os_printf("hello I'm interrupting");
}

// Create the handler
void _schedule_register_timer_irq() {
	interrupt_handler_t *timer = (interrupt_handler_t*)kmalloc(sizeof(interrupt_handler_t));
	timer->handler = &timer_interrupt_handler_q;
	register_interrupt_handler(4, timer);
	timer_start(4);
}

