#include <chipset.h>
#include <hardwareinfo.h>
#include <interrupt.h>
#include <klibc.h>
#include <mem_alloc.h>
#include <pmm.h>
#include <stdint.h>
#include <syscall.h>
#include <test.h>
#include <vas2.h>
#include <vm2.h>

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
    switch_to_vas(vas);
    allocate_page(vas, 0x0, true);
    syscall(SYS_dummy, 1, 2, 3, 4);

    // DEBUG
    // TODO: load address space here
    // asm volatile("msr spsr_c, #0x10");    // load user mode to spsr
    // asm volatile("mov lr, #0x60000000");  // set lr to (almost) user entry point
    // asm volatile("add lr, #-8");          // adjust lr
    // asm volatile("movs sp, lr");          // jump to user and load spsr into cpsr

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

    INFO("End of boot sequence.\n");
    SLEEP;
}
