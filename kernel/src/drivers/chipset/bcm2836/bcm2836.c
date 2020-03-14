// https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2836/QA7_rev3.4.pdf
#include <bcm2836.h>
#include <chipset.h>
#include <interrupt.h>
#include <mmio.h>
#include <stdio.h>
#include <vm2.h>

// static TimerHandle handleindex;

// uart.c
void bcm2836_uart_init();
void bcm2836_uart_putc(char c, int uartchannel);
void bcm2836_uart_on_message(UartCallback callback, int uartchannel);

// timer.c
void bcm2836_timer_init();
TimerHandle bcm2836_schedule_timer_periodic(TimerCallback callback, uint32_t ms);
TimerHandle bcm2836_schedule_timer_once(TimerCallback callback, uint32_t ms);
void bcm2836_deschedule_timer(TimerHandle handle);

void bcm2836_on_interrupt(InterruptCallback callback) {}


void bcm2836_late_init() {
    bcm2836_timer_init();
}

void bcm2836_irq_handler() {
    volatile uint32_t pending = bcm2836_registers_base->Core0IRQSource;
    TRACE("interrupt: 0x%x", pending);
}

void bcm2836_fiq_handler() {}

void bcm2836_init() {
    chipset.schedule_timer_periodic = &bcm2836_schedule_timer_periodic;
    chipset.schedule_timer_once = &bcm2836_schedule_timer_once;
    chipset.deschedule_timer = &bcm2836_deschedule_timer;
    chipset.uart_putc = &bcm2836_uart_putc;
    chipset.uart_on_message = &bcm2836_uart_on_message;
    chipset.on_interrupt = &bcm2836_on_interrupt;
    chipset.handle_irq = &bcm2836_irq_handler;
    chipset.handle_fiq = &bcm2836_fiq_handler;
    chipset.late_init = &bcm2836_late_init;

    bcm2836_registers_base =
        (struct BCM2836Registers *)vm2_map_peripheral(BCM2836_REGISTERS_PHYSICAL_BASE, 1);
    bcm2836_peripheral_base = vm2_map_peripheral(BCM2836_PERIPHERALS_PHYSICAL_BASE, 4);

    bcm2836_uart_init();
}
