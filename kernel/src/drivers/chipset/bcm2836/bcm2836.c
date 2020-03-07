#include <bcm2836.h>
#include <chipset.h>
#include <mmap.h>
#include <timer.h>
#include <uart.h>

volatile BCM2836Registers* bcm2836_registers_base = (BCM2836Registers*) 0x40000000;
const size_t BCM2836_peripheral_base = 0x3F000000;

void bcm2836_irq_handler()
{
    volatile const uint32_t pending = bcm2836_registers_base->Core0IRQSource;

    switch (pending) {
    // Generic timers
    case PHYSICAL_SECURE_TIMER:
        timer_handle_interrupt();
        break;
    }
}

void bcm2836_fiq_handler() {}

void bcm2836_init()
{
    bcm2836_uart_init();
    chipset.schedule_timer_periodic = &bcm2836_schedule_timer_periodic;
    chipset.schedule_timer_once = &bcm2836_schedule_timer_once;
    chipset.deschedule_timer = &bcm2836_deschedule_timer;
    chipset.uart_putc = &bcm2836_uart_putc;
    chipset.uart_on_message = &bcm2836_uart_on_message;
    chipset.handle_irq = &bcm2836_irq_handler;
    chipset.handle_fiq = &bcm2836_fiq_handler;
    chipset.late_init = &bcm2836_late_init;

    // Mapping memory used by bcm2836 peripherals
    request_identity_mapped_section(BCM2836_peripheral_base, 4);

    // Map control registers
    request_identity_mapped_section((size_t)bcm2836_registers_base, 1);
}

void bcm2836_late_init()
{
    bcm2836_timer_init();
}
