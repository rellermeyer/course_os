#ifndef __VM_H
#define __VM_H 1

#define BLOCK_SIZE (1<<20)

struct vas {
	// Hrm...
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

#endif
