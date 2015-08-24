
#include "mmap.h"
#include <stdint.h>
#include "pi_light.h"

//#define GPIO_BASE 0x20200000
//#define LED_PIN 16

volatile unsigned int* gpio;
void set_up_LED()
{
	//Initialize the LED
	gpio = (unsigned int*) GPIO_BASE;
	gpio[1] |= (1 << 18);
}
void light_on()
{
	gpio[10] = (1 << LED_PIN);
}
void light_off()
{
	gpio[7] = (1 << LED_PIN);
}
void on_off()
{
	light_on();
	for (int i = 0; i < 250000; ++i)
	{
	}
	light_off();
	for (int i = 0; i < 250000; ++i)
	{
	}
}
void LED_alternate()
{

	for (int i = 0; i < 20; ++i)
	{
		on_off();
	}
}
void LED_alternate2(int numFlash)
{
	//Flash the ACT LED numFlash number of times
	for (int i = 0; i < numFlash; ++i)
	{
		on_off();
	}
}
