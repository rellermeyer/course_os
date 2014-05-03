


//Physical location of kernel
#define P_KERNBASE		0x00010000
#define P_KERNTOP		0x00200000
//Virtual location of kernel
#define V_KERNBASE		0xf0000000
#define V_KERNTOP		0xf0200000

//Physical region managed by kmalloc
#define P_KHEAPBASE		0x04000000
#define P_KHEAPTOP		0x05f00000
//Vitual region managed by kmalloc
#define V_KHEAPBASE		0xee100000
#define V_KHEAPTOP		V_KERNBASE

//Physical region managed by umalloc
#define P_UHEAPBASE		P_KHEAPTOP
#define P_UHEAPTOP		0x07f00000
//Virtual region managed by umalloc
#define V_UHEAPBASE		0x00200000
#define V_UHEAPTOP 		0x02200000

//All static kernel data structures
//Total space=1MB, currently used=44kB	
#define P_KDSBASE		0x07f00000
	//#define	KERNSTKBASE		0x07ff9000	//7 stacks 4K each = 28K
	#define	P_L1PTBASE		0x07f00000	//16K (16K boundary required also)

#define V_KDSBASE		0xfff00000
 	#define V_L1PTBASE		0xfff00000

#define PERIPHBASE		0x10000000
#define PERIPHTOP		0x20000000

#define PCIBASE			0x41000000
#define PCITOP			0x70000000

//remapped physical memory for direct access
#define PMAPBASE 		0xf0200000
#define PMAPTOP			0xf4000000

//High vectors are enabled by setting V bit in the control register
#define HIVECTABLE		0xffff0000

static unsigned int * first_level_pt = (unsigned int*) P_L1PTBASE;
static unsigned int * v_first_level_pt = (unsigned int*)V_L1PTBASE;

//Physical address (between P_KERNTOP and P_KHEAPBASE)
#define pa2va(pa)		(pa+V_KERNBASE)

#define va2pa(va)		(P_KHEAPBASE + (va - V_KHEAPBASE))