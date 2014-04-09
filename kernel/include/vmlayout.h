/*
 *			*****************************
 *  			Virtual Memory Layout
 * 			*****************************
 *
 *
 *			+-----------------------+ 	0xffff ffff
 *			|vectors & other kernel |
 *			|	branch locations	|	(?? 1MB)
 *			|	_________________ 	|	0xffef ffff
 *			|						|
 *			|		KERNEL 			|	(255 MB)
 *			+-----------------------+	0xf000 0000 			
 *			|	6 kernel stacks	w/	|
 *			|	6 invalid mem in btw|	(12*4K=48K)
 *			+-----------------------+	0xefff ffff
 *			|						|
 *			|	reserved for		|
 *			:	kernel data			:	(1038 MB)
 *			:	structures			:
 *			|						|
 *			+-----------------------+	0xaf20 0000
 *			|	 PCI Interface		|	(752MB)
 *			+-----------------------+	0x8020 0000
 *			|	Peripheral Registers|	
 *			|	(UART etc)			|	(2MB)
 *			+-----------------------+	0x8000 0000
 *			|						|
 *			|						|
 *			:		User Space		:	(2GB)
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

#define PCISTART		0x80200000

#define PREGSTART		0x80000000

#define USER_LIM		CREGSTART

//Use this macro to get virtual address of peripherals
#define peri_p2v(pa)	(pa+PREGSTART)


#endif