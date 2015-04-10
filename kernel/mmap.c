#include "include/mmap.h"
#include "include/memory.h"
#include "include/klibc.h"
#include "include/drivers/uart.h"
#include "include/vm.h"

/*
 * APX AP            Privileged    Unprivileged
 *  1  11 (0x8c00) = read-only     read-only
 *  1  01 (0x8400) = read-only     no access
 *  0  10 (0x0800) = read-write    read-only
 *  0  01 (0x0400) = read-write    no-access
 * See http://infocenter.arm.com/help/topic/com.arm.doc.ddi0333h/Caceaije.html

 * Bits 0 and 1 identify the table entry type
 * 0 = translation fault 
 * 1 = course page table
 * 2 = section or supersection
 */	

int vm_build_free_frame_list(void *start, void *end);

static unsigned int * first_level_pt = (unsigned int*) P_L1PTBASE;

void mmap(void *p_bootargs) {
	//char *cmdline_args = read_cmdline_tag(p_bootargs);
	//print_uart0(cmdline_args);
	//print_uart0("\n");
	asm volatile("cpsid if");

	//stash register state on the stack
	asm volatile("push {r0-r11}");

	os_printf("%X\n",p_bootargs);

/*
	int pte;
	unsigned int mb_addr = 0;
	for(pte = 0; pte < 4096; pte++){

		//one-to-one mapping
		first_level_pt[pte] = mb_addr | 0x0400 | 2;
		mb_addr += (1024*1024);

	}
*/

	first_level_pt = (unsigned int *)(P_L1PTBASE + PAGE_TABLE_SIZE);
	os_printf("first_level_pt=%X\n",first_level_pt);

	//temporarily map where it is until we copy it in VAS
	first_level_pt[P_KDSBASE>>20] = P_KDSBASE | 0x0400 | 2;

	//1MB for static kernel data structures (stacks and l1 pt)
	first_level_pt[V_KDSBASE>>20] = P_KDSBASE | 0x0400 | 2;

	//map the kernel where its currently loaded in the same location temporarily
	//should be less than a MB
	first_level_pt[P_KERNBASE>>20] = 0<<20 | 0x0400 | 2;

	//also map it to high memory at 0xf0000000 (vpn = 3840)
	first_level_pt[V_KERNBASE>>20] = 0<<20 | 0x0400 | 2;
	first_level_pt[(V_KERNBASE+0x100000)>>20] = 0x100000 | 0x0400 | 2;

	//map ~2MB of peripheral registers one-to-one
	first_level_pt[PERIPHBASE>>20] = PERIPHBASE | 0x0400 | 2;
	first_level_pt[(PERIPHBASE+0x100000)>>20] = (PERIPHBASE+0x100000) | 0x0400 | 2;

	//map 752MB of PCI interface one-to-one
	unsigned int pci_bus_addr = PCIBASE;
	int i;
	for(i = (PCIBASE>>20); i < (PCITOP>>20); i++){
		first_level_pt[i] = pci_bus_addr | 0x0400 | 2; 
		pci_bus_addr += 0x100000;
	}

	//remap 62MB of physical memory after the kernel 
	// (KERNTOP to end of physical RAM (PMAPTOP))
	// This is where we allocate frames from. Except for the first one.
	unsigned int phys_addr = P_KERNTOP;
	for(i = (PMAPBASE>>20); i < (PMAPTOP>>20); i++){
		first_level_pt[i] = phys_addr | 0x0400 | 2;
		phys_addr += 0x100000;
	}

	first_level_pt[V_L1PTBASE>>20] = P_L1PTBASE | 0x0400 | 2;

	// We have to empty out the first MB of that, so we can use it as an array of VASs
	// The first slot is actually the kernel's VAS
	((struct vas*)P_L1PTBASE)->l1_pagetable = (unsigned int*)(V_L1PTBASE + PAGE_TABLE_SIZE);//first_level_pt;
	((struct vas*)P_L1PTBASE)->l1_pagetable_phys = first_level_pt;
	((struct vas*)P_L1PTBASE)->next = 0x0;

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
	 * We disable high vectors, since low vectors work just fine.
	 */
	unsigned int control;

	//Read contents into control
	asm volatile("mrc p15, 0, %[control], c1, c0, 0" : [control] "=r" (control));
	//Set bit 0,1,2,12,13
	//control |= 0x3007; //0b11000000000111
	control |= 0x1007; //0b11000000000111
	//Write back value into the register
	asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));

	os_printf("Got here\n");

	// Build the free frame list
	vm_build_free_frame_list((void*)PMAPBASE + 0x100000, (void*)PMAPBASE+(unsigned int)((PMAPTOP)-(PMAPBASE)));

	//restore register state
	asm volatile("pop {r0-r11}");

	// Except for r0, which is p_bootargs. stacks.s needs to know it.
	asm volatile("mov r0, %[args]" : : [args] "r" (p_bootargs));

	asm volatile("cpsie if");
  	asm volatile (".include \"stacks.s\"");

  	//branch to proper kernel at start
  	asm volatile("bl start2");

}
