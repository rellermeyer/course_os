#include <bcm2835.h>
#include <chipset.h>
#include <vm2.h>

#include <bcm2835_timer.h>
#include <bcm2835_uart.h>

volatile UartDevice bcm2835_uart;

void bcm2835_irq_handler() {
  volatile const uint32_t pending = bcm2835_timer_registers_base->ControlStatus;

  if(pending&Match1){
    bcm2835_timer_registers_base->ControlStatus |= Match1;
    bcm2835_timer_handle_interrupt();
  }
}

void bcm2835_fiq_handler() {}

void bcm2835_init() {
    chipset.schedule_timer_periodic = &bcm2835_schedule_timer_periodic;
    chipset.schedule_timer_once = &bcm2835_schedule_timer_once;
    chipset.deschedule_timer = &bcm2835_deschedule_timer;
    chipset.handle_irq = &bcm2835_irq_handler;
    chipset.handle_fiq = &bcm2835_fiq_handler;
    chipset.late_init = &bcm2835_late_init;
    chipset.uart = &bcm2835_uart;

    bcm2835_peripheral_base = vm2_map_peripheral(BCM2835_PERIPHERALS_PHYSICAL_BASE, 16);
    bcm2835_int_registers_base = (BCM2835InterruptRegisters *)(bcm2835_peripheral_base + 0xB200);
    bcm2835_timer_registers_base = (BCM2835SystemTimerRegisters *)(bcm2835_peripheral_base + 0x3000);

    bcm2835_uart_init(&bcm2835_uart);
}

void bcm2835_late_init() {
    bcm2835_timer_init();
}
