#include <stdint.h>


typedef struct {
  uint32_t timer_load_value; //read/write
  uint32_t timer_actual_value; //read only
  uint32_t control; //read/write
  uint32_t limit;
  int frequency;
  //int int_level; what is this?

} rasp_pi_timer;

/*volatile uint32_t *const TIMER_0_ADDRESS= (uint32_t *)0x101e2000;*/
/*volatile uint32_t *const TIMER_1_ADDRESS= (uint32_t *)0x101e2020;*/
/*volatile uint32_t *const TIMER_2_ADDRESS= (uint32_t *)0x101e3000;*/
/*volatile uint32_t *const TIMER_3_ADDRESS= (uint32_t *)0x101e3020;*/

void timer_start() {
  volatile rasp_pi_timer *TIMER_0 = (rasp_pi_timer *)0x101e2000; 
  print_uart0("Timer driver loaded\n");
  TIMER_0->timer_load_value = 1000;
  os_printf("control address:%x\n", &(TIMER_0->control));
  TIMER_0->control |= 0x1;
  os_printf("control value:%x\n", TIMER_0->control);
  int i = 0;
  for(i; i< 10; i++){
    os_printf("Timer register = %x\n", TIMER_0->timer_actual_value);
  }

}
