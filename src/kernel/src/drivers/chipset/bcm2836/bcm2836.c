// https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2836/QA7_rev3.4.pdf
#include <bcm2836.h>
#include <chipset.h>
#include <timer.h>
#include <uart.h>
#include <vm2.h>

void bcm2836_irq_handler() {
    volatile const uint32_t pending = bcm2836_registers_base->Core3IRQSource;

    switch (pending) {
        // Generic timers
        case PHYSICAL_SECURE_TIMER:
            timer_handle_interrupt();
            break;
    }
}

void bcm2836_fiq_handler() {}

void bcm2836_init() {
    chipset.schedule_timer_periodic = &bcm2836_schedule_timer_periodic;
    chipset.schedule_timer_once = &bcm2836_schedule_timer_once;
    chipset.deschedule_timer = &bcm2836_deschedule_timer;
    chipset.uart_putc = &bcm2836_uart_putc;
    chipset.uart_on_message = &bcm2836_uart_on_message;
    chipset.handle_irq = &bcm2836_irq_handler;
    chipset.handle_fiq = &bcm2836_fiq_handler;
    chipset.late_init = &bcm2836_late_init;

    bcm2836_registers_base =
        (struct BCM2836Registers *)vm2_map_peripheral(BCM2836_REGISTERS_PHYSICAL_BASE, 4);
    bcm2836_peripheral_base = vm2_map_peripheral(BCM2836_PERIPHERALS_PHYSICAL_BASE, 16);

    bcm2836_uart_init();
}

void bcm2836_late_init() {
    bcm2836_timer_init();
}
