#include<klibc.h>
#include<drivers/timer.h>
#include<driver_handler.h>

static void (*function)(void *args);

void function_call( void (*callback_function)(void *args),int handler)
{
        function=callback_function;
}

void timer_interrupt_handler(){
 	function(args);
        return;
}
