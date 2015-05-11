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

#define VM_ERR_BADV -1
#define VM_ERR_BADP -2
#define VM_ERR_MAPPED -3
#define VM_ERR_BADPERM -4
#define VM_ERR_UNKNOWN -5
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

/**
 * vm_allocate_page and vm_free_page allocate and free pages, and allow the
 * VAS to access them at the given virtual address (vptr).
 * Note that you cannot call free_page on a virtual address that was
 * mapped using set_mapping.
 * Return values:
 * 0              - success
 * VM_ERR_BADV    - vptr was not a multiple of BLOCK_SIZE.
 * VM_ERR_MAPPED  - vptr is already mapped in this VAS.
 * VM_ERR_BADPERM - permission is not valid (see above).
 *
 * free_page return values:
 * 0                 - success
 * VM_ERR_BADV       - vptr was not a multiple of BLOCK_SIZE.
 * VM_ERR_MAPPED     - vptr was mapped using set_mapping, not allocate_page.
 * VM_ERR_NOT_MAPPED - vptr is not mapped in this VAS.
 */
int vm_allocate_page(struct vas *vas, void *vptr, int permission);
void *vm_allocate_pages(struct vas *vas, void *vptr, uint32_t nbytes, int permission);
int vm_free_page(struct vas *vas, void *vptr);

/**
 * vm_pin and vm_unpin currently do nothing. However, in the future, they will
 * prevent pages from being swapped out of physical memory.
 * Return values:
 * 0 - Success.
 */
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


/* vm_swap_free_mapping is the spapping frameworks flavor on vm's vm_free_mapping
 * It takes in an extra paramter that will be set to the L2 page entry (instead of 0)
 * - Noel Negusse
 *
 * Returns: Same as vm_free_mapping 
 */
int vm_swap_free_mapping(struct vas*, void*, uint32_t*);


/**
 * Will make the memory that accessible to other_vas at other_ptr
 * accessible to vas at this_ptr.
 * It can then be unmapped from either using a typical vm_free_mapping
 * or vm_free_page. The behavior of vm_free_mapping and vm_free_page
 * will be made equivalent.
 * For both, the behavior will be to free the frame if a frame was
 * allocated (via vm_allocate_page) and the page is not shared. If the
 * page is shared, then both will behave like vm_free_mapping.
 *
 * It is an error for other_ptr to point to an area not mapped in other_vas.
 *
 * For example, given vas1 and vas2:
 *
 * vm_allocate_page(vas2,0x10)
 * vm_map_shared_memory(vas1, 0x20, vas2, 0x10)
 * // Now vas1's 0x20 points to the same memory as vas2's 0x10
 * vm_free_page(vas2, 0x10)
 * // Now vas1 is the sole owner of the memory at vas1's 0x20
 * vm_free_mapping(vas1, 0x20)
 * // Now that frame has been freed
 *
 * All the pointers have to be a multiple of BLOCK_SIZE.
 *
 * Return values:
 * 0                 - success
 * VM_ERR_BADV       - this_ptr was not a multiple of BLOCK_SIZE.
 * VM_ERR_BADP       - other_ptr was not a multiple of BLOCK_SIZE.
 * VM_ERR_NOT_MAPPED - other_ptr is not mapped in other_vas.
 * VM_ERR_MAPPED     - this_ptr is already mapped in vas.
 * VM_ERR_BADPERM    - permission is bad.
 */
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
