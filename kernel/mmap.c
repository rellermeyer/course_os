#include "include/mmap.h"
#include "include/pmap.h"
#include "include/vmlayout.h"
#include "include/klibc.h"
/*
 * APX AP            Privileged    Unprivileged
 *  1  11 (0x8c00) = read-only     read-only
 *  1  01 (0x8400) = read-only     no access
 *  0  10 (0x0800) = read-write    read-only
 *  0  01 (0x0400) = read-write    no-access

 * Bits 0 and 1 identify the table entry type
 * 0 = translation fault 
 * 1 = course page table
 * 2 = section or supersection
 */	

//static unsigned int* first_level_pt = L1PTBASE;

void mmap(){//unsigned int * first_level_pt){

	asm volatile("push {r0-r11}");
	//disable all interrupts
	asm volatile("cpsid if");
	//os_printf("first_level_pt=%x\n", *first_level_pt);
	//asm volatile("wfi");

	//initialize first_level_pt entires with second level coarse page tables
	//reserved at 0x07b000000 - 0x07f00000
/*
	int pte;
	
	unsigned int mb_addr = 0;
	unsigned int l2pt_addr = L2PTSBASE;

	for(pte = 0; pte < 4096; pte++){

		//each level 2 pt has 256 entires (256*4B=1024B)
		//first_level_pt[pte] = l2pt_addr | 0x400 | 2;
		//l2pt_addr += 1024;

		//one-to-one mapping
		first_level_pt[pte] = mb_addr | 0x0400 | 2;
		mb_addr += (1024*1024);

	}
*/
	
	//temporarily map where it is until we copy it in VAS
	first_level_pt[KERNDSBASE>>20] = KERNDSBASE | 0x0400 | 2;

	//1MB for static kernel data structures (stacks and l1 pt)
	first_level_pt[V_KDSBASE>>20] = KERNDSBASE | 0x0400 | 2;

	//4 1M sections for second level coarse page table region
	first_level_pt[V_L2PTSBASE>>20] = L2PTSBASE | 0x0400 | 2;
	first_level_pt[(V_L2PTSBASE+0x100000)>>20] = (L2PTSBASE+0x100000) | 0x0400 | 2;
	first_level_pt[(V_L2PTSBASE+0x200000)>>20] = (L2PTSBASE+0x200000) | 0x0400 | 2;
	first_level_pt[(V_L2PTSBASE+0x300000)>>20] = (L2PTSBASE+0x300000) | 0x0400 | 2;

	//map the kernel where its currently loaded in the same location temporarily
	//should be less than a MB
	first_level_pt[PKERNBASE>>20] = 0<<20 | 0x0400 | 2;

	//also map it to high memory at 0xf0000000 (vpn = 3840)
	first_level_pt[KERNSTART>>20] = 0<<20 | 0x0400 | 2;
	//first_level_pt[0xf0000000>>20] = 0x210000 | 0x0400 | 2;

	//map ~2MB of peripheral registers from 0x10000000-0x101f5000
	//to two 1MB sections at 0x80000000 and 0x80100000
	first_level_pt[PREGSTART>>20] = PERIPHBASE | 0x0400 | 2;
	first_level_pt[(PREGSTART+0x100000)>>20] = PERIPHBASE+0x100000 | 0x0400 | 2;

	//map 752MB of PCI interface from 0x41000000-0x6fffffff to
	//752 MB sections at 0x80200000-0xaf200000
	unsigned int pci_bus_addr = PCIBASE;
	int i;
	for(i = (PCISTART>>20); i < (PCIEND>>20); i++){
		first_level_pt[i] = pci_bus_addr | 0x0400 | 2;
		//os_printf("i= %d\n",i);
   	   	//os_printf("l1pt= %x\n", first_level_pt[i]);  
		pci_bus_addr += (1024*1024);
	}


	//map remaining physical memory as 1MB section for bump pointer malloc and kmalloc
	unsigned int pm_start = PKERNTOP;
	unsigned int pm_end = L2PTSBASE;
	unsigned int avail_pm = pm_start - pm_end;

	unsigned int u_vm_start = UHEAPSTART;
	unsigned int u_vm_end = u_vm_start + MALLOCPM;

	unsigned int k_vm_start = PCIEND;
	unsigned int k_vm_end = k_vm_start + KMALLOCPM;

	//map 22MB for kmalloc
	for(i = (k_vm_start>>20); i < (k_vm_end>>20) && pm_start < pm_end; i++){
		first_level_pt[i] = pm_start | 0x0400 | 2;
		pm_start += 0x100000;
	}

	//map remaining 100MB for malloc
	for(i = (u_vm_start>>20); i < (u_vm_end>>20) && pm_start < pm_end; i++){
		first_level_pt[i] = pm_start | 0x0400 | 2;
		pm_start += 0x100000;
	}


	unsigned int pt_addr = (unsigned int)first_level_pt;

	//TTBR0
	asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pt_addr));
	// Translation table 1 
	//asm volatile("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (pt_addr));
	//asm volatile("mcr p15, 0, %[n], c2, c0, 2" : : [n] "r" (0));

	//Set Domain Access Control to enforce out permissions
	//b01 = Client. Accesses are checked against the access permission bits in the TLB entry.
	asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0x1));
	

	/*CONTROL REGISTER
	 *	Enable MMU by setting 0
	 *	Alignment bit 1
	 *	D-cache bit 2
	 *	I-cache bit 12
	 *	V bit 13 (1=high vectors 0xffff0000)
	 */
	unsigned int control;

	//Read contents into control
	asm volatile("mrc p15, 0, %[control], c1, c0, 0" : [control] "=r" (control));
	//Set bit 0,1,2,12,13
	control |= 0x1007; //0b11000000000111
	//Write back value into the register
	asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));

	//enable interrupts
	asm volatile("cpsie fi");
	asm volatile("pop {r0-r11}");
}
