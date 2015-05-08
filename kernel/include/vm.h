#ifndef __VM_H
#define __VM_H 1

#include "memory.h"
#include<stdint.h>

//#define BLOCK_SIZE (1<<20)
#define BLOCK_SIZE (1<<12)

#define PAGE_TABLE_SIZE (1<<14)
#define L2_PAGE_TABLE_SIZE (1<<12)

struct vas {
	// A pointer to the first level of the pagetable.
	unsigned int *l1_pagetable;
	unsigned int *l1_pagetable_phys; // The physical address to it

	// A pointer to the next VAS (it's a linked list)
	struct vas *next;
};

/**
 * Indicates that the virtual pointer passed was invalid. Possible it was not
 * a multiple of BLOCK_SIZE.
 */
#define VM_ERR_BADV -1

/**
 * Indicates that the physical pointer passed was invalid. Possibly it was not
 * a multiple of BLOCK_SIZE.
 */
#define VM_ERR_BADP -2

/**
 * Indicates that the virtual pointer passed has already been mapped.
 */
#define VM_ERR_MAPPED -3

/**
 * Indicates that the permission being passed is not valid. For example,
 * VM_PERM_USER_RO | VM_PERM_USER_RW.
 */
#define VM_ERR_BADPERM -4
#define VM_ERR_UNKNOWN -5

/**
 * Indicates that the pointer passed is not mapped, even though it must be.
 */
#define VM_ERR_NOT_MAPPED -6

#define KERNEL_VAS ((struct vas*)V_L1PTBASE)

/**
 * These permissions dictate who can access what pages. Note that you
 * cannot combine these arbitrarily. For example:
 * VM_PERM_PRIVILEGED_RO|VM_PERM_PRIVILEGED_RW
 * makes no sense. If the user mode is granted a given permission, then the
 * privileged mode is granted that permission as well, unless there is a
 * confliction. Some examples:
 * VM_PERM_USER_RO
 * grants RO status to both user and privileged modes.
 *
 * VM_PERM_USER_RO|VM_PERM_PRIVILEGED_RW
 * grants RO to the user, and RW to privileged modes.
 *
 * VM_PERM_USER_RW|VM_PERM_PRIVILEGED_RO
 * doesn't make sense. USER_RW grants RW to both user and privileged, but
 * PRIVILEGED_RO contradicts the USER_RW's implied grant.
 *
 * VM_ERR_BADPERM is returned if a bad permission is passed. Note that 0
 * is a valid permission, indicating that nobody may have access.
 *
 * (RO stands for Read-Only, RW for Read-Write)
 */
#define VM_PERM_PRIVILEGED_RO 1
#define VM_PERM_USER_RO 2
#define VM_PERM_PRIVILEGED_RW 4
#define VM_PERM_USER_RW 8

void vm_init();

int vm_allocate_page(struct vas *vas, void *vptr, int permission);
void *vm_allocate_pages(struct vas *vas, void *vptr, uint32_t nbytes, int permission);
int vm_free_page(struct vas *vas, void *vptr);

int vm_pin(struct vas *vas, void *vptr);
int vm_unpin(struct vas *vas, void *vptr);

/**
 * vm_set_mapping maps the given vptr to the given pptr.
 * Return values:
 * 0              - success
 * VM_ERR_BADV    - vptr was not a multiple of BLOCK_SIZE.
 * VM_ERR_BADP    - pptr was not a multiple of BLOCK_SIZE.
 * VM_ERR_MAPPED  - vptr is already mapped in this VAS.
 * VM_ERR_BADPERM - permission is not valid.
 *
 * vm_free_mapping unmaps the vptr. Note that you may not call free_mapping
 * on a virtual address that was allocated using allocate_page.
 * Return values:
 * 0                 - success
 * VM_ERR_BADV       - vptr was not a multiple of BLOCK_SIZE.
 * VM_ERR_NOT_MAPPED - vptr was not mapped using set_mapping.
 */
int vm_set_mapping(struct vas *vas, void *vptr, void *pptr, int permission);
int vm_free_mapping(struct vas *vas, void *vptr);

int vm_map_shared_memory(struct vas *vas, void *this_ptr, struct vas *other_vas, void *other_ptr, int permission);

/**
 * This enabled the given VAS.
 */
void vm_enable_vas(struct vas *vas);

/**
 * Allocates a new VAS.
 *
 * Note that there is a limit of 4096 VASs in the system, including the
 * kernel's VAS.
 */
struct vas *vm_new_vas();
int vm_free_vas(struct vas *vas);

/**
 * Retrieves a reference to the currently running VAS.
 */
struct vas *vm_get_current_vas();

/**
 * Switches to the kernel's VAS.
 */
void vm_use_kernel_vas();

void vm_test();

#endif
