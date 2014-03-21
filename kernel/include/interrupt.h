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
 *		see (http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0290g/I27695.html)
 */

// IRQ -> proc disables irqs, fiqs still enabled
// FIQ -> proc diasbles both

#define IRQ_DISABLE	0x80
#define FIQ_DISABLE	0x40
#define NO_INT		0xC0


// these prototypes aren't complete
void irq_enable(void);
void irq_disable(void);
void irq_handle(void);

void irq_register_handler(void);
// etc. etc.



static inline unsigned long get_proc_status(void)
{
	unsigned long cpsr;
	asm ("mrs %0, cpsr" : "=r"(cpsr));
	return cpsr;
}


// should this be in mmap?
/* Interrupt Mappings */
	// Primary Interrupt Controller (PIC)
#define WATCHDOG_IRQ	0	/* watchdog controller */
#define SWI_IRQ		1	/* software interrupt */
#define COMMS_RX_IRQ	2	/* debug comms receive intercept */
#define COMMS_TX_IRQ	3	/* debug comms transmit intercept */
#define TIMER_A_IRQ	4	/* timer 0 or 1 */
#define TIMER_B_IRQ	5 	/* timer 2 or 3 */
#define	GPIO_A_IRQ	6	/* GPIO 0 */
#define GPIO_B_IRQ	7	/* GPIO 1 */
#define GPIO_C_IRQ	8	/* GPIO 2 */
#define GPIO_D_IRQ	9	/* GPIO 3 */
#define RTC_IRQ		10	/* Real Time Clock (RTC) */
#define SSP_IRQ		11	/* synchronous serial port */
#define UART_A_IRQ	12	/* UART 0 */
#define UART_B_IRQ	13	/* UART 1 */
#define UART_C_IRQ	14	/* UART 2 */
#define SCIO_IRQ	15	/* smart card interface */
#define CLCD_IRQ	16	/* CLCD controller */
#define DMA_IRQ		17	/* DMA controller */
#define PWRFAIL_IRQ	18	/* power failure from FPGA */
#define MBX_IRQ		19	/* graphics processor */
// IRQ 20 is reserved by the architecture
#define VICINTSOURCE_21	21	/* external interrupt signal from DiskOnChip flash device */
#define VICINTSOURCE_22	22	/* external interrupt signal from MCIO A */
// IRQ 23 is reserved by the architecture
#define VICINTSOURCE_24	24	/* external interrupt signal from AACI */
#define VICINTSOURCE_25	25	/* Ethernet */
#define VICINTSOURCE_26	26	/* USB */
#define VICINTSOURCE_27	27	/* external interrupt signal from expansion connector */
#define VICINTSOURCE_28	28	/* external interrupt signal from expansion connector */
// IRQ 29 is reserved by the architecture
// IRQ 30 is reserved by the architecture
#define VICINTSOURCE_31	31	/* secondary interrupt controller (SIC) */

	// Secondary Interrupt Controller
