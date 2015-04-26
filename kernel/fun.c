#include <stdio.h>
typedef struct interrupt_handler_t
{
	void (*handler)(void *args);
} interrupt_handler_t; 
static interrupt_handler_t *handlers[12];
int register_interrupt_handler(int num, interrupt_handler_t *handler){
	handlers[num] = handler;
	return 0;
}
void timer_interrupt_handler(){
printf("hello I'm interrupting");
//function(args);
}

void _schedule_register_timer_irq(){
        interrupt_handler_t *timer;
        timer->handler=&timer_interrupt_handler;
       register_interrupt_handler(4,timer);
       // initialize_timers();
}
void main(){
	int x;
	x=5;
	_schedule_register_timer_irq();
	handlers[4]->handler((void *)&x);
}
