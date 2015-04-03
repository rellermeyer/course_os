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
	struct vas *p = (struct vas*)PMAPBASE;
	// TODO: What if we overrun our alloted MB?
	while (p->next != 0x0) {
		p = p->next;
	}

	// TODO: Have a better allocation scheme.
	struct vas *next = (void*)p + PAGE_TABLE_SIZE*2;
	p->next = next;

	// Now, advance and fill in the new page table
	//struct vas *p = (struct vas*)(PMAPBASE);// + 2*PAGE_TABLE_SIZE);
	p = p->next;
	p->next = 0x0;
	p->l1_pagetable = (unsigned int*)((void*)p + PAGE_TABLE_SIZE); // The area immediately after the struct vas, aligned to 16KB

	// Chintzy conversion thing
	p->l1_pagetable_phys = (unsigned int*)((unsigned int)p->l1_pagetable - (PMAPBASE - P_KERNTOP));

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

	// Test making a thing in this thing
	struct vas *vas3 = vm_new_vas();
	vm_enable_vas(vas3);
	os_printf("asfdsafkmfdaskdakfdank\n");
	os_printf("%X and %X and %X\n", vas1, vas2, vas3);

	// Clean up & switch back to the kernel's VAS before we return.
	//vm_enable_vas((struct vas*)P_KERNTOP);
	// PMAPBASE is the address of the kernel's VAS (in virtual memory)
	vm_enable_vas((struct vas*)PMAPBASE);
}
