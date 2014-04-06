/* Virtual memory layout
 *
 * 0x00000000 - 0x7fffffff (0-2GB) = user process memory
 * 0x80000000 - 0xa0ffffff (2GB) = physical memory
 * 	includes peripherals at 0x20000000 - 0x20ffffff
 * 0xc0000000 - 0xefffffff = kernel heap/stack
 * 0xf0000000 - 0xffffffff = kernel code
 *
 * Memory from 0x80000000 upwards won't be accessible to user processes
 */

//Variables defined in kernel.ld generated during linking
extern unsigned int _kstart, _kend;
extern unsigned int _datastart, _dataend;
extern unsigned int _bssstart, _bssend;

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

	//Registers r0:r2 are used by start/main
	asm volatile("push {r0, r1, r2}")

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
			levelone_pt[i] = (x-2048)<<20 | 0x0400 | 2;
		}

		else {
			//pages 0-2047 and 2176-4095
			//not mapped == translation fault
			levelone_pt[i] = 0;
		}
	}

	//Map the first MB into VM at hr same location.
	//This is where the code is currently running
	levelone_pt[0] = 0<<20 | 0x0400 | 2;


}