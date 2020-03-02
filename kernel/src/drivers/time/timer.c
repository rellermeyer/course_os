
#include <timer.h>
#include <stdio.h>
#include <mmap.h>
#include <memory.h>

inline static uint32_t get_frequency() {
    uint32_t val;
    asm volatile ("mrc p15, 0, %0, c14, c0, 0" : "=r"(val) );
    return val;
}

void write_cntv_tval(uint32_t val) {
    asm volatile ("mcr p15, 0, %0, c14, c3, 0" :: "r"(val) );
    return;
}

uint32_t read_cntv_tval() {
    uint32_t val;
    asm volatile ("mrc p15, 0, %0, c14, c3, 0" : "=r"(val) );
    return val;
}

// https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2836/QA7_rev3.4.pdf
#define CORE0_TIMER_IRQCNTL BCM2836BASE
#define CORE1_TIMER_IRQCNTL BCM2836BASE + 0x04
#define CORE2_TIMER_IRQCNTL BCM2836BASE + 0x08
#define CORE3_TIMER_IRQCNTL BCM2836BASE + 0x0c

#define CORE0_IRQ_SOURCE BCM2836BASE    + 0x20
#define CORE1_IRQ_SOURCE BCM2836BASE    + 0x24
#define CORE2_IRQ_SOURCE BCM2836BASE    + 0x28
#define CORE3_IRQ_SOURCE BCM2836BASE    + 0x2c

#define CORE0_FIQ_SOURCE BCM2836BASE    + 0x30
#define CORE1_FIQ_SOURCE BCM2836BASE    + 0x34
#define CORE2_FIQ_SOURCE BCM2836BASE    + 0x38
#define CORE3_FIQ_SOURCE BCM2836BASE    + 0x3c


void timer_handler_func(void * args) {

}

//struct interrupt_handler_t timer_irq_handler =  {
//    .handler =  timer_handler_func
//};



void initialize_timers() {
//    return;

    uint32_t freq = get_frequency();

    kprintf("control frequency: %i\n", freq);
    // 1 second timer
    write_cntv_tval(freq);

    kprintf("value: %i\n", read_cntv_tval());

    mmio_write(CORE0_TIMER_IRQCNTL, 0x08);

    uint32_t cntv_ctl = 1;
    asm volatile ("mcr p15, 0, %0, c14, c3, 1" :: "r"(cntv_ctl) ); // write CNTV_CTL

//    register_handler(, timer_handler)
//    register_interrupt_handler(4, &timer_irq_handler);
//    register_interrupt_handler(3, &timer_irq_handler);


}





int start_timer_interrupts( int timer_index, int star_val) {
    return 0;
}

int register_handler(int timer_index, void (*handler)(void *args)) {
    return 0;
}

int unregister_handler(int timer_index) {
    return 0;
}


