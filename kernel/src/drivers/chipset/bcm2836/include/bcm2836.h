// Data sheet: https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2836/QA7_rev3.4.pdf
#ifndef BCM2836_H
#define BCM2836_H

#include <stdint.h>

#define BCM2836_REGISTERS_PHYSICAL_BASE     0x40000000
#define BCM2836_PERIPHERALS_PHYSICAL_BASE   0x3F000000

// Starts at memory address 0x4000_0000
struct BCM2836Registers {
    uint32_t ControlRegister;
    uint32_t __unused1;
    uint32_t CoreTimerPrescaler;
    uint32_t GPUInterruptsRouting;
    uint32_t PerformanceMonitorRoutingSet;
    uint32_t PerformanceMonitorRoutingClear;
    uint32_t __unused2;
    uint32_t CoreTimerAccessLS32Bits;
    uint32_t CoreTimerAccessMS32Bits;
    uint32_t LocalInterupts0Routing;
    uint32_t __LocalInterupts8Routing __attribute__ ((deprecated)); // Deprecated
    uint32_t AxiOutstandingCounters;
    uint32_t AxiOutstandingIRQ;
    uint32_t LocalTimerControlAndStatus;
    uint32_t LocalTimerWriteFlags;
    uint32_t __unused3;
    uint32_t Core0TimersInterruptControl;
    uint32_t Core1TimersInterruptControl;
    uint32_t Core2TimersInterruptControl;
    uint32_t Core3TimersInterruptControl;
    uint32_t Core0MailboxInterruptControl;
    uint32_t Core1MailboxInterruptControl;
    uint32_t Core2MailboxInterruptControl;
    uint32_t Core3MailboxInterruptControl;
    uint32_t Core0IRQSource;
    uint32_t Core1IRQSource;
    uint32_t Core2IRQSource;
    uint32_t Core3IRQSource;
    uint32_t Core0FIQSource;
    uint32_t Core1FIQSource;
    uint32_t Core2FIQSource;
    uint32_t Core3FIQSource;
    uint32_t Core0Mailbox0WriteSet;
    uint32_t Core0Mailbox1WriteSet;
    uint32_t Core0Mailbox2WriteSet;
    uint32_t Core0Mailbox3WriteSet;
    uint32_t Core1Mailbox0WriteSet;
    uint32_t Core1Mailbox1WriteSet;
    uint32_t Core1Mailbox2WriteSet;
    uint32_t Core1Mailbox3WriteSet;
    uint32_t Core2Mailbox0WriteSet;
    uint32_t Core2Mailbox1WriteSet;
    uint32_t Core2Mailbox2WriteSet;
    uint32_t Core2Mailbox3WriteSet;
    uint32_t Core3Mailbox0WriteSet;
    uint32_t Core3Mailbox1WriteSet;
    uint32_t Core3Mailbox2WriteSet;
    uint32_t Core3Mailbox3WriteSet;
    uint32_t Core0Mailbox0ReadClear;
    uint32_t Core0Mailbox1ReadClear;
    uint32_t Core0Mailbox2ReadClear;
    uint32_t Core0Mailbox3ReadClear;
    uint32_t Core1Mailbox0ReadClear;
    uint32_t Core1Mailbox1ReadClear;
    uint32_t Core1Mailbox2ReadClear;
    uint32_t Core1Mailbox3ReadClear;
    uint32_t Core2Mailbox0ReadClear;
    uint32_t Core2Mailbox1ReadClear;
    uint32_t Core2Mailbox2ReadClear;
    uint32_t Core2Mailbox3ReadClear;
    uint32_t Core3Mailbox0ReadClear;
    uint32_t Core3Mailbox1ReadClear;
    uint32_t Core3Mailbox2ReadClear;
    uint32_t Core3Mailbox3ReadClear;
};

enum InterruptSource {
    // nCNTPSIRQ : Secure physical timer event
    PHYSICAL_SECURE_TIMER       = (1u << 0u),

    // nCNTPNSIRQ : Non-secure physical timer event
    PHYSICAL_NONSECURE_TIMER    = (1u << 1),

    // nCNTHPIRQ: Physical Timer for use in Hypervisor mode.
    PHYSICAL_HYPERVISOR_TIMER   = (1 << 2),

    // nCNTVIRQ: Virtual Timer for use in Non-secure PL1 modes.
    VIRTUAL_NONSECURE_TIMER     = (1 << 3),

    MAILBOX_0                   = (1 << 4),
    MAILBOX_1                   = (1 << 5),
    MAILBOX_2                   = (1 << 6),
    MAILBOX_3                   = (1 << 7),

    GPU                         = (1 << 8),
    PMU                         = (1 << 9),

    AXI                         = (1 << 10),
    LOCAL_TIMER                 = (1 << 11),
};

void bcm2836_init();

volatile struct BCM2836Registers * bcm2836_registers_base;
size_t bcm2836_peripheral_base;

#endif
