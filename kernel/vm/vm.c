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
	return 0;
}

int vm_free_mapping(struct vas *vas, void *vptr) {
	return 0;
}

void vm_enable_vas(struct vas *vas) {
	return;
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

	return p;
}

int vm_free_vas(struct vas *vas) {
	return 0;
}

// TODO: Move this into a framework...
void vm_test() {
	struct vas *vas1 = vm_new_vas();
	os_printf("Got new vas at 0x%X\n", vas1);
}
