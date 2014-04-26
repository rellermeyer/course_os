#include "include/mmap.h"
#include "include/memory.h"
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


void mmap(void *p_bootargs) {
	char *cmdline_args = read_cmdline_tag(p_bootargs);
	print_uart0(cmdline_args);
	print_uart0("\n");
	asm volatile("cpsid if");

	//stash register state on the stack
	asm volatile("push {r0-r11}");

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
	first_level_pt[P_KDSBASE>>20] = P_KDSBASE | 0x0400 | 2;

	//1MB for static kernel data structures (stacks and l1 pt)
	first_level_pt[V_KDSBASE>>20] = P_KDSBASE | 0x0400 | 2;

	//map the kernel where its currently loaded in the same location temporarily
	//should be less than a MB
	first_level_pt[P_KERNBASE>>20] = 0<<20 | 0x0400 | 2;

	//also map it to high memory at 0xf0000000 (vpn = 3840)
	first_level_pt[V_KERNBASE>>20] = 0<<20 | 0x0400 | 2;
	first_level_pt[V_KERNBASE+0x100000>>20] = 0<<20+0x100000 | 0x0400 | 2;

	//map 31MB of phsyical memory managed by kmalloc
	unsigned int p_kheap_addr = P_KHEAPBASE;
	int i;
	for(i = (V_KHEAPBASE>>20); i < (V_KHEAPTOP>>20); i++){
		first_level_pt[i] = p_kheap_addr | 0x0400 | 2;
		p_kheap_addr += 0x100000;
	}

	//map 32MB of physical memory manged by umalloc
	unsigned int p_uheap_addr = P_UHEAPBASE;
	for(i = (V_UHEAPBASE>>20); i < (V_UHEAPTOP>>20); i++){
		first_level_pt[i] = p_uheap_addr | 0x0400 | 2;
		p_uheap_addr += 0x100000;
	}

	//map ~2MB of peripheral registers one-to-one
	first_level_pt[PERIPHBASE>>20] = PERIPHBASE | 0x0400 | 2;
	first_level_pt[(PERIPHBASE+0x100000)>>20] = PERIPHBASE+0x100000 | 0x0400 | 2;

	//map 752MB of PCI interface one-to-one
	unsigned int pci_bus_addr = PCIBASE;
	for(i = (PCIBASE>>20); i < (PCITOP>>20); i++){
		first_level_pt[i] = pci_bus_addr | 0x0400 | 2; 
		pci_bus_addr += 0x100000;
	}

	//remap 62MB of physical memory after the kernel 
	unsigned int phys_addr = P_KERNTOP;
	for(i = (PMAPBASE>>20); i < (PMAPTOP>>20); i++){
		first_level_pt[i] = phys_addr | 0x0400 | 2;
		phys_addr += 0x100000;
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
	control |= 0x3007; //0b11000000000111
	//Write back value into the register
	asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));

	//restore register state
	asm volatile("pop {r0-r11}");

	asm volatile("cpsie if");
  	asm volatile (".include \"stacks.s\"");

  	//branch to proper kernel at start
  	asm volatile("bl start");

}
