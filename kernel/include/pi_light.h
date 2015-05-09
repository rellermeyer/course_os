#include "mmap.h"

#define GPIO_BASE 0x20200000
#define LED_PIN 16

void set_up_LED();
void light_on();
void light_off();