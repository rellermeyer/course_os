/*
 *		*******************************************
 *  			 Virtual Memory Layout
 * 		*******************************************
 *
 *
 *			+-----------------------+ 	0xffff ffff
 *			|						|
 *          :                       :
 *			|		kernel 			|	(255 MB)
 *			+-----------------------+	0xf000 0000 			
 *			|7 kernel stacks w/		|
 *			|7 invalid mem in btw	|	(14*4K=56K)
 *			+-----------------------+	
 *			|						|
 *			|	reserved for		|
 *			:	kernel data			:	(1038 MB)
 *			:	structures			:
 *			|						|
 *			+-----------------------+	0xaf20 0000
 *			|	 PCI Interface		|	(752MB)
 *			+-----------------------+	0x8020 0000
 *			|Peripheral Registers	|	
 *			|	(UART etc)			|	(2MB)
 *			+-----------------------+	0x8000 0000
 *			|						|
 *			|						|
 *			:	User Space			:	(2GB)
 *			:						:
 *			|						|
 *			|_______________________|	0x0000 0000
 */

#ifndef VMLAYOUT_H_
#define VMLAYOUT_H_

//High vectors are enabled by setting V bit in the control register
#define HIVECTABLE		0xffff0000

#define KERNSTART		0xf0000000

#define KSTACKTOP		KERNSTART
#define	KSTACKSIZE		0x1000	//size of a stack
#define KSTACKGAP		0x1000	//size of stack guard
#define V_KDSBASE			0xeff00000
#define	V_L2PTREGBASE	0xefb00000

#define PCISTART		0x80200000
#define PCIEND			0xaf200000

#define PREGSTART		0x80000000

#define USER_LIM		PREGSTART

//Use this macro to get virtual address of peripherals
#define peri_p2v(pa)		(pa+PREGSTART)


#endif
