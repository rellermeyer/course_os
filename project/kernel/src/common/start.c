#include <chipset.h>
#include <constants.h>
#include <debug.h>
#include <dtb.h>
#include <hardwareinfo.h>
#include <interrupt.h>
#include <klibc.h>
#include <loader.h>
#include <mem_alloc.h>
#include <pmm.h>
#include <scheduler.h>
#include <stdint.h>
#include <string.h>
#include <test.h>
#include <vas2.h>
#include <vm2.h>

#define EXEC_FILE_ADDRESS  ((void *) (__PROCESS_START))

extern const size_t __PROCESS_START[];
void call_init_state(Elf32_Addr pc, Elf32_Addr sp, Elf32_Addr pcb);

void init() {
    ProcessControlBlock * pcb = createPCB(0);
    add(pcb, true);
    getNext();
    allocate_page(pcb->vas, 0x8000, true);

    int available_mem_addr = 0x8004;

    // copy the SWI instruction from _userspace_test_program to the allocated page at 0x8000
    int userspace_test_program = 0;
    asm volatile("ldr %0, =_userspace_test_program" : "=r"(userspace_test_program));

    // TODO size of userspace test program is hardcoded
    kprintf("userspace test: %x\n", userspace_test_program);
    memcpy((void *)available_mem_addr, (void *)userspace_test_program, (size_t)60);

    register ProcessControlBlock * r5 asm("r5") = pcb;
    asm volatile("push {lr}");
    asm volatile("bl _init");
    asm volatile("pop {lr}");

}


void load_process() {
    char *file = EXEC_FILE_ADDRESS;
    struct ProcessControlBlock *pcb = createPCB(0);
    stack_and_heap_and_entry stackAndHeapAndEntry;

    union loader_result input;
    input.static_exec_result.PCB = pcb;
    input.static_exec_result.stackAndHeapAndEntry = &stackAndHeapAndEntry;

    int result = load_elf_file(file, &input);

    if (result < 0) {
        kprintf("Loader operation failed due to error %d\n", result);
        return;
    }

    add(pcb, true);
    getNext();

    call_init_state(stackAndHeapAndEntry.entry, stackAndHeapAndEntry.stack_pointer, (Elf32_Addr) pcb);

    INFO("Stack initialized successfully!\n");

}


/// Entrypoint for the C part of the kernel.
/// This function is called by the assembly located in [startup.s].
/// The MMU has already been initialized here but only the first MiB of the kernel has been mapped.
void start(uint32_t * p_bootargs, struct DTHeader * dtb) {
    4 + 2;

    // Before this point, all code has to be hardware independent.
    // After this point, code can request the hardware info struct to find out what
    // Code should be ran.
    init_hardwareinfo(dtb);

    // Initialize the chipset and enable uart
    init_chipset();


    INFO("Detected memory size: 0x%x Bytes", get_hardwareinfo()->memory_size);
    INFO("Started chipset specific handlers");

    // just cosmetic (and for debugging)
    print_hardwareinfo();

    // start proper virtual and physical memory management.
    // Even though we already enabled the mmu in startup.s to
    // create a higher half kernel. The pagetable created there
    // was temporary and has to be replaced here.
    // This will actually map the whole kernel in memory and initialize the physicalMemoryManager.
    INFO("Initializing the physical and virtual memory managers.");
    vm2_start(get_hardwareinfo()->memory_size);

    INFO("Setting up heap");
    // After this point kmalloc and kfree can be used for dynamic memory management.
    init_heap();

    // Allow the kernel to attach to the debugger, this can be done
    // earlier by rewriting to make it use hardcoded buffer sizes
    // instead.
    debug_init();

    INFO("Setting up interrupt vector tables");
    // Set up the exception handlers.
    init_vector_table();

    // Splash screen
    splash();

    // Turn on interrupts
    enable_interrupt(BOTH);

    // Call the chipset again to do any initialization after enabling interrupts and the heap.
    chipset.late_init();

    #ifndef ENABLE_TESTS
    // DEBUG

    init_scheduler();
    load_process();

    init();
    init();
    asm volatile("b _switch_to_usermode");
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
