/* Virtual memory layout
 *
 * 0x00000000 - 0x7fffffff (0-2GB) = user process memory
 * 0x80000000 - 0x88000000 = physical memory (128M)
 * 0xc0000000 - 0xefffffff = kernel heap/stack
 * 0xf0000000 - 0xffffffff = kernel code
 *
 * Memory from 0x80000000 upwards won't be accessible to user processes
 */

//Variables defined in kernel.ld generated during linking
extern unsigned int _kstart, _kend;
extern unsigned int _datastart, _dataend;
extern unsigned int _bssstart, _bssend;

//Call start when done
extern void start(void*);

// 4096 entries = 16KB below kernel
static unsigned int *levelone_pt = (unsigned int * const)0xc000; 
//256 entries = 1KB below level one pt 
static unsigned int *kernel_leveltwo_pt = (unsigned int * const)0xb000;


/*
 * AP            Privileged    Unprivileged
 * 11 (0x8c00) = read-only     read-only
 * 01 (0x8400) = read-only     no access
 * 10 (0x0800) = read-write    read-only
 * 01 (0x0400) = read-write    no-access

 * Bits 0 and 1 identify the table entry type
 * 0 = translation fault 
 * 1 = course page table
 * 2 = section or supersection
 */	

void initvm(void){
   	print_uart0("Initializing virtual memory\n");

	//Registers r0:r2 are used by start/main
	asm volatile("push {r0, r1, r2}");

	unsigned int i;
	/*
	 * Remap physical memory into the virtual address space for device access, mmio etc
	 * VA: 0x80000000 - 0x88000000 
	 * Entries 2048 - 2175 in the level 1 table 
	 * 		-- marked as 1MB sections
	 *		   (128 entries cover 128MB of PM)
	 * 		-- permission is R/W
     *
	*/

	for(i=0; i<4096; i++){

		if(i >= 2048 && i < 2176){
			// section base addr | AP | entry type
			levelone_pt[i] = (i-2048)<<20 | 0x0400 | 2;
		}

		else {
			//pages 0-2047 and 2176-4095
			//not mapped == translation fault
			levelone_pt[i] = 0;
		}
	}

	//Map the first MB into VM at the same location.
	//This is where the code is currently running
	levelone_pt[0] = 0<<20 | 0x0400 | 2;


	/*
	 * Reserved 1MB for kernel image in physical memory
	 * which inlcudes the initvm, start, code, data and bss
	 * Map this MB using a section at 0xf0000000 - 0xf00fffff
	 * which corresponds to virtual page number 3840
	 * 	-- map as read-only
	 *  ?? leave 0xf00fffff - 0xffffffff if kernel grows
	 *	    -- if so, will need more coarse tables 
     *
	*/
    levelone_pt[3840] = 0<<20 | 0x8400 | 2;

    /* 0xc0000000-0xc00fffff is mapped to physical memory by a coarse
	 * page table
	 *
	 * This maps the kernel data from where it has been loaded in memory
	 * (ie somewhere after initvm and start) to 0xc0000000
	 * At the moment this is the only kernel structure in the kernel heap region
	 */
	levelone_pt[3072] = 1 | (unsigned int)kernel_leveltwo_pt;


	//TODO: initailze the 256 entries in the coarse table


	//Initialize .bss section
	unsigned int* bss;
	bss = &_bssstart;
	while(bss < &_bssend){
		*bss = 0;
		bss++;
	}

    print_uart0("Enabling MMU\n");


	//Set TTBR0 and TTBR1 in the MMU with the address
	//of the level one page table
	unsigned int pt_addr = (unsigned int)levelone_pt;

	// Translation table 0 - ARM1176JZF-S manual, 3-57
	asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pt_addr));
	// Translation table 1 
	asm volatile("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (pt_addr));

	//TODO: other MMU registers

	//Set lr to the address of start
	asm volatile("mov lr, %[start]" : : [start] "r" ((unsigned int)&start) );
	//Restore registers used in start
	asm volatile("pop {r0, r1, r2}");
	//Jump to value in lr
	asm volatile("bx lr");

}