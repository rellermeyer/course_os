#include <memory.h>
#include <string.h>
#include <klibc.h>
#include <vm.h>
#include <interrupt.h>
#include <constants.h>

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

volatile unsigned int * first_level_pt = (unsigned int *) (P_L1PTBASE + PAGE_TABLE_SIZE);
extern struct vm_free_list *vm_vas_free_list;
extern struct vm_free_list *vm_l1pt_free_list;
extern struct vm_free_list *vm_l2pt_free_list;

bool mmapped = false;

// Must be called before mmap and after vm.c
void request_identity_mapped_section(size_t start_address, size_t megabytes) {
    
    if (mmapped) {
        // TODO: Technically possible if we were to flush the cache
        kprintf("Can only request identity mapping before mmap is called.");
        panic();
    }
    kprintf("Identity mapping %i megabyte(s) at 0x%x\n", megabytes, start_address);

    for (unsigned int i = 0; i < megabytes; i++) {
        first_level_pt[(start_address + (i * 0x100000)) >> 20] = ((start_address + (i *0x100000)) & 0xFFF00000)  | 0x0400 | 2;
    }
}

void prepare_pagetable() {
    memset((uint32_t *)first_level_pt,0, PAGE_TABLE_SIZE);
}

// 0x00000000
//   ---  ---
//   L1   off
void mmap(void *p_bootargs) {

    disable_interrupt(BOTH);
    //stash register state on the stack
    asm volatile("push {r0-r11}");

    kprintf("Boot arguments location: %X\n", p_bootargs);

    kprintf("first_level_pt=%X\n", first_level_pt);

//    for (int i = 0; i < PAGE_TABLE_SIZE >> 2; i++) {
//        first_level_pt[i] = 0;
//    }

    //temporarily map where it is until we copy it in VAS
//	first_level_pt[P_KDSBASE >> 20] = P_KDSBASE | 0x0400 | 2;
	//first_level_pt[0x07f00000>>20] = first_level_pt[7F] = 0x07ffirst_level_pt=400000000 = 0x07f04010
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
	first_level_pt[(V_KERNBASE + 0x100000) >> 20] = 0x100000 | 0x0400 | 2; // 0xf...;

	//map ~2MB of peripheral registers one-to-one
//	first_level_pt[PERIPHBASE >> 20] = PERIPHBASE | 0x0400 | 2; // PERIPHBASE + 1 MiB
//	first_level_pt[(PERIPHBASE + 0x100000) >> 20] = (PERIPHBASE + 0x100000) | 0x0400 | 2;
//	first_level_pt[(PERIPHBASE + 0x200000) >> 20] = (PERIPHBASE + 0x200000) | 0x0400 | 2;
	first_level_pt[(0x20000000) >> 20] = (0x20000000) | 0x0400 | 2;

    first_level_pt[BCM2836BASE >> 20] = (BCM2836BASE & 0xFFF00000) | 0x0400 | 2; // TIMERBASE + 1 MiB
    first_level_pt[(PERIPHBASE) >> 20] = (PERIPHBASE + 0x300000) | 0x0400 | 2;


    // TODO: re enable when we actually need it. 700mb seems a bit excessive though.
    //map 752MB of PCI interface one-to-one
//    unsigned int pci_bus_addr = PCIBASE;
//    for (int i = (PCIBASE >> 20); i < (PCITOP >> 20); i++) {
//        first_level_pt[i] = pci_bus_addr | 0x0400 | 2;
//        pci_bus_addr += 0x100000;
//    }
    request_identity_mapped_section(Gibibyte - 25 * Mibibyte, 20);

	// Quick coarse page table address
	//unsigned int coarse_page_table_address = P_L1PTBASE + 2*PAGE_TABLE_SIZE;
	//os_printf("coarse pt: 0x%X\n", coarse_page_table_address);

	//remap 62MB of physical memory after the kernel
	// (KERNTOP to end of physical RAM (PMAPTOP))
	// This is where we allocate frames from. Except for the first one.
	unsigned int phys_addr = P_KERNTOP;
	// +1 to skip L1PTBASE
	for (int i = (PMAPBASE >> 20); i < (PMAPTOP >> 20); i++) {
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
	((struct vas*) P_L1PTBASE)->l1_pagetable = (unsigned int*) (V_L1PTBASE + PAGE_TABLE_SIZE);	//first_level_pt;
	((struct vas*) P_L1PTBASE)->l1_pagetable_phys = first_level_pt;
	((struct vas*) P_L1PTBASE)->next = 0x0;
	vm_vas_free_list = (struct vm_free_list*) ((void*) vm_vas_free_list + sizeof(struct vas));
	vm_l1pt_free_list = (struct vm_free_list*) ((void*) vm_l1pt_free_list + PAGE_TABLE_SIZE);

 	unsigned int pt_addr = (unsigned int) first_level_pt;

	/*CONTROL REGISTER
	 *	Enable MMU by setting 0
	 *	Alignment bit 1
	 *	D-cache bit 2
	 *	I-cache bit 12
	 *	V bit 13 (1=high vectors 0xffff0000)
	 * We disable high vectors, since low vectors work just fine.
	 */

    asm volatile (
    // invalidate caches
    "mcr p15, 0, %[r], c7, c7, 0\n"
    // invalidate tlb
    "mcr p15, 0, %[r], c8, c7, 0\n"
    // data sync barrier
    "mcr p15, 0, %[r], c7,c10, 4\n"

    // set domains
    "mov r2, #0x01\n"
//    "ldr r2, =0x55555555\n"         	// [added] full r/w to everyone
    "mcr p15, 0, r2, c3, c0, 0\n"

    "mcr p15, 0, %[addr], c2, c0, 0\n"	// Give the pagetable addr to the MMU
    "mcr p15, 0, %[addr], c2, c0, 1\n"

    "mrc p15, 0, r3, c1, c0, 0\n" // Read p15
//    "mov r4, #0x1000 \n"
//    "add r4, #0x7 \n"
//    "orr r3, r4 \n"
    "orr r3, #0x800000\n"          // Enable Armv6 bit
//    "orr r3, #0x1000\n"
    "orr r3, r3, #0x1\n"           // Enable MMU bit
    "mcr p15, 0, r3, c1, c0, 0 \n" // Set p15
    :
    : [addr] "r" (pt_addr),
      [r] "r" (0x0)
    );

    mmapped = true;

//    SemihostingCall(OSSpecific);

	// Build the free frame list
	vm_build_free_frame_list((void*) PMAPBASE + 0x100000, (void*) PMAPTOP);	//(void*)PMAPBASE+(unsigned int)((PMAPTOP)-(PMAPBASE)));

	//restore register state
	asm volatile("pop {r0-r11}");

	// Except for r0, which is p_bootargs. stacks.s needs to know it.
	asm volatile("mov r0, %[args]" : : [args] "r" (p_bootargs));

	// asm volatile("cpsie if");
	asm volatile (".include \"src/memory/stacks.s\"");

    void start2(uint32_t *p_bootargs);
	kprintf("Location of start2: 0x%x\n", &start2);
    //branch to proper kernel at start
//	asm volatile("bl start2");

    void * oldstart2 = start2;
    void (*newstart2)(uint32_t *) = oldstart2 + V_KERNBASE;

    newstart2(p_bootargs);
}
