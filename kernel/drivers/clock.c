#include <stdint.h>
#include "clock.h"


void get_time(){
  uint32_t temp_time = *CLOCK;
  os_printf("the time is? %x\n", temp_time);
  get_seconds(temp_time);

}

void get_seconds(uint32_t time_thing){
  uint32_t SECOND_MASK = 0xF;  
  uint32_t TENS_OF_SECONDS_MASK = 0x7;
  uint32_t seconds = time_thing & SECOND_MASK;
  uint32_t tens_seconds = (time_thing >> 4) & TENS_OF_SECONDS_MASK;
  uint32_t total_seconds = seconds + 10 * tens_seconds;
  os_printf("seconds: %d\n", total_seconds);


}
