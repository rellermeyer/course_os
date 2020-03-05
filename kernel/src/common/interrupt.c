#include <interrupt.h>
#include <mmap.h>
#include <memory.h>
#include <interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <vm.h>
#include <chipset.h>

/* copy vector table from wherever QEMU loads the kernel to 0x00 */
void init_vector_table() {
	/* This doesn't seem to work well with virtual memory; reverting
	 * to old method.       
	 extern uint32_t vector_table_start, vector_table_end;
	 uint32_t *src = &vector_table_start;
	 uint32_t *dst = (uint32_t *) HIVECTABLE;

	 while(src < &vector_table_end)
	 *dst++ = *src++;
	 */

	/* Primary Vector Table */
	mmio_write(0x0, BRANCH_INSTRUCTION);
	mmio_write(0x04, BRANCH_INSTRUCTION);
	mmio_write(0x08, BRANCH_INSTRUCTION);
	mmio_write(0x0C, BRANCH_INSTRUCTION);
	mmio_write(0x10, BRANCH_INSTRUCTION);
	mmio_write(0x14, BRANCH_INSTRUCTION);
	mmio_write(0x18, BRANCH_INSTRUCTION);
	mmio_write(0x1C, BRANCH_INSTRUCTION);

    /* Secondary Vector Table */
	mmio_write(0x20, &reset_handler);
	mmio_write(0x24, &undef_instruction_handler);
	mmio_write(0x28, &software_interrupt_handler);
	mmio_write(0x2C, &prefetch_abort_handler);
	mmio_write(0x30, &data_abort_handler);
	mmio_write(0x34, &reserved_handler);
	mmio_write(0x38, &irq_handler);
	mmio_write(0x3C, &fiq_handler);

}

/* handlers */
void reset_handler(void)
{
    kprintf("RESET HANDLER\n");
	_Reset();
}

void __attribute__((interrupt("UNDEF"))) undef_instruction_handler(void)
{
	int spsr, lr;

	asm volatile("mrs %0, spsr" : "=r"(spsr));
	asm volatile("mov %0, lr" : "=r" (lr));

	int thumb = spsr & 0x20;
	int pc = thumb ? lr - 0x2 : lr - 0x4;

	if ((*(size_t *) pc) == UNDEFINED_INSTRUCTION_BYTES){
        kprintf("FATAL ERROR\n");
        panic();
	}

	kprintf("UNDEFINED INSTRUCTION HANDLER\n");

    int copro = (*(int*)pc & 0xf00000) >> 24;

	if (spsr & 0x20) {
        kprintf("THUMB mode\n");
	} else {
        kprintf("ARM mode\n");
	}
	if (spsr & 0x1000000) {
        kprintf("JAZELLE enabled\n");
	}

    kprintf("COPRO: %x\n", copro);
    kprintf("violating instruction (at %x): %x\n", pc, *((int *) pc));
	if (pc >= V_KERNBASE && pc < V_KERNTOP)
	{
        kprintf("(instruction is in kernel address range)\n");
	}

	panic();
}

long __attribute__((interrupt("SWI"))) software_interrupt_handler(void)
{
	int callNumber = 0, r0 = 0, r1 = 0, r2 = 0, r3 = 0;

	asm volatile ("MOV %0, r7":"=r"(callNumber)::);
	asm volatile ("MOV %0, r0":"=r"(r0)::);
	asm volatile ("MOV %0, r1":"=r"(r1)::);
	asm volatile ("MOV %0, r2":"=r"(r2)::);
	asm volatile ("MOV %0, r3":"=r"(r3)::);

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
	switch (callNumber)
	{
	case SYSCALL_EXIT:
		// TODO: remove current process from scheduler
		for (;;);
		break;
	case SYSCALL_DUMMY:
		return 0L;
		break;

    // NOTE: All FS syscalls have been *DISABLED* until the filesystem works again.
	case SYSCALL_CREATE:
        kprintf("Create system call called!\n");
		return -1;

//		return (long) kcreate((char*) r0, r1, 0);
	case SYSCALL_DELETE:
        kprintf("Delete system call called!\n");
		return -1;

//		return (long) kdelete((char*) r0, 1);
	case SYSCALL_OPEN:
        kprintf("Open system call called!\n");
		return -1;

//		return (long) kopen((char*) r0, r1);
	case SYSCALL_MKDIR:
        kprintf("Mkdir system call called!\n");
		return -1;

//		return (long) kcreate((char*) r0, 'w', 1);
	case SYSCALL_READ:
        kprintf("Read system call called!\n");
		return -1;

//		return (long) kread(r0, (void*) r1, r2);
	case SYSCALL_WRITE:
        kprintf("Write system call called!\n");
		return -1;

//		return (long) kwrite(r0, (void*) r1, r2);
	case SYSCALL_CLOSE:
        kprintf("Close system call called!\n");
		return -1;

//		return (long) kclose(r0);
	case SYSCALL_SEEK:
        kprintf("Seek system call called!\n");
		return -1;

//		return (long) kseek(r0, r1);
	case SYSCALL_COPY:
        kprintf("Copy system call called!\n");
		return -1;

//		return (long) kcopy((char*) r0, (char*) r1, r2);
	case SYSCALL_LS:
        kprintf("Ls system call called!\n");
		return -1;
//		return (long) kls((char*) r0);
	case SYSCALL_SET_PERM:
        kprintf("Set permission system call called!\n");
            kprintf("Yet to be implemented\n");
		return -1;
	case SYSCALL_MEM_MAP:
        kprintf("Memory map system call called!\n");
            kprintf("Yet to be implemented\n");
		return -1;

	case SYSCALL_MALLOC:
        kprintf("malloc system call called!\n");

		void *ptr = umalloc(r0);

            kprintf("malloc is about to return %x\n", ptr);

		return (long) ptr;
	case SYSCALL_ALIGNED_ALLOC:
        kprintf("aligned_alloc system call called!\n");
		void *ptr2 = ualigned_alloc(r0, r1);

            kprintf("ualigned_alloc is about to return %x\n", ptr2);

		return (long) ptr2;
	case SYSCALL_FREE:
        kprintf("Free system call called!\n");

		ufree((void*) r0);
		return 0L;
	case SYSCALL_PRINTF:
        kprintf("Printf system call called!\n");

            kprintf((const char *) r0);
		return 0L;
	default:
        kprintf("That wasn't a syscall you knob!\n");
		return -1L;
	}
}

void __attribute__((interrupt("ABORT"))) prefetch_abort_handler(void)
{
	int lr;

	asm volatile("mov %0, lr" : "=r" (lr));

    kprintf("PREFETCH ABORT HANDLER, violating address: %x\n", (lr - 4));

	panic();
}

void __attribute__((interrupt("ABORT"))) data_abort_handler(void)
{
	int lr;
	asm volatile("mov %0, lr" : "=r" (lr));
	int pc = lr - 8;

	int far;
	asm volatile("mrc p15, 0, %0, c6, c0, 0" : "=r" (far));

    kprintf("DATA ABORT HANDLER (Page Fault)\n");
    kprintf("faulting address: 0x%x\n", far);
	if (far >= V_KDSBASE) {
        kprintf("(address is in kernel address range)\n");
	}
    kprintf("violating instruction (at 0x%x): %x\n", pc, *((int *) pc));

	// Get the DSFR
	int dsfr;
	asm volatile("MRC p15, 0, %0, c5, c0, 0" : "=r" (dsfr));
	//os_printf("DSFR: 0x%X\n", dsfr);

#ifdef ENABLE_TESTS
    panic();
#endif

	switch (dsfr)
	{
	case 6: // Access bit.
		// Set it to 1 so we don't get notified again.
		// TODO: The eviction policy will listen to this.
		*((unsigned int*) (V_L1PTBASE + 2 * PAGE_TABLE_SIZE)) |= (1 << 4);
		break;
	default:
		break;
	};
}

void reserved_handler(void)
{
    kprintf("RESERVED HANDLER\n");
}

// the attribute automatically saves and restores state
void __attribute__((interrupt("IRQ"))) irq_handler(void) {
    kprintf("IRQ HANDLER\n");
    return chipset.handle_irq();

//    int * pendingregister = (int *) 0x40000060;
//	int cpsr = disable_interrupt_save(IRQ);

    //os_printf("disabled CSPR:%X\n",cpsr);
	// Discover source of interrupt
	// do a straight run through the VIC_INT_STATUS to determine
	// which interrupt lines need to be tended to
//	for (int i = 0; i < MAX_NUM_INTERRUPTS; i++) {
//		// is the line active?
//		if ((1 << i) & mmio_read(&interrupt_registers->irq_basic_pending)) {
//			// activate that specific handler
//			handle_irq_interrupt(i);
//		}
//	}
	// we've gone through the VIC and handled all active interrupts
    // restore_proc_status(cpsr);

    //	enable_interrupt(IRQ_MASK);

}

void __attribute__((interrupt("FIQ"))) fiq_handler(void) {
    kprintf("FIQ HANDLER\n");
    return chipset.handle_fiq();

//    handle_irq_interrupt(interrupt_registers->fiq_control & 0x7f);


// FIQ handler returns from the interrupt by executing:
// SUBS PC, R14_fiq, #4
}

void SemihostingCall(enum SemihostingSWI mode) {

    int a = mode;
    asm volatile (
        "MOV r0, #0x18\n"
        "LDR r1, %[in0]\n"
        "svc 0x00123456\n"
        :
        : [in0] "m"  (a)
    );
}

/* enable IRQ and/or FIQ */
void enable_interrupt(InterruptType mask) {
    kprintf("Enabling interrupts with mask %i\n", mask);

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
    }
}

/* disable IRQ and/or FIQ */
void disable_interrupt(InterruptType mask) {
    kprintf("Disabling interrupts with mask %i\n", mask);

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
    }
}

/* disable IRQ and/or FIQ, but also return a copy of the CPSR */
int disable_interrupt_save(InterruptType mask) {

    kprintf("Disabling interrupts (save) with mask %i\n", mask);

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
    }
    return cpsr;
}

/* return a full 32-bit copy of the current process status register */
int get_proc_status() {
    int cpsr;
    asm volatile("mrs %0, cpsr" : "=r"(cpsr));
    return cpsr;
}

/* restore control status (interrupt, mode bits) of the cpsr */
/* (e.g. when we return from a handler, restore value from
 disable_interrupt_save				     */
void restore_proc_status(int cpsr) {
    asm volatile("msr cpsr_c, %0" : : "r"(cpsr));
}
