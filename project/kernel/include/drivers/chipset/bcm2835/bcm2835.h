// https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2835/BCM2835-ARM-Peripherals.pdf

#ifndef BCM2835_H
#define BCM2835_H
#include <stdint.h>


#define BCM2835_PERIPHERALS_PHYSICAL_BASE 0x20000000

// 0x7E00B200
typedef struct BCM2835InterruptRegisters {
  uint32_t IRQBasicPending;
  uint32_t IRQPending1;
  uint32_t IRQPending2;
  uint32_t FIQControl;
  uint32_t EnableIRQ1;
  uint32_t EnableIRQ2;
  uint32_t EnableBasicIRQ;
  uint32_t DisableIRQ1;
  uint32_t DisableIRQ2;
  uint32_t DisableBasicIRQ;
} BCM2835InterruptRegisters;

enum IRQ1Source {
  SysTimerCMP1 = 1 << 1,
};


void bcm2835_init();
void bcm2835_late_init();

__attribute__((__common__)) size_t bcm2835_peripheral_base;
__attribute__((__common__)) volatile struct BCM2835InterruptRegisters * bcm2835_int_registers_base;

#endif
