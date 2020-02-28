#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__
/*
 *
 *  Interrupt handler for course_os
 *
 *
 *  - ARM has two interrupt lines to the core: FIQ (fast interrupt), IRQ (normal interrupt)
 *  - Our VIC multiplexes intterupts and feeds them to the processor as either FIQ or IRQ
 *  Basic interrupt control flow (no vectored interrupts, no nested interrupts) is as follows:
 *	Interrupt Occurs -> Core branches to FIQ or IRQ vector -> vector branches to hanlder \
 *		handler interfaces with VIC to determine source of interrupt -> branch to service routine \
 *
 */

#include <stdint.h>
#include <mmap.h>

volatile uint32_t * const PIC_ADDRESS;

// general syscall function
extern int syscall(int number);

// the VIC has 32 bits to indicate a type of interrupt
// currently we just pull a bit off the VIC and jump to that number handler
// in the handler array
// this may need to be expanded if we use the secondary controller
#define MAX_NUM_INTERRUPTS	32

typedef enum
{
	IRQ_MASK,		// (this is bit 0x8 on the CPSR)
	FIQ_MASK,		// (this is bit 0x4 on the CPSR)
	ALL_INTERRUPT_MASK
} interrupt_t;

extern interrupt_t IRQ;
extern interrupt_t FIQ;
extern interrupt_t ALL;

// this will tell if an interrupt mapping is an FIQ
// HIGH bits are FIQs
// extern char check_if_fiq[MAX_NUM_INTERRUPTS];

typedef struct interrupt_handler_t
{
	void (*handler)(void *args);
// more may need to be added
} interrupt_handler_t;

/* these are what you should use to effect an
 interrupt status change! */

#define enable_irq() \
	enable_interrupt(IRQ)
#define enable_fiq() \
	enable_interrupt(FIQ)
#define enable_interrupts() \
	enable_interrupt(ALL)
#define disable_irq() \
	disable_interrupt(IRQ)
#define disable_fiq() \
	disable_interrupt(FIQ)
#define disable_irq_save() \
	disable_interrupt_save(IRQ)
#define disable_fiq_save() \
	disable_interrupt_save(FIQ)
#define disable_interrupts() \
	disable_interrupt(ALL);
#define disable_interrupts_save() \
	disable_interrupt_save(ALL);



/* VIC Interrupt Mappings */
#define VIC_IRQ_STATUS		PIC_ADDRESS	  // status of pending irqs after masking (R)
#define VIC_FIQ_STATUS		(((volatile uint32_t *)(PIC_ADDRESS+0x004))) // status of pending fiqs after masking (R)
#define VIC_RAW_STATUS		(((volatile uint32_t *)(PIC_ADDRESS+0x008))) // pending irqs before masking by enable register (R)
#define VIC_INT_SELECT		(((volatile uint32_t *)(PIC_ADDRESS+0x00C))) // select whether source generates an IRQ or FIQ (R/W)
#define VIC_INT_ENABLE		(((volatile uint32_t *)(PIC_ADDRESS+0x010))) // actually enable interrupt lines (1 = YES) (R/W)
#define VIC_INT_ENCLEAR		(((volatile uint32_t *)(PIC_ADDRESS+0x014))) // clear enabled lines in VICINTENABLE (1=clear)
//#define VIC_VECT_ADDR		(*((volatile uint32_t *)(PIC_ADDRESS=0x030))) // the ISR of the currently active interrupt

// these should be used in conjunction with the bit shift mappings below
#define hw_interrupt_enable(n)	mmio_write(VIC_INT_ENABLE, mmio_read(VIC_INT_ENABLE) | (1 << n))
#define hw_interrupt_disable(n)	mmio_write(VIC_INT_ENCLEAR, (1 << n));
#define vic_select_fiq(n) 	mmio_write(VIC_INT_SELECT, (1 << n));

enum InterruptID {
    WATCHDOG_IRQ    = 0,    /* watchdog controller */
    SWI_IRQ         = 1,    /* software interrupt */
    COMMS_RX_IRQ    = 2,    /* debug comms receive intercept */
    COMMS_TX_IRQ    = 3,    /* debug comms transmit intercept */
    TIMER_A_IRQ     = 4,    /* timer 0 or 1 */
    TIMER_B_IRQ     = 5,    /* timer 2 or 3 */
    GPIO_A_IRQ	    = 6,	/* GPIO 0 */
    GPIO_B_IRQ		= 7,	/* GPIO 1 */
    GPIO_C_IRQ		= 8,	/* GPIO 2 */
    GPIO_D_IRQ		= 9,	/* GPIO 3 */
    RTC_IRQ		    = 10,	/* Real Time Clock (RTC) */
    SSP_IRQ		    = 11,	/* synchronous serial port */
    UART0_IRQ		= 12,	/* UART 0 */
    UART1_IRQ		= 13,	/* UART 1 */
    UART2_IRQ		= 14,	/* UART 2 */
    SCIO_IRQ		= 15,	/* smart card interface */
    CLCD_IRQ		= 16,	/* CLCD controller */
    DMA_IRQ		    = 17,	/* DMA controller */
    PWRFAIL_IRQ		= 18,	/* power failure from FPGA */
    MBX_IRQ		    = 19,	/* graphics processor */
    // IRQ 20 is reserved by the architecture
    VICINTSOURCE_21	= 21,	/* external interrupt signal from DiskOnChip flash device */
    VICINTSOURCE_22	= 22,	/* external interrupt signal from MCIO A */
    // IRQ 23 is reserved by the architecture
    VICINTSOURCE_24	= 24,	/* external interrupt signal from AACI */
    VICINTSOURCE_25	= 25,	/* Ethernet */
    VICINTSOURCE_26	= 26,	/* USB */
    VICINTSOURCE_27	= 27,	/* external interrupt signal from expansion connector */
    VICINTSOURCE_28	= 28,	/* external interrupt signal from expansion connector */
    // IRQ 29 is reserved by the architecture
    // IRQ 30 is reserved by the architecture
    VICINTSOURCE_31	= 31,	/* secondary interrupt controller (SIC) */
};

// Primary Interrupt Controller (PIC)


/*   functions (e.g. passing a bad parameter), so we'll
 refer to the macros above for adjusting specific interrupt status */
void enable_interrupt(interrupt_t);
int enable_interrupt_save(interrupt_t);

void disable_interrupt(interrupt_t);
int disable_interrupt_save(interrupt_t);

int get_proc_status(void);
void restore_proc_status(int);

int register_interrupt_handler(enum InterruptID, interrupt_handler_t *);
void handle_irq_interrupt(enum InterruptID);


#endif //__INTERRUPT_H__
