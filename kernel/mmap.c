#include "mmap.h"
#include "memory.h"
#include "klibc.h"
#include "drivers/uart.h"
#include "vm.h"

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

unsigned int * first_level_pt = (unsigned int*) P_L1PTBASE;
extern struct vm_free_list *vm_vas_free_list;
extern struct vm_free_list *vm_l1pt_free_list;
extern struct vm_free_list *vm_l2pt_free_list;

void mmap(void *p_bootargs)
{
	//char *cmdline_args = read_cmdline_tag(p_bootargs);
	//print_uart0(cmdline_args);
	//print_uart0("\n");
	asm volatile("cpsid if");

	//stash register state on the stack
	asm volatile("push {r0-r11}");

	os_printf("%X\n", p_bootargs);

	/*
	 int pte;
	 unsigned int mb_addr = 0;
	 for(pte = 0; pte < 4096; pte++){

	 //one-to-one mapping
	 first_level_pt[pte] = mb_addr | 0x0400 | 2;
	 mb_addr += (1024*1024);

	 }
	 */
	//TODO:. Collin LOOOK HERE
	first_level_pt = (unsigned int *) (P_L1PTBASE + PAGE_TABLE_SIZE);
	//first_level_pt = 0x00200000 + 0x4000 = 0x00204000
	os_printf("first_level_pt=%X\n", first_level_pt);

	int i;
	for (i = 0; i < PAGE_TABLE_SIZE >> 2; i++)
	{
		first_level_pt[i] = 0;
	}

	//temporarily map where it is until we copy it in VAS
	first_level_pt[P_KDSBASE >> 20] = P_KDSBASE | 0x0400 | 2;
	//first_level_pt[0x07f00000>>20] = first_level_pt[7F] = 0x07f00000 = 0x07f04010
	//														0x00004000
	//														0x00000010			

	//1MB for static kernel data structures (stacks and l1 pt)
	first_level_pt[V_KDSBASE >> 20] = P_KDSBASE | 0x0400 | 2;
	//first_level_pt[0xfff00000>>20] = first_level_pt[0xfff] = 0x0x7f04010

	//map the kernel where its currently loaded in the same location temporarily
	//should be less than a MB
	first_level_pt[P_KERNBASE >> 20] = 0 << 20 | 0x0400 | 2;

	//also map it to high memory at 0xf0000000 (vpn = 3840)
	first_level_pt[V_KERNBASE >> 20] = 0 << 20 | 0x0400 | 2;
	first_level_pt[(V_KERNBASE + 0x100000) >> 20] = 0x100000 | 0x0400 | 2;

	//map ~2MB of peripheral registers one-to-one
	first_level_pt[PERIPHBASE >> 20] = PERIPHBASE | 0x0400 | 2;
	first_level_pt[(PERIPHBASE + 0x100000) >> 20] = (PERIPHBASE + 0x100000)
			| 0x0400 | 2;

	//map 752MB of PCI interface one-to-one
	unsigned int pci_bus_addr = PCIBASE;
	for (i = (PCIBASE >> 20); i < (PCITOP >> 20); i++)
	{
		first_level_pt[i] = pci_bus_addr | 0x0400 | 2;
		pci_bus_addr += 0x100000;
	}

	// Quick coarse page table address
	//unsigned int coarse_page_table_address = P_L1PTBASE + 2*PAGE_TABLE_SIZE;
	//os_printf("coarse pt: 0x%X\n", coarse_page_table_address);

	//remap 62MB of physical memory after the kernel 
	// (KERNTOP to end of physical RAM (PMAPTOP))
	// This is where we allocate frames from. Except for the first one.
	unsigned int phys_addr = P_KERNTOP;
	// +1 to skip L1PTBASE
	for (i = (PMAPBASE >> 20); i < (PMAPTOP >> 20); i++)
	{
		first_level_pt[i] = phys_addr | 0x0400 | 2;
		phys_addr += 0x100000;
	}

	// Fill in the coarse page table
	// (TODO: How do we handle 64kB pages? Do they take up 16 entries?)
	//os_memset((void*)coarse_page_table_address, 0, L2_PAGE_TABLE_SIZE);
	// Set the first page to phys_addr
	//*(unsigned int*)coarse_page_table_address = phys_addr | 0x20 | 2;
	//os_printf("0x%X\n", *(unsigned int*)coarse_page_table_address);

	first_level_pt[V_L1PTBASE >> 20] = P_L1PTBASE | 0x0400 | 2;

	// We have to empty out the first MB of that, so we can use it as an array of VASs
	// The first slot is actually the kernel's VAS
	((struct vas*) P_L1PTBASE)->l1_pagetable = (unsigned int*) (V_L1PTBASE
			+ PAGE_TABLE_SIZE);	//first_level_pt;
	((struct vas*) P_L1PTBASE)->l1_pagetable_phys = first_level_pt;
	((struct vas*) P_L1PTBASE)->next = 0x0;
	vm_vas_free_list = (struct vm_free_list*) ((void*) vm_vas_free_list
			+ sizeof(struct vas));
	vm_l1pt_free_list = (struct vm_free_list*) ((void*) vm_l1pt_free_list
			+ PAGE_TABLE_SIZE);

	unsigned int pt_addr = (unsigned int) first_level_pt;

	os_printf("0x%X\n", first_level_pt[(PMAPBASE + 0x100000) >> 20]);

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
	control |= 0x1007; //0b01000000000111 (No high vectors)
	control |= 1 << 23; // Enable ARMv6
	//control |= 1<<29; // Enable ForceAP
	os_printf("control reg: 0x%x\n", control);
	//Write back value into the register
	asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));

	os_printf("Got here\n");

	// Build the free frame list
	vm_build_free_frame_list((void*) PMAPBASE + 0x100000, (void*) PMAPTOP);	//(void*)PMAPBASE+(unsigned int)((PMAPTOP)-(PMAPBASE)));

	//restore register state
	asm volatile("pop {r0-r11}");

	// Except for r0, which is p_bootargs. stacks.s needs to know it.
	asm volatile("mov r0, %[args]" : : [args] "r" (p_bootargs));

	asm volatile("cpsie if");
	asm volatile (".include \"stacks.s\"");

	//branch to proper kernel at start
	asm volatile("bl start2");

}
