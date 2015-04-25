/*
 *
 * Interrupts  
 *
 */
#include "interrupt.h"
#include "klibc.h"
#include "drivers/timer.h"

// there are 32 kinds of interrupts on the VIC
// this structure may need to be expanded if the secondary controller is incorporated
static interrupt_handler_t *handlers[MAX_NUM_INTERRUPTS];

// holds defined fiq interrupts
static int check_if_fiq[MAX_NUM_INTERRUPTS];

// define interrupt types
interrupt_t IRQ = IRQ_MASK;
interrupt_t FIQ = FIQ_MASK;
interrupt_t ALL = ALL_INTERRUPT_MASK;

// In a system with an interrupt controller, software is required to:
// determine from the interrupt controller which interrupt source is requesting service
// determine where the service routine for that interrupt source is loaded
// mask or clear that interrupt source, before re-enabling processor interrupts to permit another interrupt to be taken.

// Interrupts must be enabled in three places:
// (1) the core (CPSR)
// (2) the VIC (interrupt controller)
// (3) (sometimes) in the device (this should be done in the device driver)

// Setup FIQ interrupts
void init_fiqs(){
	check_if_fiq[11] = 1; // synchronous serial port
	check_if_fiq[17] = 1; // DMA controller  
}

// when you register an interrupt handler it's line will be enabled in the VIC
// telling the device itself to fire interrupts should be done in the driver
//
// also, since we don't really have a working malloc, the handler structure will
// have to be built in the driver and passed to register_
int register_interrupt_handler(int num, interrupt_handler_t *handler){

	if(num < 0 || num > MAX_NUM_INTERRUPTS) // bad irq number
		return -1;
	else if(handlers[num] != 0) // something has already been registered there
		return -1;
	else if(handler == 0) // we need a NULL macro
		return -1;
	
	// put the handler in the array
	handlers[num] = handler;

	// enable the specific interrupt in hardware on the VIC
	hw_interrupt_enable(num);

	// check to see if this is an FIQ
	if (check_if_fiq[num])
		// update the "select" register on the VIC
		vic_select_fiq(num);

	// return a success value
	return 0;
}

// handle_interrupt takes a number (the interrupt from the VIC), looks into
// the table of registered handlers, and calls the appropriate handler
void handle_irq_interrupt(int interrupt_vector){
	os_printf("handling interrupt %d\n", interrupt_vector);
	// go to handler routine
	handlers[interrupt_vector]->handler((void*) interrupt_vector);
	// ok interrupt handled, clear it
	hw_interrupt_disable(interrupt_vector); // this doesn't seem right b/c we need to then re-enable
	// yea this needs to be changed
	// we actually should probably just go ahead and disable interrupts on the VIC and in the core (and possibly on the device as well) since we don't have a nested handler
}


/* enable IRQ and/or FIQ */
void enable_interrupt(interrupt_t mask) {
	// enable interrupt on the core
	switch(mask) {
		case IRQ_MASK:
			asm volatile("cpsie i");
			break;
		case FIQ_MASK:
			asm volatile("cpsie f");
			break;
		case ALL_INTERRUPT_MASK:
			asm volatile("cpsie if");
			break;
	}
}


/* disable IRQ and/or FIQ */
void disable_interrupt(interrupt_t mask) {
	// disable interrupts on the core
	switch(mask) {
		case IRQ_MASK:
			asm volatile("cpsid i");
			break;
		case FIQ_MASK:
			asm volatile("cpsid f");
			break;
		case ALL_INTERRUPT_MASK:
			asm volatile("cpsid if");
			break;
	}
}

/* disable IRQ and/or FIQ, but also return a copy of the CPSR */
int disable_interrupt_save(interrupt_t mask) {
	/* get a copy of the current process status register */
	int cpsr;
	asm volatile("mrs %0, cpsr" : "=r"(cpsr));
	// disable interrupts on the core
	switch(mask) {
		case IRQ_MASK:
			asm volatile("cpsid i");
			break;
		case FIQ_MASK:
			asm volatile("cpsid f");
			break;
		case ALL_INTERRUPT_MASK:
			asm volatile("cpsid if");
			break;
	}
	return cpsr;
}

/* return a full 32-bit copy of the current process status register */
int get_proc_status(void) {
	int cpsr;
	asm volatile("mrs %0, cpsr" : "=r"(cpsr));
	return cpsr;
}

/* restore control status (interrupt, mode bits) of the cpsr */
/* (e.g. when we return from a handler, restore value from 
   disable_interrupt_save				     */
void restore_proc_status(int cpsr) {
	asm volatile("msr cpsr_c, %0" : : "r"(cpsr));
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
// Create the handler
void _schedule_register_timer_irq(){
        interrupt_handler_t *timer;
        timer->handler=&timer_interrupt_handler;
        register_interrupt_handler(4,timer);
        timer_start();
}


