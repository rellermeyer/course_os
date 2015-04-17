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

void timer_start(int timer_index) {
  os_printf("Timer driver loaded\n");
  timer_pointers[0]->timer_load_value = 1000;
  os_printf("control address:%x\n", &(timer_pointers[0]->control));
  os_printf("control value:%x\n", timer_pointers[0]->control);
  for(int i=0; i< 4; i++){
    os_printf("Timer register = %x\n", timer_pointers[i]->timer_actual_value);
  }
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

int set_32_bit_mode(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    timer_pointers[timer_index]->control |= 0x20;
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

int set_periodic_mode(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    timer_pointers[timer_index]->control &=0xFFFFFFBE;
    return 0;
  }
  return -1;
}

int enable_timer_interrupt(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    timer_pointers[timer_index]->control |= 0x20;//0x40;
    return 0;
  }
  return -1;
}

int disable_timer_interrupt(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    timer_pointers[timer_index]->control &= 0xFFFFFFDF;
    return 0;
  }
  return -1;
}


int set_free_running_mode(int timer_index){
  //Documentation is wrong
  //control register bit 1 is for one shot or wrapping mode (assuming 0 index counting)
  if(timer_index < 4 && timer_index  >= 0){
    timer_pointers[timer_index]->control |= 0x1;
    return 0;
  }
  return -1;
}
//starts the timer, countdown from load value
int start_timer(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    timer_pointers[timer_index]->control |= 0x40;
    return 0;
  }
  return -1;
}
//prints the configuration of the control byte
void print_control_status(int timer_index){
  if(timer_index < 4 && timer_index >= 0){
    os_printf("control byte:%x",timer_pointers[timer_index]->control); 
  return 0;
  }
  return -1;
}

/*testing timer code
starts interrupts every start_val ticks */
int start_interrupts(int start_val){
        initialize_timers();
        timer_start();
        set_background_load_value(0,start_val);
        set_periodic_mode(0);
        enable_timer_interrupt(0);
	print_control_status(0);
        start_timer(0);
	// int i=10;
	while(1){
                os_printf("\n%d",get_current_timer_value(0));
		if(get_current_timer_value(0)==0){
                        os_printf("\nInterrupt");
        //        	i--;
		}
	}       
        return 0;
}
//just testing code
void timer_test(){
	initialize_timers();

	os_printf("time %d\n",get_current_timer_value(0));
	start_interrupts(5);
	//timer_start();
	//start_timer(1);
	//start_timer(2);
	//start_timer(3);
	//set_periodic_mode(0);
	//set_load_value(0, 10);
	//enable_timer_interrupt(0);	
	//set_periodic_mode(0);
  	//print_control_status(0);
        //int val=9;
	//os_printf("starting%d\n",start_timer(0));
	//for(int i=0;i<1000;i++){
	//	if(get_current_timer_value(0)!=val){
	//		os_printf("time %d\n",get_current_timer_value(0));
	//		val=get_current_timer_value(0);
 	//	}
	//	os_printf("time %d\n",get_current_timer_value(1));
	//	os_printf("time %d\n",get_current_timer_value(2));
	//	os_printf("time %d\n",get_current_timer_value(3));		
	return;
}


