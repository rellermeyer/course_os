#include "vm.h"
#include "memory.h"
#include "klibc.h"
#include "./frame.h"

int vm_allocate_page(struct vas *vas, void *vptr, int permission) {
	// TODO: Check if the vas already has a mapping there.
	void *pptr = vm_get_free_frame();
	if (pptr == 0x0) {
		// We need to swap! (or something...)
		return 1; // For now, just fail
	}
	return vm_set_mapping(vas, vptr, pptr, permission);
}

int vm_free_page(struct vas *vas, void *vptr) {
	// TODO: Check if it was actually allocated
	vm_release_frame((void*)(vas->l1_pagetable[(unsigned int)vptr>>20]&~((PAGE_TABLE_SIZE<<1) - 1)));
	vas->l1_pagetable[(unsigned int)vptr>>20] = 0;
	return 0;
}

int vm_pin(struct vas *vas, void *vptr) {
	return 0;
}

int vm_unpin(struct vas *vas, void *vptr) {
	return 0;
}

int vm_set_mapping(struct vas *vas, void *vptr, void *pptr, int permission) {
	// TODO: Make permissions matter
	vas->l1_pagetable[(unsigned int)vptr>>20] = (unsigned int)pptr | 0x0400 | 2;
	return 0;
}

int vm_free_mapping(struct vas *vas, void *vptr) {
	// TODO: If this is a paged frame, then we need to release it...
	vas->l1_pagetable[(unsigned int)vptr>>20] = 0;
	return 0;
}

void vm_enable_vas(struct vas *vas) {
	os_printf("Enabling mapping at VAS 0x%X\n", vas);
	//os_printf("(entry at 0xF0300000: 0x%X)\n", vas->l1_pagetable[0xF0300000>>20]);
	//os_printf("(entry at 0x200000: 0x%X)\n", vas->l1_pagetable[0x200000>>20]);

	// Clear the BTAC
	asm volatile("mcr p15, 0, %[r], c7, c5, 6" : : [r] "r" (0x0));

	// Flush the write caches
	asm volatile("MCR p15, 0, %[r], c7, c10, 4" : : [r] "r" (0x0));

	//TTBR0
	asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (vas->l1_pagetable_phys));
	// Translation table 1 

	os_printf("Got here.\n");

	// Clean the caches (data & instruction)
	asm volatile("mcr p15, 0, %[r], c7, c7, 0" : : [r] "r" (0x0));

	os_printf("Got here?\n");

	// Enable caches

	//Set Domain Access Control to enforce out permissions
	//b01 = Client. Accesses are checked against the access permission bits in the TLB entry.
	//asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0x1));
}

struct vas *vm_new_vas() {
	// Grab a new page table (remember, we have the first MB after KERNTOP)
	/*struct vas *p = (struct vas*)P_KERNTOP;
	// TODO: What if we overrun our alloted MB?
	while (p->next != 0x0) {
		p = p->next;
	}

	// TODO: Have a better allocation scheme.
	struct vas *next = (void*)p + PAGE_TABLE_SIZE*2;
	p->next = next;*/

	// Now, advance and fill in the new page table
	struct vas *p = (struct vas*)(PMAPBASE);// + 2*PAGE_TABLE_SIZE);
	p->next = 0x0;
	p->l1_pagetable = (unsigned int*)((void*)p + 3*PAGE_TABLE_SIZE); // The area immediately after the struct vas, aligned to 16KB
	p->l1_pagetable_phys = (unsigned int*)(P_KERNTOP + 3*PAGE_TABLE_SIZE);

	// Zero out the page table
	int i;
	for (i=0; i<PAGE_TABLE_SIZE>>2; i++) {
		p->l1_pagetable[i] = 0;
	}


	unsigned int *first_level_pt = (unsigned int *)p->l1_pagetable;
	os_printf("New first_level_pt=%X\n",first_level_pt);

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

	//map 31MB of phsyical memory managed by kmalloc
	unsigned int p_kheap_addr = P_KHEAPBASE;
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
	first_level_pt[(PERIPHBASE+0x100000)>>20] = (PERIPHBASE+0x100000) | 0x0400 | 2;

	//map 752MB of PCI interface one-to-one
	unsigned int pci_bus_addr = PCIBASE;
	for(i = (PCIBASE>>20); i < (PCITOP>>20); i++){
		first_level_pt[i] = pci_bus_addr | 0x0400 | 2; 
		pci_bus_addr += 0x100000;
	}

	//remap 62MB of physical memory after the kernel 
	// (KERNTOP to KHEAPBASE)
	// This is where we allocate frames from. Except for the first one.
	unsigned int phys_addr = P_KERNTOP;
	for(i = (PMAPBASE>>20); i < (PMAPTOP>>20); i++){
		first_level_pt[i] = phys_addr | 0x0400 | 2;
		phys_addr += 0x100000;
	}

	return p;
}

struct vas *vm_new_vas2() {
	// Grab a new page table (remember, we have the first MB after KERNTOP)
	struct vas *p = (struct vas*)P_KERNTOP;
	// TODO: What if we overrun our alloted MB?
	while (p->next != 0x0) {
		p = p->next;
	}

	// TODO: Have a better allocation scheme.
	struct vas *next = (void*)p + PAGE_TABLE_SIZE*2;
	p->next = next;

	// Now, advance and fill in the new page table
	p = p->next;
	p->next = 0x0;
	p->l1_pagetable = (unsigned int*)((void*)p + PAGE_TABLE_SIZE); // The area immediately after the struct vas, aligned to 16KB

	// Zero out the page table
	int i;
	for (i=0; i<PAGE_TABLE_SIZE>>2; i++) {
		p->l1_pagetable[i] = 0;
	}

	// Setup the static mappings...
	// The kernel (high & low addresses)
	//should be less than a MB
	p->l1_pagetable[P_KERNBASE>>20] = 0<<20 | 0x0400 | 2;

	//also map it to high memory at 0xf0000000
	p->l1_pagetable[V_KERNBASE>>20] = 0<<20 | 0x0400 | 2;
	p->l1_pagetable[(V_KERNBASE+0x100000)>>20] = 0x100000 | 0x0400 | 2;

	// Our 1MB page to store VAS datastructures
	p->l1_pagetable[PMAPBASE>>20] = P_KERNTOP | 0x0400 | 2;

	// The high vector table
	p->l1_pagetable[HIVECTABLE>>20] = 0<<20 | 0x0400 | 2;

	os_printf("Creating page table w/ 0xF0200000's entry = 0x%X\n", p->l1_pagetable[PMAPBASE>>20]);

	// 2MB of peripheral registers (so we get the serial port et. al.)
	p->l1_pagetable[PERIPHBASE>>20] = PERIPHBASE | 0x0400 | 2;
	p->l1_pagetable[(PERIPHBASE+0x100000)>>20] = (PERIPHBASE+0x100000) | 0x0400 | 2;

	return p;
}

int vm_free_vas(struct vas *vas) {
	return 0;
}

// TODO: Move this into a framework...
void vm_test() {
	struct vas *vas1 = vm_new_vas();
	os_printf("Got new vas at 0x%X\n", vas1);

	// We're part of the kernel, which is already mapped into vas1.
	// But our stack isn't, so let's add that mapping.
	unsigned int mystack = (unsigned int)&vas1;
	mystack &= 0xFFF00000; // Round down to nearest MB
	os_printf("Stack addr: 0x%X\n", mystack);
	vm_set_mapping(vas1, (void*)mystack, (void*)mystack, 0);

	//char *p = (char*)0x1;
	//p[0]++;

	os_printf("Created page table w/ 0xF0200000's entry = 0x%X\n", vas1->l1_pagetable[PMAPBASE>>20]);

	//vas1 = (struct vas*)PMAPBASE;
	vm_enable_vas(vas1);
	//vm_enable_vas((struct vas*)P_KERNTOP);

	// Can we still print?
	os_printf("Hey, I'm printing!\n");

	// Do we still have the stack?
	os_printf("This value better be the same as above: 0x%X\n", vas1);

	struct vas *vas2 = (struct vas*)PMAPBASE;//V_KERNTOP;
	vas2 = (struct vas*)0x0;
	os_printf("%X (%X)\n", vas2, &vas2);
	os_printf("%X\n", *((unsigned int*)vas2));
	os_printf("%X\n", vas2->l1_pagetable);
	os_printf("(deref: entry at 0x200000: 0x%X)\n", vas2->l1_pagetable[0x200000>>20]);

	// Clean up & switch back to the kernel's VAS before we return.
	//vm_enable_vas((struct vas*)P_KERNTOP);
	vm_enable_vas((struct vas*)PMAPBASE);
}
