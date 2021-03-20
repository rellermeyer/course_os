#include <bcm2835.h>
#include <chipset.h>
#include <vm2.h>

#include "timer.h"
#include "uart.h"

volatile UartDevice bcm2835_uart;

void bcm2835_irq_handler() {}

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
    bcm2835_int_registers_base = (BCM2835InterruptRegisters *)(bcm2835_peripheral_base + 0xB000);
    bcm2835_timer_registers_base = (BCM2835SystemTimerRegisters *)(bcm2835_peripheral_base + 0x3000);

    bcm2835_uart_init(&bcm2835_uart);
}

void bcm2835_late_init() {
    bcm2835_timer_init();
}
