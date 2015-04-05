#ifndef __VM_H
#define __VM_H 1

// There are some invariant things about VASs, for the moment.
// The bottom MB (virtual) contains both the kernel code (just the code) and the
// VAS data for the kernel. This allows us to do just enough so that
// interrupts (esp. system calls) can load the kernel's VAS to do more
// processing.

#define BLOCK_SIZE (1<<20)

#define PAGE_TABLE_SIZE (1<<14)

struct vas {
	// A pointer to the first level of the pagetable.
	unsigned int *l1_pagetable;
	unsigned int *l1_pagetable_phys; // The physical address to it

	// A pointer to the next VAS (it's a linked list)
	struct vas *next;
};

int vm_allocate_page(struct vas *vas, void *vptr, int permission);
int vm_free_page(struct vas *vas, void *vptr);

int vm_pin(struct vas *vas, void *vptr);
int vm_unpin(struct vas *vas, void *vptr);

int vm_set_mapping(struct vas *vas, void *vptr, void *pptr, int permission);
int vm_free_mapping(struct vas *vas, void *vptr);

void vm_enable_vas(struct vas *vas);

struct vas *vm_new_vas();

int vm_free_vas(struct vas *vas);

struct vas *vm_get_current_vas();
void vm_use_kernel_vas();

void vm_test();

#endif
