/* Device Driver for ARM Dual-Timer Module (SP804)
Reference Manual can be found here : http://infocenter.arm.com/help/topic/com.arm.doc.ddi0271d/DDI0271.pdf*/
#include <stdint.h>
#include "klibc.h"
#include "drivers/timer.h"

/* initializes timers as an array. Call this before
 * using any of the timer functions */
void initialize_timers(){
  os_printf("initializing timers\n");
  volatile rasp_pi_timer *TIMER_0 = (rasp_pi_timer *)0x101e2000; 
  volatile rasp_pi_timer *TIMER_1 = (rasp_pi_timer *)0x101e2020; 
  volatile rasp_pi_timer *TIMER_2 = (rasp_pi_timer *)0x101e3000; 
  volatile rasp_pi_timer *TIMER_3 = (rasp_pi_timer *)0x101e3020; 
  timer_pointers[0] =(rasp_pi_timer*)TIMER_0;
  timer_pointers[1] =(rasp_pi_timer*)TIMER_1;
  timer_pointers[2] =(rasp_pi_timer*)TIMER_2;
  timer_pointers[3] =(rasp_pi_timer*)TIMER_3;
}


/* This function sets the value that the timer will begin at.
 * This operation also resets the timer to this new value.
 * if you want to keep the timer running with its current state
 * use set background load. */
int set_load_value(int timer_index, int value){
  if(timer_index < 4 && timer_index  >= 0){
    timer_pointers[timer_index]->timer_load_value = value;
    return 0;
  }
  return -1;
}

//returns the current control register value (configuration of timer) 
int get_timer_control_value(int timer_index){
  if(timer_index < 4 && timer_index  >= 0){
    return timer_pointers[timer_index]->control;
  }
  return -1;
}

/* Sets the value for the timer to load the next time it reaches
 * 0 and loads the reset value. Does not affect current timer. */
int set_background_load_value(int timer_index, int value){
  if(timer_index < 4 && timer_index  >= 0){
    timer_pointers[timer_index]->background_timer_load_value = value;
    return 0;
  }
  return -1;
}

/* Clear any interrupt data for the timer.
 * note: writing to the clear timer register clears
 * the interrupt status completely. */
int clear_interrupt(int timer_index){
  if(timer_index < 4 && timer_index  >= 0){
    timer_pointers[timer_index]->interrupt_clear = 0x1;
    return 0;
  }
  return -1;
}

//enable 32 bit mode in the specified timer index 
int set_32_bit_mode(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    timer_pointers[timer_index]->control |= 0x2;
    return 0;
  }
  return -1;
}

int get_current_timer_value(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    return timer_pointers[timer_index]->timer_actual_value;
  }
  return -1;
}

//enable periodic mode of specified timer index
int set_periodic_mode(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    timer_pointers[timer_index]->control &= 0xFFFFFFBE;
    timer_pointers[timer_index]->control &=0xFFFFFFBE;
		timer_pointers[timer_index]->control |=0x40;
    return 0;
  }
  return -1;
}

//converts ms into ticks
//assumes modes are valid, if not, should return 0
long int conversion(int ms){
	int mode = timer_pointers[0]->control & 0xC;
	int ticks = 0;
	if(mode == 0){
		ticks = 32;
	}
	else if(mode == 1){
		ticks = 256;
	}
	else if(mode == 2){
		ticks = 1000;
	}
	return ms * ticks;
}

//function to set timer to different timer clock rates... 0 -> 1 (default), 1 -> 16, 2 -> 256...
//any other modes are invalid and will do nothing
int set_prescale(int timer_index, int mode){
	if(timer_index < 4 && timer_index >= 0 && mode <= 2 && mode >= 0){
		if(mode == 0)
			timer_pointers[timer_index]->control &= 0xFFFFFFF3;
		else if(mode == 1)
		{
			timer_pointers[timer_index]->control |= 0x4;
			timer_pointers[timer_index]->control &= 0xFFFFFFF7;
		}
		else if(mode == 2){
			timer_pointers[timer_index]->control |= 0x8;
			timer_pointers[timer_index]->control &= 0xFFFFFFFB;
		}
		return 0;
	}
	return -1;
}

//enables timer interrupt of the given timer index
int enable_timer_interrupt(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    timer_pointers[timer_index]->control |= 0x20;
    return 0;
  }
  return -1;
}

//disables timer interrupt of the given timer index
int disable_timer_interrupt(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    timer_pointers[timer_index]->control &= 0xFFFFFFDF;
    return 0;
  }
  return -1;
}


int set_free_running_mode(int timer_index){
  if(timer_index < 4 && timer_index  >= 0){
    timer_pointers[timer_index]->control |= 0x1;
    return 0;
  }
  return -1;
}
//starts the timer, countdown from load value
int enable_timer(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    timer_pointers[timer_index]->control |= 0x80;
    return 0;
  }
  return -1;
}

//pauses the timer
int disable_timer(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    timer_pointers[timer_index]->control &= 0xFFFFFF7F;
    return 0;
  }
  return -1;
}

//prints the configuration of the control byte
int print_control_status(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    os_printf("control byte:%x",timer_pointers[timer_index]->control); 
  	return 0;
  }
  return -1;
}

void timer_start(int timer_index) {
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
int start_timer_interrupts(int timer_index,int start_val){
  if(timer_index < 4 && timer_index >= 0){
    initialize_timers();
    timer_start(timer_index);
    set_background_load_value(timer_index,start_val);
    set_periodic_mode(timer_index);
    enable_timer_interrupt(timer_index);     
    return 0;
  }
  return -1;
}
//just testing code
/*while(1){
                os_printf("\n%d",get_current_timer_value(1));
                if(get_current_timer_value(1)==0){
                        os_printf("\nInterrupt");
                        //i--;
                }
        } */


void timer_test(){
	initialize_timers();
	start_timer_interrupts(1,5);
	print_control_status(1);
		
	return;
  }
