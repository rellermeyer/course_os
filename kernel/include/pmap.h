#define PKERNBASE		0x00010000
#define PKERNTOP		0x00100000

//All level 2 pages are in this region
//4096 1K tables = 4MB
#define L2PTSBASE		0x07b00000	

//All static kernel data structures
//Total space=1MB, currently used=44kB	
#define KERNDSBASE		0x07f00000
	//#define	KERNSTKBASE		0x07ff9000	//7 stacks 4K each = 28K
	#define	L1PTBASE		0x07f00000	//16K (16K boundary required also)
	//reserve more DS below

#define PERIPHBASE		0x10000000
#define PCIBASE			0x41000000
#define PCITOP			0x70000000

#define PMSTART 		PKERNTOP
#define PMEND 			L2PTSBASE
#define FRAMESIZE		4096

#define KMALLOCPM		22*0x100000  //22MB for kernel
#define MALLOCPM 		100*0x100000 //100MB for user programs
		

static unsigned int * first_level_pt = (unsigned int*) L1PTBASE;

unsigned int get_page(unsigned int num_frames);