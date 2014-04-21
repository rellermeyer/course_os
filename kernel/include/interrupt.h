#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__
/*
 *
 *  Interrupt handler four course_os
 *
 *  A bit of backgroundf:
 *  - The ARM architecture has 7 modes of operation:
 * 	+ USR - user mode
 *	+ FIQ - processing "fast" interrupts
 *	+ IRQ - processing "normal" interrupts
 *	+ SVC - proctected mode for OS
 *	+ UND - processing an undefined instruction exception
 *	+ SYS - also protecteed mode for OS --if anyone wants to clarify, feel free--
 *	These modes can be entered or exited by modifying the CPSR (status register)
 *  - When an interrupt occurs, the apropriate bits are pulled 
 *
 *  - ARM has two interrupt lines: FIQ (fast interrupt), IRQ (normal interrupt)
 *  - Our VIC multiplexes intterupts from multiple sources and feeds them to the processor as either FIQ or IRQ
 *  Basic interrupt control flow (no vectored interrupts, no nested interrupts) is as follows:
 *	Interrupt Occurs -> Core branches to FIQ or IRQ vector -> vector branches to hanlder \
 *		handler interfaces with VIC to determine source of interrupt -> branch to service routine \
 *
 */

#include <stdint.h>
#include "mmap.h"

// general syscall function
extern int syscall(int number);

typedef enum {
        IRQ_MASK,		// (this is bit 0x8 on the CPSR)
        FIQ_MASK,		// (this is bit 0x4 on the CPSR)
        ALL_INTERRUPT_MASK
} interrupt_t;

extern interrupt_t IRQ;
extern interrupt_t FIQ;
extern interrupt_t ALL;

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


/* we don't really wan't others mucking around with the interrupt state
   functions (e.g. passing a bad parameter), so we'll  
   refer to the macros above for adjusting specific interrupt status */
inline int	enableInterrupt(interrupt_t); // deprecated
inline void 	handle_interrupt(int);
inline void	enable_interrupt(interrupt_t);
inline int	enable_interrupt_save(interrupt_t);
	
inline void	disable_interrupt(interrupt_t);
inline int	disable_interrupt_save(interrupt_t);

inline int 	get_proc_status(void);
inline void	restore_proc_status(int);

/* VIC Interrupt Mappings */
	// Primary Interrupt Controller (PIC)
#define WATCHDOG_IRQ	(1 << 0)	/* watchdog controller */
#define SWI_IRQ		(1 << 1)	/* software interrupt */
#define COMMS_RX_IRQ	(1 << 2)	/* debug comms receive intercept */
#define COMMS_TX_IRQ	(1 << 3)	/* debug comms transmit intercept */
#define TIMER_A_IRQ	(1 << 4)	/* timer 0 or 1 */
#define TIMER_B_IRQ	(1 << 5) 	/* timer 2 or 3 */
#define	GPIO_A_IRQ	(1 << 6)	/* GPIO 0 */
#define GPIO_B_IRQ	(1 << 7)	/* GPIO 1 */
#define GPIO_C_IRQ	(1 << 8)	/* GPIO 2 */
#define GPIO_D_IRQ	(1 << 9)	/* GPIO 3 */
#define RTC_IRQ		(1 << 10)	/* Real Time Clock (RTC) */
#define SSP_IRQ		(1 << 11)	/* synchronous serial port */
#define UART_A_IRQ	(1 << 12)	/* UART 0 */
#define UART_B_IRQ	(1 << 13)	/* UART 1 */
#define UART_C_IRQ	(1 << 14)	/* UART 2 */
#define SCIO_IRQ	(1 << 15)	/* smart card interface */
#define CLCD_IRQ	(1 << 16)	/* CLCD controller */
#define DMA_IRQ		(1 << 17)	/* DMA controller */
#define PWRFAIL_IRQ	(1 << 18)	/* power failure from FPGA */
#define MBX_IRQ		(1 << 19)	/* graphics processor */
// IRQ 20 is reserved by the architecture
#define VICINTSOURCE_21	(1 << 21)	/* external interrupt signal from DiskOnChip flash device */
#define VICINTSOURCE_22	(1 << 22)	/* external interrupt signal from MCIO A */
// IRQ 23 is reserved by the architecture
#define VICINTSOURCE_24	(1 << 23)	/* external interrupt signal from AACI */
#define VICINTSOURCE_25	(1 << 24)	/* Ethernet */
#define VICINTSOURCE_26	(1 << 25)	/* USB */
#define VICINTSOURCE_27	(1 << 26)	/* external interrupt signal from expansion connector */
#define VICINTSOURCE_28	(1 << 27)	/* external interrupt signal from expansion connector */
// IRQ 29 is reserved by the architecture
// IRQ 30 is reserved by the architecture
#define VICINTSOURCE_31	(1 << 31)	/* secondary interrupt controller (SIC) */

	// Secondary Interrupt Controller
#endif //__INTERRUPT_H__
