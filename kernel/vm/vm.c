#include "vm.h"
#include "memory.h"
#include "klibc.h"

int vm_allocate_page(struct vas *vas, void *vptr, int permission) {
	return 0;
}

int vm_free_page(struct vas *vas, void *vptr) {
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
	vas->l1_pagetable[(unsigned int)pptr>>20] = (unsigned int)vptr | 0x0400 | 2;
	return 0;
}

int vm_free_mapping(struct vas *vas, void *vptr) {
	return 0;
}

void vm_enable_vas(struct vas *vas) {
	os_printf("Enabling mapping at VAS 0x%X\n", vas);

	//TTBR0
	asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (vas->l1_pagetable));
	// Translation table 1 

	//Set Domain Access Control to enforce out permissions
	//b01 = Client. Accesses are checked against the access permission bits in the TLB entry.
	//asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0x1));
}

struct vas *vm_new_vas() {
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

	// Setup the static mappings...
	// The kernel (high & low addresses)
	//should be less than a MB
	p->l1_pagetable[P_KERNBASE>>20] = 0<<20 | 0x0400 | 2;

	//also map it to high memory at 0xf0000000
	p->l1_pagetable[V_KERNBASE>>20] = 0<<20 | 0x0400 | 2;
	p->l1_pagetable[(V_KERNBASE+0x100000)>>20] = 0x100000 | 0x0400 | 2;

	// Our 1MB page to store VAS datastructures
	p->l1_pagetable[PMAPBASE>>20] = P_KERNTOP | 0x0400 | 2;

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
	vm_set_mapping(vas1, (void*)mystack, (void*)mystack, 0);

	vm_enable_vas(vas1);
	//vm_enable_vas((struct vas*)P_KERNTOP);

	// Can we still print?
	os_printf("Hey, I'm printing!\n");

	// Do we still have the stack?
	os_printf("This value better be the same as above: 0x%X\n", vas1);
}
