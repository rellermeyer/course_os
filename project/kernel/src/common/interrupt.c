#include <chipset.h>
#include <interrupt.h>
#include <interrupt_handler.h>
#include <mmio.h>
#include <stdio.h>
#include <vm2.h>
#include <debug.h>

/* copy vector table from wherever QEMU loads the kernel to 0x00 */
void init_vector_table() {
    // allocate space for the IVR at the high vector location.
    vm2_allocate_page(kernell1PageTable,
                      HIGH_VECTOR_LOCATION,
                      false,
                      (struct PagePermission){.access = KernelRW, .executable = true},
                      NULL);

    /* Primary Vector Table */
    mmio_write(HIGH_VECTOR_LOCATION + 0x00, BRANCH_INSTRUCTION);
    mmio_write(HIGH_VECTOR_LOCATION + 0x04, BRANCH_INSTRUCTION);
    mmio_write(HIGH_VECTOR_LOCATION + 0x08, BRANCH_INSTRUCTION);
    mmio_write(HIGH_VECTOR_LOCATION + 0x0C, BRANCH_INSTRUCTION);
    mmio_write(HIGH_VECTOR_LOCATION + 0x10, BRANCH_INSTRUCTION);
    mmio_write(HIGH_VECTOR_LOCATION + 0x14, BRANCH_INSTRUCTION);
    mmio_write(HIGH_VECTOR_LOCATION + 0x18, BRANCH_INSTRUCTION);
    mmio_write(HIGH_VECTOR_LOCATION + 0x1C, BRANCH_INSTRUCTION);

    /* Secondary Vector Table */
    mmio_write(HIGH_VECTOR_LOCATION + 0x20, &reset_handler);
    mmio_write(HIGH_VECTOR_LOCATION + 0x24, &undef_instruction_handler);
    mmio_write(HIGH_VECTOR_LOCATION + 0x28, &handle_swi);
    mmio_write(HIGH_VECTOR_LOCATION + 0x2C, &prefetch_abort_handler);
    mmio_write(HIGH_VECTOR_LOCATION + 0x30, &data_abort_handler);
    mmio_write(HIGH_VECTOR_LOCATION + 0x34, &reserved_handler);
    mmio_write(HIGH_VECTOR_LOCATION + 0x38, &handle_irq);
    mmio_write(HIGH_VECTOR_LOCATION + 0x3C, &fiq_handler);

    /// Enable high vectors (Vectors located at HIGH_VECTOR_LOCATION).
    asm volatile("mrc p15, 0, r1, c1, c0, 0 \n"  // Read p15
                 "orr r1, %0\n"                  // Enable High Vector bit
                 "mcr p15, 0, r1, c1, c0, 0\n"   // Set p15
                 ::"r"(1u << 13u));
}

/* handlers */
void reset_handler(void) {
    INFO("RESET HANDLER\n");
    _Reset();
}


void __attribute__((interrupt("UNDEF"))) undef_instruction_handler() {
    size_t spsr, lr;

    asm volatile("mrs %0, spsr" : "=r"(spsr));
    asm volatile("mov %0, lr" : "=r"(lr));

    size_t thumb = spsr & 0x20u;
    size_t pc = thumb ? lr - 0x2u : lr - 0x4u;


    WARN("UNDEFINED INSTRUCTION HANDLER");

    size_t copro = (*(size_t *)pc & 0xf00000u) >> 24u;

    if (spsr & 0x20u) {
        WARN("THUMB mode");
    } else {
        WARN("ARM mode");
    }
    if (spsr & 0x1000000u) { WARN("JAZELLE enabled"); }

    WARN("COPRO: 0x%x", copro);
    WARN("violating instruction (at 0x%x): 0x%x", pc, *((size_t *)pc));
    if (pc >= KERNEL_VIRTUAL_START && pc < KERNEL_VIRTUAL_END) {
        WARN("(instruction is in kernel address range)");
    }

    FATAL("UNDEFINED INSTRUCTION HANDLER");
}

long syscall_handler(void) {
    register int reg7 asm("r7");
    register int reg0 asm("r0");
    register int reg1 asm("r1");
    register int reg2 asm("r2");
    register int reg3 asm("r3");
    int callNumber = reg7, r0 = reg0, r1 = reg1, r2 = reg2, r3 = reg3;

    kprintf("SOFTWARE INTERRUPT HANDLER\n");

    // Print out syscall # for debug purposes
    kprintf("Syscall #: ");
    kprintf("%d\n", callNumber);
    kprintf("arg0=%d\n", r0);
    kprintf("arg1=%d\n", r1);
    kprintf("arg2=%d\n", r2);
    kprintf("arg3=%d\n", r3);
    kprintf("\n");

    // System Call Handler
    switch (callNumber) {
        case SYSCALL_EXIT:
            // TODO: remove current process from scheduler
            kprintf("BYE :)");
            for (;;)
                ;
            break;
        case SYSCALL_DUMMY:
            return 0L;
            break;

        // NOTE: All FS syscalls have been *DISABLED* until the filesystem works again.
        case SYSCALL_CREATE:
            kprintf("Create system call called!\n");
            debug_run();
            return 0;

            //      return (long) kcreate((char*) r0, r1, 0);
        case SYSCALL_DELETE:
            kprintf("Delete system call called!\n");
            return -1;

            //      return (long) kdelete((char*) r0, 1);
        case SYSCALL_OPEN:
            kprintf("Open system call called!\n");
            return -1;

            //      return (long) kopen((char*) r0, r1);
        case SYSCALL_MKDIR:
            kprintf("Mkdir system call called!\n");
            return -1;

            //      return (long) kcreate((char*) r0, 'w', 1);
        case SYSCALL_READ:
            kprintf("Read system call called!\n");
            return -1;

            //      return (long) kread(r0, (void*) r1, r2);
        case SYSCALL_WRITE:
            kprintf("Write system call called!\n");
            return -1;

            //      return (long) kwrite(r0, (void*) r1, r2);
        case SYSCALL_CLOSE:
            kprintf("Close system call called!\n");
            return -1;

            //      return (long) kclose(r0);
        case SYSCALL_SEEK:
            kprintf("Seek system call called!\n");
            return -1;

            //      return (long) kseek(r0, r1);
        case SYSCALL_COPY:
            kprintf("Copy system call called!\n");
            return -1;

            //      return (long) kcopy((char*) r0, (char*) r1, r2);
        case SYSCALL_LS:
            kprintf("Ls system call called!\n");
            return -1;
            //      return (long) kls((char*) r0);
        case SYSCALL_SET_PERM:
            kprintf("Set permission system call called!\n");
            kprintf("Yet to be implemented\n");
            return -1;
        case SYSCALL_MEM_MAP:
            kprintf("Memory map system call called!\n");
            kprintf("Yet to be implemented\n");
            return -1;

        case SYSCALL_PRINTF:
            kprintf("Printf system call called!\n");

            //            kprintf((const char *)r0);
            return 0L;
        default:
            kprintf("That wasn't a syscall you knob!\n");
            return 0x0;
    }
}

void __attribute__((interrupt("ABORT"))) prefetch_abort_handler(void) {
    size_t lr;
    asm volatile("mov %0, lr" : "=r"(lr));

    FATAL("PREFETCH ABORT HANDLER, violating address: 0x%x", (lr - 4u));
}

void __attribute__((interrupt("ABORT"))) data_abort_handler(void) {
    // TODO Check if the address is valid according to the kernel and add it to the currently loaded
    // pagetables if so.

    int lr;
    asm volatile("mov %0, lr" : "=r"(lr));
    int pc = lr - 8;

    uint32_t far;
    asm volatile("mrc p15, 0, %0, c6, c0, 0" : "=r"(far));

    WARN("DATA ABORT HANDLER (Page Fault)");
    WARN("faulting address: 0x%x", far);
    if (far >= KERNEL_VIRTUAL_OFFSET) { DEBUG("(address is in kernel address range)"); }
    WARN("violating instruction (at 0x%x): 0x%x", pc, *((int *)pc));

    // Get the Data Fault Status Register
    int dfsr;
    asm volatile("MRC p15, 0, %0, c5, c0, 0" : "=r"(dfsr));
    WARN("DFSR: 0x%x", dfsr);


#ifdef ENABLE_TESTS
    FATAL("Data abort is disallowed in tests");
#endif
}

void reserved_handler(void) {
    INFO("RESERVED HANDLER\n");
}

void __attribute__((interrupt("FIQ"))) fiq_handler(void) {
    DEBUG("FIQ HANDLER\n");
    return chipset.handle_fiq();
}

void __attribute__((always_inline)) inline SemihostingCall(enum SemihostingSWI mode) {
    int a = mode;
    asm volatile("MOV r0, #0x18\n"
                 "LDR r1, %[in0]\n"
                 "svc 0x00123456\n"
                 :
                 : [in0] "m"(a));
}


/// Uses the ExtendedExit Semihosting call
/// ARM Docs:
/// https://developer.arm.com/docs/100863/0200/semihosting-operations/sys_exit_extended-0x20
void __attribute__((always_inline)) inline SemihostingOSExit(uint8_t code) {
    struct {
        uint32_t f1;
        uint32_t f2;
    } parameters = {ApplicationExit, code};

    asm volatile("MOV r0, #0x20\n"
                 "mov r1, %[in0]\n"
                 "svc 0x00123456\n" ::[in0] "r"(&parameters));

    __builtin_unreachable();
}

/* enable IRQ and/or FIQ */
void enable_interrupt(InterruptType mask) {
    INFO("Enabling interrupts with mask 0x%x", mask);

    // enable interrupt on the core
    switch (mask) {
        case IRQ:
            asm volatile("cpsie i");
            break;
        case FIQ:
            asm volatile("cpsie f");
            break;
        case BOTH:
            asm volatile("cpsie if");
            break;
        default:
            /** should never happen **/
            WARN("invalid interrupt mask");
    }
}

/* disable IRQ and/or FIQ */
void disable_interrupt(InterruptType mask) {
    INFO("Disabling interrupts with mask 0x%x", mask);

    // disable interrupts on the core
    switch (mask) {
        case IRQ:
            asm volatile("cpsid i");
            break;
        case FIQ:
            asm volatile("cpsid f");
            break;
        case BOTH:
            asm volatile("cpsid if");
            break;
        default:
            /** should never happen **/
            WARN("invalid interrupt mask");
    }
}

/* disable IRQ and/or FIQ, but also return a copy of the CPSR */
int disable_interrupt_save(InterruptType mask) {
    INFO("Disabling interrupts (save) with mask 0x%x", mask);

    /* get a copy of the current process status register */
    int cpsr;
    asm volatile("mrs %0, cpsr" : "=r"(cpsr));
    // disable interrupts on the core
    switch (mask) {
        case IRQ:
            asm volatile("cpsid i");
            break;
        case FIQ:
            asm volatile("cpsid f");
            break;
        case BOTH:
            asm volatile("cpsid if");
            break;
        default:
            /** should never happen **/
            WARN("invalid interrupt mask");
    }
    return cpsr;
}

/* return a full 32-bit copy of the current process status register */
size_t get_proc_status() {
    size_t cpsr;
    asm volatile("mrs %0, cpsr" : "=r"(cpsr));
    return cpsr;
}

/* restore control status (interrupt, mode bits) of the cpsr */
/* (e.g. when we return from a handler, restore value from
 disable_interrupt_save                  */
void restore_proc_status(size_t cpsr) {
    asm volatile("msr cpsr_c, %0" : : "r"(cpsr));
}
