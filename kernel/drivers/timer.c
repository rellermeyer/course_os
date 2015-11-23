/* Device Driver for ARM Dual-Timer Module (SP804)
 Reference Manual can be found here : http://infocenter.arm.com/help/topic/com.arm.doc.ddi0271d/DDI0271.pdf*/
//#include <stdint.h>
#include "klibc.h"
#include "drivers/timer.h"
#include "interrupt.h"

rasp_pi_timer *timer_pointers[4];

void handler(void* p);

/* initializes timers as an array. Call this before
 * using any of the timer functions */
void initialize_timers()
{
	os_printf("initializing timers\n");
	volatile rasp_pi_timer *TIMER_0 = (rasp_pi_timer *) 0x101e2000;
	volatile rasp_pi_timer *TIMER_1 = (rasp_pi_timer *) 0x101e2020;
	volatile rasp_pi_timer *TIMER_2 = (rasp_pi_timer *) 0x101e3000;
	volatile rasp_pi_timer *TIMER_3 = (rasp_pi_timer *) 0x101e3020;
	timer_pointers[0] = (rasp_pi_timer*) TIMER_0;
	timer_pointers[1] = (rasp_pi_timer*) TIMER_1;
	timer_pointers[2] = (rasp_pi_timer*) TIMER_2;
	timer_pointers[3] = (rasp_pi_timer*) TIMER_3;

	interrupt_handler_t *tmr_handler = kmalloc(sizeof(interrupt_handler_t));
	tmr_handler->handler = handler;

	os_printf("@@@@@@@@ fn ptr: %X\n", tmr_handler);

	register_interrupt_handler(4, tmr_handler);
}

#define CHECK_TIMER_INDEX(index) { if (index < 0 || index > 4) return -1; }

/* This function sets the value that the timer will begin at.
 * This operation also resets the timer to this new value.
 * if you want to keep the timer running with its current state
 * use set background load. */
int set_load_value(int timer_index, int value)
{
	CHECK_TIMER_INDEX(timer_index);

	timer_pointers[timer_index]->timer_load_value = value;
	return 0;
}

//returns the current control register value (configuration of timer) 
int get_timer_control_value(int timer_index)
{
	CHECK_TIMER_INDEX(timer_index);

	return timer_pointers[timer_index]->control;
}

/* Sets the value for the timer to load the next time it reaches
 * 0 and loads the reset value. Does not affect current timer. */
int set_background_load_value(int timer_index, int value)
{
	CHECK_TIMER_INDEX(timer_index);

	timer_pointers[timer_index]->background_timer_load_value = value;
	return 0;
}

/* Clear any interrupt data for the timer.
 * note: writing to the clear timer register clears
 * the interrupt status completely. */
int clear_interrupt(int timer_index)
{
	CHECK_TIMER_INDEX(timer_index);

	timer_pointers[timer_index]->interrupt_clear = 0x1;

	return 0;
}

//enable 32 bit mode in the specified timer index 
int set_32_bit_mode(int timer_index)
{
	CHECK_TIMER_INDEX(timer_index);

	disable_timer(timer_index);
	timer_pointers[timer_index]->control |= 0x2;
	return 0;
}

int get_current_timer_value(int timer_index)
{
	CHECK_TIMER_INDEX(timer_index);

	return timer_pointers[timer_index]->timer_actual_value;
}

//enable periodic mode of specified timer index
int set_periodic_mode(int timer_index)
{
	CHECK_TIMER_INDEX(timer_index);

	disable_timer(timer_index);
	timer_pointers[timer_index]->control &= 0xFFFFFFBE;

	return 0;
}

//converts ms into ticks
//assumes modes are valid, if not, should return 0
int conversion(int timer_index, int milliseconds)
{
	int mode = (timer_pointers[timer_index]->control & 0xC) >> 2;

	int ticks = 0;
	if (mode == 0)
	{
		ticks = 32;
	}
	else if (mode == 1)
	{
		ticks = 256;
	}
	else if (mode == 2)
	{
		ticks = 1000;
	}
	return milliseconds * ticks;
}

//function to set timer to different timer clock rates... 0 -> 1 (default), 1 -> 16, 2 -> 256...
//any other modes are invalid and will do nothing
int set_prescale(int timer_index, int mode)
{
	CHECK_TIMER_INDEX(timer_index);

	disable_timer(timer_index);
	if (mode == 0)
		timer_pointers[timer_index]->control &= 0xFFFFFFF3;
	else if (mode == 1)
	{
		timer_pointers[timer_index]->control |= 0x4;
		timer_pointers[timer_index]->control &= 0xFFFFFFF7;
	}
	else if (mode == 2)
	{
		timer_pointers[timer_index]->control |= 0x8;
		timer_pointers[timer_index]->control &= 0xFFFFFFFB;
	}
	return 0;
}

//enables timer interrupt of the given timer index
int enable_timer_interrupt(int timer_index)
{
	CHECK_TIMER_INDEX(timer_index);

	timer_pointers[timer_index]->control |= (1 << 5);
	return 0;
}

//disables timer interrupt of the given timer index
int disable_timer_interrupt(int timer_index)
{
	CHECK_TIMER_INDEX(timer_index);

	timer_pointers[timer_index]->control &= 0xFFFFFFDF;
	return 0;
}

int set_free_running_mode(int timer_index)
{
	CHECK_TIMER_INDEX(timer_index);

	disable_timer(timer_index);
	timer_pointers[timer_index]->control |= 0x1;
	return 0;
}

//starts the timer, countdown from load value
int enable_timer(int timer_index)
{
	CHECK_TIMER_INDEX(timer_index);

	timer_pointers[timer_index]->control |= (1 << 7);
	return 0;
}

//pauses the timer
int disable_timer(int timer_index)
{
	CHECK_TIMER_INDEX(timer_index);

	timer_pointers[timer_index]->control &= 0xFFFFFF7F;
	return 0;
}

//prints the configuration of the control byte
int print_control_status(int timer_index)
{
	CHECK_TIMER_INDEX(timer_index);

	os_printf("control byte:%x", timer_pointers[timer_index]->control);
	return 0;
}

void timer_start(int timer_index)
{
	os_printf("Timer driver loaded\n");
	//set_prescale(timer_index,2);
	enable_timer(timer_index);
	os_printf("control address:%x\n", &(timer_pointers[timer_index]->control));
	os_printf("control value:%x\n", timer_pointers[timer_index]->control);
}

/*starts interrupts every start_val ticks */
//You give it a vallut and the specific timer you want to star.
// YOu have four timers just start with timer zero
// The speed of counter or number ticks depends on qemu hertz speed
// we don't know he start of it but it should be around 10 milliseconds
// per proccess.
// ex: start_timer_interrupts(0,10) which means start timer(0)
// and interrupt every 10 clicks.
int start_timer_interrupts(int timer_index, int milliseconds)
{
//	conversion(timer_index, milliseconds);
	CHECK_TIMER_INDEX(timer_index);

	int clicks = conversion(timer_index, milliseconds);

	os_printf("CLICKS ARE %d\n", clicks);

	set_background_load_value(timer_index, clicks);
	set_periodic_mode(timer_index);
	enable_timer_interrupt(timer_index);
	timer_start(timer_index);
	// return 0;
	// return -1;

//just testing code
//int count=0;
	/*while(1){
	 os_printf("\n%d",get_current_timer_value(timer_index));
	 if(get_current_timer_value(timer_index)==0){
	 os_printf("\nInterrupt Control Val:%X \t Process Val:%X\t VIC Val:%X\t",get_timer_control_value(timer_index),get_proc_status(),mmio_read(VIC_INT_ENABLE));
	 // count++;
	 }
	 //  if(get_current_timer_value(timer_index)==0){
	 //	hw_interrupt_enable(4);
	 ///	}
	 //   if(get_current_timer_value(timer_index==9&count>0))
	 //	hw_interrupt_enable(4);

	 }  */

	return 0;
}

void handler(void *args)
{
	os_printf("@@@@@@ RECEIVED TIMER INTERRUPT\n");

	os_printf("ARGS ARE %d\n", args);

	clear_interrupt(0);
}

void timer_test()
{

//	enable_interrupt(ALL_INTERRUPT_MASK);

	/*interrupt_handler_t *tmr_handler = kmalloc(sizeof(interrupt_handler_t));
	 tmr_handler->handler = simple_timer_handler;
	 os_printf("fn ptr: %X\n", simple_timer_handler);
	 register_interrupt_handler(4, tmr_handler);*/

	os_printf("FIQ status: %X\n", mmio_read(VIC_FIQ_STATUS));
	initialize_timers();
	start_timer_interrupts(0, 10);
	//print_control_status(1);

	// Wait forever...
	/*	os_printf("\n");
	 int cnt = 0;
	 os_printf("Timer: %d\n", timer_pointers[0]->timer_actual_value);
	 os_printf("Timer: %d\n", timer_pointers[0]->timer_actual_value);
	 os_printf("Timer: %d\n", timer_pointers[0]->timer_actual_value);
	 while (!(timer_pointers[0]->masked_interrupt_status&1)) {
	 cnt++;
	 //os_printf("%X\n", timer_pointers[1]->timer_actual_value);
	 int i;
	 for (i=0; i<1000; i++);
	 }
	 os_printf("%d\n", cnt);
	 os_printf("%X\n", mmio_read(PIC_ADDRESS+0x8));
	 os_printf("%X\n", mmio_read(VIC_IRQ_STATUS));
	 os_printf("%X\n", mmio_read(VIC_FIQ_STATUS));
	 os_printf("Timer: %d\n", timer_pointers[0]->timer_actual_value);
	 os_printf("Timer: %d\n", timer_pointers[0]->timer_actual_value);
	 os_printf("Timer: %d\n", timer_pointers[0]->timer_actual_value);
	 while(1);
	 */
	return;
}
