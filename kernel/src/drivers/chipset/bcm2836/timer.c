#include <bcm2836.h>
#include <chipset.h>
#include <mmio.h>
#include <stdio.h>

inline static uint32_t get_frequency() {
    uint32_t val;
    asm volatile("mrc p15, 0, %0, c14, c0, 0" : "=r"(val));
    return val;
}

void write_interrupt_count_value(uint32_t val) {
    asm volatile("mcr p15, 0, %0, c14, c3, 0" ::"r"(val));
}

uint32_t read_interrupt_count_value() {
    uint32_t val;
    asm volatile("mrc p15, 0, %0, c14, c3, 0" : "=r"(val));
    return val;
}


// https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2836/QA7_rev3.4.pdf
// Page ~ 117
struct IRQ_registers {
    uint32_t irq_basic_pending;
    uint32_t irq_gpu1_pending;
    uint32_t irq_gpu2_pending;
    uint32_t fiq_control;
    uint32_t irq_gpu1_enable;   // Interrupt enable register 1 (GPU)
    uint32_t irq_gpu2_enable;   // Interrupt enable register 2 (GPU)
    uint32_t irq_basic_enable;  // Base enable register (ARM/CPU)
    uint32_t irq_gpu1_disable;
    uint32_t irq_gpu2_disable;
    uint32_t irq_basic_disable;
};


void bcm2836_timer_init() {
    uint32_t freq = get_frequency();

    TRACE("control frequency: %i", freq);
    // 1 (milli)second timer
    write_interrupt_count_value(freq * 100);

    // when the register reaches the count value set above, interrupt.
    // TODO: Enable timer
    //    mmio_write(&bcm2836_registers_base->Core0TimersInterruptControl, VIRTUAL_NONSECURE_TIMER);

    // Enable timer interrupts.
    uint32_t cntv_ctl = 1;
    asm volatile("mcr p15, 0, %0, c14, c3, 1" ::"r"(cntv_ctl));

    bcm2836_registers_base->PerformanceMonitorRoutingSet = 0xf;
}

TimerHandle bcm2836_schedule_timer_periodic(TimerCallback callback, uint32_t ms) {
    return 0;
}

TimerHandle bcm2836_schedule_timer_once(TimerCallback callback, uint32_t ms) {
    return 0;
}

void bcm2836_deschedule_timer(TimerHandle handle) {}
