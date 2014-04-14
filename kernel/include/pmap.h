#define PKERNBASE		0x00010000
#define PKERNTOP		0x00100000

//All level 2 pages are in this region
//4096 1K tables = 4MB
#define L2PTSBASE		0x07b00000	

//All static kernel data structures
//Total space=1MB, currently used=44kB	
#define KERNDSBASE		0x07f00000
	#define	KERNSTKBASE		0x07ff9000	//7 stacks 4K each = 28K
	#define	L1PTBASE		0x07ff4000	//16K (16K boundary required also)
	//reserve more DS below

#define PMSTART 		PKERNTOP
#define PMEND 			L2PTREGBASE
#define FRAMESIZE		4096

static unsigned int* first_level_pt = L1PTBASE;

unsigned int get_frame(unsigned int num_frames);