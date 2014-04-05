/*
 *
 *  Interrupt handler four course_os
 *
 *  A bit of background:
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

#define IRQ_MASK	0x80	/* IRQs are masked when bit 7 of CPSR is pulled high */
#define FIQ_MASK	0x40	/* FIQs are masked when bit 6 of CPSR is pulled high */
#define ALL_INTER_MASK	0xC0

typedef enum inter_os
{
        IRQ_MASK,
        FIQ_MASK,
        ALL_INTER_MASK
};

enum inter_os IRQ = IRQ_MASK;
enum inter_os FIQ = FIQ_MASK;
enum inter_os ALL = ALL_INTER_MASK;

// these prototypes aren't complete
int enableInterrupt(enum inter_os);

void irq_disable(void);
void irq_handle(void);
void irq_register_handler(void);
// etc. etc.



inline uint32_t get_proc_status(void); 


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
