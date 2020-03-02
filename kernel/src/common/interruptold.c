/*
 *
 * Interrupts  
 *
 */
#include <interruptold.h>
#include <interrupt.h>
#include <stdio.h>
#include <memory.h>

// TODO: make consistent (old val: 0x10140000)
volatile uint32_t * const PIC_ADDRESS = (volatile uint32_t *)(0x2000b200);

// there are 32 kinds of interrupts on the VIC
// this structure may need to be expanded if the secondary controller is incorporated
static interrupt_handler_t *handlers[MAX_NUM_INTERRUPTS];

static int initialized;

// holds defined fiq interrupts
static int check_if_fiq[MAX_NUM_INTERRUPTS];


// In a system with an interrupt controller, software is required to:
// determine from the interrupt controller which interrupt source is requesting service
// determine where the service routine for that interrupt source is loaded
// mask or clear that interrupt source, before re-enabling processor interrupts to permit another interrupt to be taken.

// Interrupts must be enabled in three places:
// (1) the core (CPSR)
// (2) the VIC (interrupt controller)
// (3) (sometimes) in the device (this should be done in the device driver)

// CLear Interrupts
// Do not disable the VIC and the CSPR is disable in the hw_hanlders
// you have to clear the interrupt from the register handler
// Look At timer.c it has a great example of it.
// Here's the Website to the VIC we are using http://infocenter.arm.com/help/topic/com.arm.doc.ddi0181e/DDI0181.pdf

//They are three handlers you must use First is the (IRQ)interrupt handler its in hw_handlers.c (with attribute Irq)
// second is the ISR routine handler which is the one in interrupt.c
// third is the specific handler you created a great example is in the timer.c it is called timer_interrupt_handler.
// If youre having an error it must be in the third handler not the first two.

// Setup FIQ interrupts
void init_fiqs()
{
	check_if_fiq[11] = 1; // synchronous serial port
	check_if_fiq[17] = 1; // DMA controller  
}

void init_chipsetold() {
    init_fiqs();
    interrupt_registers = (volatile struct IRQ_registers *)PIC_ADDRESS;

    enable_interrupt(BOTH);

    interrupt_registers->irq_basic_enable = 0;
    interrupt_registers->irq_gpu1_enable = 0;
    interrupt_registers->irq_gpu2_enable = 0;
}

// when you register an interrupt handler it's line will be enabled in the VIC
// telling the device itself to fire interrupts should be done in the driver
//
// TODO: We do now ------------------------------V
// also, since we don't really have a working malloc, the handler structure will
// have to be built in the driver and passed to register_
int register_interrupt_handler(enum InterruptID num, interrupt_handler_t *handler) {
    // lazy initialization
    if (initialized != -1) {
        kprintf("INITIALIZING THE INTERRUPT SYSTEM\n");

        for (int i = 0; i < MAX_NUM_INTERRUPTS; i++) {
            handlers[i] = 0;
        }

        kprintf("INITIALIZED THE INTERRUPT SYSTEM\n");

        initialized = -1;
    }

    if (num < 0 || num > MAX_NUM_INTERRUPTS) {
        kprintf("Register a handler between 0 and %i\n", MAX_NUM_INTERRUPTS);
        panic();
    } else if (handlers[num] != NULL) {
        kprintf("Tried to re-register interrupt handler %i\n", num);
        panic();
	} else if (handler == NULL) {
        kprintf("The interrupt handler can't be NULL\n");
        panic();
    }

	// put the handler in the array
	handlers[num] = handler;

    kprintf("Enabling interrupt 0x%x\n", num);
	// enable the specific interrupt in hardware on the VIC
	interrupt_registers->irq_basic_enable |= (1 << num);

	// check to see if this is an FIQ
//	if (check_if_fiq[num]){
//        // update the "select" register on the VIC
//        vic_select_fiq(num);
//    }

	// return a success value
	return 0;
}

// handle_interrupt takes a number (the interrupt from the VIC), looks into
// the table of registered handlers, and calls the appropriate handler
void handle_irq_interrupt(enum InterruptID interrupt_vector) {
    kprintf("handling interrupt %d\n", interrupt_vector);
	// go to handler routine
    kprintf("Jumping to %X...\n", handlers[interrupt_vector]->handler);
	handlers[interrupt_vector]->handler((void *) interrupt_vector);
}



