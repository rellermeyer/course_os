#include <chipset.h>
#include <hardwareinfo.h>
#include <interrupt.h>
#include <klibc.h>
#include <mem_alloc.h>
#include <stdint.h>
#include <test.h>
#include <vm2.h>
#include <vas2.h>
#include <syscall.h>
#include <string.h>

extern unsigned int user_start;
extern unsigned int user_end;

/// Entrypoint for the C part of the kernel.
/// This function is called by the assembly located in [startup.s].
/// The MMU has already been initialized here but only the first MiB of the kernel has been mapped.
void start(uint32_t * p_bootargs) {
    // Before this point, all code has to be hardware independent.
    // After this point, code can request the hardware info struct to find out what
    // Code should be ran.
    init_hardwareinfo();

    // Initialize the chipset and enable uart
    init_chipset();

    INFO("Started chipset specific handlers");

    // just cosmetic (and for debugging)
    print_hardwareinfo();
    detect_boardtype();

    // start proper virtual and physical memory management.
    // Even though we already enabled the mmu in startup.s to
    // create a higher half kernel. The pagetable created there
    // was temporary and has to be replaced here.
    // This will actually map the whole kernel in memory and initialize the physicalMemoryManager.
    INFO("Initializing the physical and virtual memory managers.");
    vm2_start();

    INFO("Setting up interrupt vector tables");
    // Set up the exception handlers.
    init_vector_table();

    INFO("Setting up heap");
    // After this point kmalloc and kfree can be used for dynamic memory management.
    init_heap();

    // Splash screen
    splash();

    // Turn on interrupts
    enable_interrupt(BOTH);

    // Call the chipset again to do any initialization after enabling interrupts and the heap.
    chipset.late_init();


#ifndef ENABLE_TESTS
    // DEBUG
    struct vas2 * vas = create_vas();
    allocate_page(vas, 0x500000, true);
    // TODO memcpy?
    //void *start = (void *)&user_start;
    //void *end = (void *)&user_end;
    //size_t size = end - start;
    //memcpy(vas, start, size);
    switch_to_vas(vas);
    kprintf("vas : %x\n", vas->l1PageTable);

    int mode = 0;
    asm ("mrs %0, cpsr" : "=r" (mode));
    kprintf("mode: %x\n", mode);
    asm volatile("bl _switch_to_usermode");
    //asm("swi 0x0");
    // DEBUG
#else
    test_main();
    // If we return, the tests failed.
    SemihostingCall(OSSpecific);
#endif

    // TODO:
    //  * Mount vfs
    //  * Load initramfs into tmpfs
    //  * execute userland init program

    asm volatile("cpsie i");

    //asm volatile("mov sp, #0x80000000");
    //asm volatile("MSR     CPSR_c, #0x10");
    //asm volatile("MOV lr, pc");
    //asm volatile("bl asdf");
    //asm("MOV     SP, R0");
    //asm volatile("mov r0, #0x10");
    //asm volatile("msr SPSR, r0");
    //asm("movs pc, lr");
    //kprintf("heree");
    //syscall(SYS_dummy, 1, 2, 3, 4);
    INFO("End of boot sequence.\n");
    SLEEP;
}
