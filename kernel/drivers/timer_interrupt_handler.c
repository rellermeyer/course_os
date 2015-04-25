#include "../include/interrupt.h"
<<<<<<< HEAD
#include "klibc.h"
#include "drivers/timer.h"
=======
#include "../include/klibc.h"
#include "../include/timer.h"
>>>>>>> a9720e750e1357a4b9e523bd70e1d5d1af8f5724


// Create the handler
void _schedule_register_timer_irq(){
	interrrupt_handler_t timer;
	timer->handler=&timer_interrupt_handler;
	register_interrupt_handler(4, timer);
	timer_start();
}

static void (*function)(void *args);

void timer_interrupt_handler_q( void (*callback_function)(void *args),int time)
{
	function=callback_function;		
	start_timer_interrupts(0,time);
}
void timer_interrupt_handler(){
os_printf("hello I'm interrupting");
//function(args);
}

