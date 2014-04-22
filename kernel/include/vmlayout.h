

#ifndef VMLAYOUT_H_
#define VMLAYOUT_H_

//High vectors are enabled by setting V bit in the control register
#define HIVECTABLE		0x0

#define KERNSTART		0xf0000000

#define KSTACKTOP		KERNSTART
#define	KSTACKSIZE		0x1000	//size of a stack
#define KSTACKGAP		0x1000	//size of stack guard
#define V_KDSBASE		0xeff00000
 	#define V_L1PTBASE		0xefff4000

#define	V_L2PTSBASE		0xefb00000

#define PCISTART		0x80200000
#define PCIEND			0xaf200000

#define PREGSTART		0x80000000

#define USER_LIM		PREGSTART

#define UHEAPSTART		0x100000
#define	KHEAPSTART		PCIEND

//Use this macro to get virtual address of peripherals
#define peri_p2v(pa)		(pa+PREGSTART)

static unsigned int * v_first_level_pt = (unsigned int*)V_L1PTBASE;


#endif
