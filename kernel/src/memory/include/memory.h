//Physical location of kernel
#define P_KERNBASE		0x00010000
#define P_KERNTOP		0x00200000
//Virtual location of kernel
#define V_KERNBASE		0xf0000000
#define V_KERNTOP		0xf0200000

//All static kernel data structures
//Total space=1MB, currently used=44kB	
#define P_KDSBASE		0x07f00000
#define	P_L1PTBASE	        P_KERNTOP

#define V_KDSBASE		0xfff00000
#define V_L1PTBASE	        V_KERNTOP

#define PERIPHBASE		0x10000000
#define PERIPHTOP		0x20000000

#define PCIBASE			0x41000000
#define PCITOP			0x70000000

//remapped physical memory for direct access
#define PMAPBASE 		0xf0200000
#define PMAPTOP			0xf7f00000

// Region where we use 4K pages.
#define P_4K_BASE (P_L1PTBASE+0x200000)
#define P_4K_TOP (PMAPTOP - 0xf0000000)

//High vectors are enabled by setting V bit in the control register
#define HIVECTABLE		0xffff0000
