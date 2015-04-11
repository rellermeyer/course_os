#include "vm.h"
#include "memory.h"
#include "klibc.h"
#include "./frame.h"

#define CHECK_VPTR if ((unsigned int)vptr & (BLOCK_SIZE-1)) return VM_ERR_BADV;
#define CHECK_PPTR if ((unsigned int)pptr & (BLOCK_SIZE-1)) return VM_ERR_BADP;

static const int perm_mapping[16] = {
	0,  // 0000 Nothing
	5,  // 0001 Privileged RO, nothing otherwise
	6,  // 0010 User RO, privileged RO.
	6,  // 0011 User RO, privileged RO.
	1,  // 0100 Privileged RW, nothing otherwise
	-1, // 0101 ???
	2,  // 0110 Privileged RW, user RO
	-1, // 0111 ???
	3,  // 1000 User RW, privileged RW
	-1, // 1001 ???
	-1, // 1010 ???
	-1, // 1011 ???
	3,  // 1100 User RW, privileged RW
	-1, // 1101 ???
	-1, // 1110 ???
	-1, // 1111 ???
};

static struct vas *vm_current_vas = (struct vas*)V_L1PTBASE;

struct vm_free_list {
	struct vm_free_list *next;
};

struct vm_free_list *vm_vas_free_list = 0x0;
struct vm_free_list *vm_l1pt_free_list = 0x0;

void vm_init() {
	// Initialize the VAS structures stored in the first PAGE_TABLE_SIZE
	// of our alloted MB.
	struct vm_free_list *free_vas = (struct vm_free_list*)P_L1PTBASE;
	//vm_vas_free_list = free_vas;
	vm_vas_free_list = (struct vm_free_list*)((void*)free_vas + V_L1PTBASE-P_L1PTBASE);
	struct vm_free_list *last = 0x0;
	while ((uint32_t)free_vas<P_L1PTBASE+PAGE_TABLE_SIZE) {
		free_vas->next = 0x0;
		if (last) {
			last->next = (struct vm_free_list*)((void*)free_vas + V_L1PTBASE-P_L1PTBASE);
		}
		last = free_vas;
		free_vas = (struct vm_free_list*)((void*)free_vas+sizeof(struct vas));
	}

	// Initialize the L1 page tables
	struct vm_free_list *free_l1pt = (struct vm_free_list*)(P_L1PTBASE + PAGE_TABLE_SIZE);
	vm_l1pt_free_list = (struct vm_free_list*)((void*)free_l1pt + V_L1PTBASE-P_L1PTBASE);
	last = 0x0;
	while ((uint32_t)free_l1pt < P_L1PTBASE + BLOCK_SIZE) {
		free_l1pt->next = 0x0;
		if (last) {
			last->next = (struct vm_free_list*)((void*)free_l1pt + V_L1PTBASE-P_L1PTBASE);
		}
		last = free_l1pt;
	        free_l1pt = (struct vm_free_list*)((void*)free_l1pt + PAGE_TABLE_SIZE);
	}
}

struct vas *vm_get_current_vas() {
	return vm_current_vas;
}

void vm_use_kernel_vas() {
	vm_enable_vas((struct vas*)V_L1PTBASE);
}

int vm_allocate_page(struct vas *vas, void *vptr, int permission) {
	CHECK_VPTR;

	// We have to save the current VAS
	struct vas *prev_vas = vm_current_vas;
	vm_enable_vas((struct vas*)V_L1PTBASE);

	// TODO: Check if the vas already has a mapping there.
	void *pptr = vm_get_free_frame();
	if (pptr == 0x0) {
		// We need to swap! (or something...)
		return VM_ERR_UNKNOWN; // For now, just fail
	}
	int retval = vm_set_mapping(vas, vptr, pptr, permission);
        if (retval) {
		// Release the frame to prevent a memory leak
		vm_release_frame(pptr);
		vm_enable_vas(prev_vas);
		return retval;
	}
	vm_enable_vas(prev_vas);
	return 0;
}

#define VM_L1_GET_ENTRY(table,vptr) table[((unsigned int)vptr)>>20]
#define VM_L1_SET_ENTRY(table,vptr,ent) (table[((unsigned int)vptr)>>20]=ent)
#define VM_ENTRY_GET_FRAME(x) ((x)&~((PAGE_TABLE_SIZE<<1) - 1))

int vm_free_page(struct vas *vas, void *vptr) {
	CHECK_VPTR;

	// We have to save the current VAS
	struct vas *prev_vas = vm_current_vas;
	vm_enable_vas((struct vas*)V_L1PTBASE);

	// TODO: Check if it was actually allocated
	uint32_t entry = VM_L1_GET_ENTRY(vas->l1_pagetable, vptr);
	vm_release_frame((void*)VM_ENTRY_GET_FRAME(entry));
	vas->l1_pagetable[(unsigned int)vptr>>20] = 0;

	vm_enable_vas(prev_vas);
	return 0;
}

int vm_pin(struct vas *vas, void *vptr) {
	return 0;
}

int vm_unpin(struct vas *vas, void *vptr) {
	return 0;
}

int vm_set_mapping(struct vas *vas, void *vptr, void *pptr, int permission) {
	CHECK_VPTR;
	CHECK_PPTR;
	int perm = perm_mapping[permission];
	if (perm == -1)	return VM_ERR_BADPERM;
	if (vas->l1_pagetable[(unsigned int)vptr>>20]) {
		return VM_ERR_MAPPED;
	}

	vas->l1_pagetable[(unsigned int)vptr>>20] = (unsigned int)pptr | (perm<<10) | 2;
	return 0;
}

int vm_free_mapping(struct vas *vas, void *vptr) {
	CHECK_VPTR;
	// TODO: If this is a paged frame, then we need to throw an error
	vas->l1_pagetable[(unsigned int)vptr>>20] = 0;
	return 0;
}

void vm_enable_vas(struct vas *vas) {
	vm_current_vas = vas;

	// Clear the BTAC
	// Performed by cleaning the caches, below
	//asm volatile("mcr p15, 0, %[r], c7, c5, 6" : : [r] "r" (0x0));

	// Flush the write caches
	asm volatile("MCR p15, 0, %[r], c7, c10, 4" : : [r] "r" (0x0)); // sync barrier
	asm volatile("MCR p15, 0, %[r], c7, c10, 5" : : [r] "r" (0x0)); // memory barrier

	//TTBR0
	asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (vas->l1_pagetable_phys));
	// Translation table 1 is currently ignored

	// Clean the caches (data & instruction)
	asm volatile("mcr p15, 0, %[r], c7, c14, 0" : : [r] "r" (0x0));
}

struct vas *vm_new_vas() {
	if (!vm_vas_free_list) {
		return 0x0;
	}
	if (!vm_l1pt_free_list) {
		return 0x0;
	}
	struct vas *p = (struct vas*)vm_vas_free_list;
	vm_vas_free_list = vm_vas_free_list->next;

	p->l1_pagetable = (uint32_t*)vm_l1pt_free_list;
	vm_l1pt_free_list = vm_l1pt_free_list->next;

	p->l1_pagetable_phys = (unsigned int*)((unsigned int)p->l1_pagetable - (V_L1PTBASE - P_L1PTBASE));

	// Zero out the page table
	//os_memset(p->l1_pagetable, 0, PAGE_TABLE_SIZE);
#if 1
	int i;
	for (i=0; i<PAGE_TABLE_SIZE>>2; i++) {
		p->l1_pagetable[i] = 0;
	}
#endif

	// Setup the static mappings...
	// The kernel (high & low addresses)
	//should be less than a MB
	p->l1_pagetable[P_KERNBASE>>20] = 0<<20 | 0x0400 | 2;

	//also map it to high memory at 0xf0000000
	p->l1_pagetable[V_KERNBASE>>20] = 0<<20 | 0x0400 | 2;
	p->l1_pagetable[(V_KERNBASE+0x100000)>>20] = 0x100000 | 0x0400 | 2;

	// Kernel datastructures
	//temporarily map where it is until we copy it in VAS
	p->l1_pagetable[P_KDSBASE>>20] = P_KDSBASE | 0x0400 | 2;

	//1MB for static kernel data structures (stacks and l1 pt)
	p->l1_pagetable[V_KDSBASE>>20] = P_KDSBASE | 0x0400 | 2;

	// Our 1MB page to store VAS datastructures
	p->l1_pagetable[V_L1PTBASE>>20] = P_L1PTBASE | 0x0400 | 2;

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
	os_printf("***** Test code for VM (vm_test()): *****\n");

	struct vas *vas1 = vm_new_vas();
	os_printf("Got new vas at 0x%X\n", vas1);

	// We're part of the kernel, which is already mapped into vas1.
	// But our stack isn't, so let's add that mapping.
	unsigned int mystack = (unsigned int)&vas1;
	mystack &= 0xFFF00000; // Round down to nearest MB
	os_printf("Stack addr: 0x%X\n", mystack);

	os_printf("Created page table w/ 0xFFF00000's entry = 0x%X\n", vas1->l1_pagetable[V_KDSBASE>>20]);

	vm_enable_vas(vas1);

	// Can we still print?
	os_printf("Hey, I'm printing!\n");

	// Do we still have the stack?
	os_printf("This value better be the same as above: 0x%X\n", vas1);

	struct vas *vas2 = (struct vas*)V_L1PTBASE;
	os_printf("%X (%X)\n", vas2, &vas2);
	os_printf("%X\n", *((unsigned int*)vas2));
	os_printf("%X\n", vas2->l1_pagetable);
	os_printf("Entry: %x\n", vas1->l1_pagetable[(unsigned int)vas2->l1_pagetable >> 20]);
	os_printf("%X\n", vas2->l1_pagetable[0]);
	os_printf("(deref: entry at 0x200000: 0x%X)\n", vas2->l1_pagetable[0x200000>>20]);

	// Test making a thing in this thing
	struct vas *vas3 = vm_new_vas();
	vm_enable_vas(vas3);
	os_printf("%X and %X and %X\n", vas1, vas2, vas3);

	// Test allocating frames...
	int retval = vm_allocate_page(vas3, (void*)0x25000000, VM_PERM_PRIVILEGED_RW);
	if (retval) {
		os_printf("ERROR: vm_allocate_page returned %x\n", retval);
	}

	{
		int *p = (int*)0xFFFFFFF0;
		p[0] = 1;
		os_printf("0x%x == 1?\n", p[0]);
	}

	// Oh man! We should be able to write to there!
	int *p = (int*)0x25000000;
	p[0] = 1;
	os_printf("%x %x\n", &p, p);
	os_printf("%d == 1?\n", p[0]);

	// Test allocating many frames...
	p += BLOCK_SIZE;
	while (!vm_allocate_page(vas3, (void*)p, 0)) {
		p += BLOCK_SIZE;
	}
	p -= BLOCK_SIZE;
	os_printf("Highest frame allocated: 0x%X\n",p);
	while ((unsigned int)p > 0x25000000) {
		vm_free_page(vas3, p);
		p -= BLOCK_SIZE;
	}

	// Test the data abort...
	os_printf("You should see a data abort...\n");
	int i = p[-1];
	os_printf("%d\n",i);

	// Free the page!
	vm_free_page(vas3, p);

	// Clean up & switch back to the kernel's VAS before we return.
	vm_enable_vas((struct vas*)V_L1PTBASE);

	os_printf("*****************************\n");
}
