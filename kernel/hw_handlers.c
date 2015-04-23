/*
 *
 * Harware Handler Interface  
 *
 */
#include "hw_handlers.h"
#include "mmap.h"
#include "memory.h"
#include "interrupt.h"
#include "klibc.h"
#include "vm.h"
#include "include/file.h"

/* copy vector table from wherever QEMU loads the kernel to 0x00 */
void init_vector_table(void) {
    /* This doesn't seem to work well with virtual memory; reverting
     * to old method.       
    extern uint32_t vector_table_start, vector_table_end;
    uint32_t *src = &vector_table_start;
    uint32_t *dst = (uint32_t *) HIVECTABLE;

    while(src < &vector_table_end)
	*dst++ = *src++;
    */

    /* Primary Vector Table */
    mmio_write( 0x00, BRANCH_INSTRUCTION);
    mmio_write( 0x04, BRANCH_INSTRUCTION);
    mmio_write( 0x08, BRANCH_INSTRUCTION);
    mmio_write( 0x0C, BRANCH_INSTRUCTION);
    mmio_write( 0x10, BRANCH_INSTRUCTION);
    mmio_write( 0x14, BRANCH_INSTRUCTION);
    mmio_write( 0x18, BRANCH_INSTRUCTION);
    mmio_write( 0x1C, BRANCH_INSTRUCTION);

    /* Secondary Vector Table */
    mmio_write( 0x20, &reset_handler); 
    mmio_write( 0x24, &undef_instruction_handler ); 
    mmio_write( 0x28, &software_interrupt_handler ); 
    mmio_write( 0x2C, &prefetch_abort_handler ); 
    mmio_write( 0x30, &data_abort_handler ); 
    mmio_write( 0x34, &reserved_handler ); 
    mmio_write( 0x38, &irq_handler ); 
    mmio_write( 0x3C, &fiq_handler ); 
}


/* handlers */
void reset_handler(void) {
    os_printf("RESET HANDLER\n");
    _Reset();
}

void __attribute__((interrupt("UNDEF"))) undef_instruction_handler(void){
	os_printf("UNDEFINED INSTRUCTION HANDLER\n");
}

long  __attribute__((interrupt("SWI"))) software_interrupt_handler(void){
	int callNumber;

	// the link register currently holds the address of the instruction immediately
	// after the SVC call
	// possible that syscall # passed directly in r7, not sure yet though
	register int address asm("lr"); 
	        
	// load the SVC call and mask to get the number
	callNumber = *((uint32_t *)(address-4)) & 0x00FFFFFF;

	// We have to switch VASs to the kernel's VAS if we want to do anything
	struct vas *prev_vas = vm_get_current_vas();
	vm_use_kernel_vas();

	os_printf("SOFTWARE INTERRUPT HANDLER\n");

	// Print out syscall # for debug purposes
	os_printf("Syscall #: ");
	os_printf("%x", callNumber);
	os_printf("\n");

	// System Call Handler
	switch(callNumber)
	{
	// case SYSCALL_CREATE:
	// 	os_printf("Create system call called!\n");
	// 	break;
	// case SYSCALL_SWITCH:
	// 	os_printf("Switch system call called!\n");
	// 	break;
	char* filepath;
	int error;
	char mode;
	int fd;
	void* buf;
	int numBytes;

	case SYSCALL_DELETE:
		os_printf("Delete system call called!\n");
		
		// retrieve the args that delete() put in r1 and pass to kdelete():
		asm volatile("mov r0, %[filepath1]":[filepath1]"=r" (filepath)::);
		// call kdelete(), passing appropriate args:
		error = kdelete(filepath);
		// move error that kdelete() returns to a r1 to be retrieved by delete() and returned to user:
		return (long)error;
		break;

	case SYSCALL_OPEN:
		os_printf("Open system call called!\n");

		// retrieve the args that open() put in r1, r2 and pass to kopen():
		asm volatile("mov r0, %[filepath1]":[filepath1]"=r" (filepath)::);
		asm volatile("mov r1, %[mode1]":[mode1]"=r" (mode)::);
		// call kopen(), passing appropriate args:
		fd = kopen(filepath, mode);
		// move fd that kopen() returns to a r1 to be retrieved by open() and returned to user:
		return (long) fd;
		break;

	case SYSCALL_CREATE:
		os_printf("Create system call called!\n");
		// retrieve the args that open() put in r1, r2 and pass to kopen():
		asm volatile("mov r0, %[filepath1]":[filepath1]"=r" (filepath)::);
		asm volatile("mov r1, %[mode1]":[mode1]"=r" (mode)::);
		// call kopen(), passing appropriate args:
		error = kopen(filepath, mode);
		// move fd that kopen() returns to a r1 to be retrieved by open() and returned to user:
		return (long) error;
		break;

	case SYSCALL_READ:
		os_printf("Read system call called!\n");

		// retrieve the args that read() put in r1, r2 and pass to kread():
		asm volatile("mov r0, %[fd1]":[fd1]"=r" (fd)::);
		asm volatile("mov r1, %[buf1]":[buf1]"=r" (buf)::);
		asm volatile("mov r2, %[numBytes1]":[numBytes1]"=r" (numBytes)::);
		// call kread(), passing appropriate args:
		// TODO: macro to translate process's virtual memory into kernel's view of virtual memory
		int bytesRead = kread(fd, buf, numBytes);
		// move fd that kread() returns to a r1 to be retrieved by read() and returned to user:
		return (long)bytesRead;
		break;
	case SYSCALL_WRITE:
		os_printf("Write system call called!\n");

		// retrieve the args that write() put in r1, r2 and pass to kwrite():
		asm volatile("mov r0, %[fd1]":[fd1]"=r" (fd)::);
		asm volatile("mov r1, %[buf1]":[buf1]"=r" (buf)::);
		asm volatile("mov r2, %[numBytes1]":[numBytes1]"=r" (numBytes)::);
		// call kwrite(), passing appropriate args:
		int bytesWritten = kwrite(fd, buf, numBytes);
		// move fd that kwrite() returns to a r1 to be retrieved by write() and returned to user:
		return (long) bytesWritten;
		break;
	case SYSCALL_CLOSE:
		os_printf("Close system call called!\n");

		// retrieve the args that close() put in r1 and pass to kclose():
		asm volatile("mov r0, %[fd1]":[fd1]"=r" (fd)::);
		// call kclose(), passing appropriate args:
		error = kclose(fd);
		// move error that kclose() returns to a r1 to be retrieved by close() and returned to user:
		return (long)error;
		break;
	case SYSCALL_SEEK:
		os_printf("Seek system call called!\n");

		// retrieve the args that seek() put in r1, r2 and pass to kseek():
		asm volatile("mov r0, %[fd1]":[fd1]"=r" (fd)::);
		asm volatile("mov r1, %[numBytes1]":[numBytes1]"=r" (numBytes)::);
		// call kseek(), passing appropriate args:
		error = kseek(fd, numBytes);
		// move error that kseek() returns to a r1 to be retrieved by seek() and returned to user:
		return (long)error;
		break;
	case SYSCALL_SET_PERM:
		os_printf("Set permission system call called!\n");
		return -1;
		break;
	case SYSCALL_MEM_MAP:
		os_printf("Memory map system call called!\n");
		return -1;
		break;
	default:
		os_printf("That wasn't a syscall you knob!\n");
		return -1;
		break;

	vm_enable_vas(prev_vas);
	}
}

void __attribute__((interrupt("ABORT"))) prefetch_abort_handler(void){
	os_printf("PREFETCH ABORT HANDLER\n");
}

void __attribute__((interrupt("ABORT"))) data_abort_handler(void){
	os_printf("DATA ABORT HANDLER\n");
	int pc, lr, sp, fp;
	// not sure this is correct syntax, did we [Spring 2015 do this?]
	asm volatile("mov %0, pc" : "=r" (pc));
	asm volatile("mov %0, lr" : "=r" (lr));
	asm volatile("mov %0, sp" : "=r" (sp));
	asm volatile("mov %0, fp" : "=r" (fp));
	os_printf("HANDLER: pc=%x, lr=%x, sp=%x, fp=%x\n", pc, lr, sp, fp); 
}

void reserved_handler(void){
	os_printf("RESERVED HANDLER\n");
}

// the attribute automatically saves and restores state
void __attribute__((interrupt("IRQ"))) irq_handler(void){

	os_printf("IRQ HANDLER\n");
	disable_interrupts();	

	// Discover source of interrupt
	int i = 0;
	// do a straight run through the VIC_INT_STATUS to determine
	// which interrupt lines need to be tended to
	for(i = 0; i < MAX_NUM_INTERRUPTS; i++){
		// is the line active?
		if((1 << i) & mmio_read(VIC_IRQ_STATUS)) {
			// activate that specific handler
			handle_irq_interrupt(i);	
		}
	}
	// we've gone through the VIC and handled all active interrupts
	enable_interrupts();
}

void __attribute__((interrupt("FIQ"))) fiq_handler(void){
	os_printf("FIQ HANDLER\n");
// FIQ handler returns from the interrupt by executing:
// SUBS PC, R14_fiq, #4
}
