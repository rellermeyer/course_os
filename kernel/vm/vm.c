#include "vm.h"
#include "memory.h"
#include "klibc.h"
#include "frame.h"

#define CHECK_VPTR if ((unsigned int)vptr & (BLOCK_SIZE-1)) return VM_ERR_BADV;
#define CHECK_PPTR if ((unsigned int)pptr & (BLOCK_SIZE-1)) return VM_ERR_BADP;

static const int perm_mapping[16] =
{ 0,  // 0000 Nothing
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

static struct vas *vm_current_vas = (struct vas*) V_L1PTBASE;

struct vm_free_list
{
	struct vm_free_list *next;
};

struct vm_free_list *vm_vas_free_list = 0x0;
struct vm_free_list *vm_l1pt_free_list = 0x0;
struct vm_free_list *vm_l2pt_free_list = 0x0;

void vm_init()
{
	// Initialize the VAS structures. We allocate enough for 4096 VASs.
	struct vm_free_list *free_vas = (struct vm_free_list*) P_L1PTBASE;
	//vm_vas_free_list = free_vas;
	vm_vas_free_list = (struct vm_free_list*) ((void*) free_vas + V_L1PTBASE
			- P_L1PTBASE);
	struct vm_free_list *last = 0x0;
	while ((uint32_t) free_vas < P_L1PTBASE + sizeof(struct vas) * 4096)
	{
		free_vas->next = 0x0;
		if (last)
		{
			last->next = (struct vm_free_list*) ((void*) free_vas + V_L1PTBASE
					- P_L1PTBASE);
		}
		last = free_vas;
		free_vas =
				(struct vm_free_list*) ((void*) free_vas + sizeof(struct vas));
	}

	// Initialize the L1 page tables
	struct vm_free_list *free_l1pt = (struct vm_free_list*) (P_L1PTBASE
			+ sizeof(struct vas) * 4096);
	vm_l1pt_free_list = (struct vm_free_list*) ((void*) free_l1pt + V_L1PTBASE
			- P_L1PTBASE);
	last = 0x0;
	while ((uint32_t) free_l1pt < P_L1PTBASE + (1 << 20) - ((1 << 20) >> 2))
	{
		free_l1pt->next = 0x0;
		if (last)
		{
			last->next = (struct vm_free_list*) ((void*) free_l1pt + V_L1PTBASE
					- P_L1PTBASE);
		}
		last = free_l1pt;
		free_l1pt =
				(struct vm_free_list*) ((void*) free_l1pt + PAGE_TABLE_SIZE);
	}

	// Initialize the L2 coarse page tables
	struct vm_free_list *free_l2pt = (struct vm_free_list*) (P_L1PTBASE
			+ (1 << 19));
	vm_l2pt_free_list = (struct vm_free_list*) ((void*) free_l2pt + V_L1PTBASE
			- P_L1PTBASE);
	last = 0x0;
	while ((uint32_t) free_l2pt < P_L1PTBASE + (1 << 20))
	{
		free_l2pt->next = 0x0;
		if (last)
		{
			last->next = (struct vm_free_list*) ((void*) free_l2pt + V_L1PTBASE
					- P_L1PTBASE);
		}
		last = free_l2pt;
		free_l2pt = (struct vm_free_list*) ((void*) free_l2pt
				+ L2_PAGE_TABLE_SIZE);
	}
}

uint32_t *vm_alloc_coarse_page_table()
{
	// TODO: What if we run out?
	uint32_t *vptr = (uint32_t*) vm_l2pt_free_list;
	if (vptr == 0x0)
	{
		LOG(
				"Could not allocate a coarse page table, bad things will happen soon.\n");
		return NULL;
	}
	vm_l2pt_free_list = ((struct vm_free_list*) vptr)->next;
	os_memset((void*) vptr, 0, L2_PAGE_TABLE_SIZE);
	return vptr;
}

uint32_t *vm_vtop(struct vas *vas, uint32_t *vptr)
{
	// Hack. Assume it's all linearly mapped, and vas == KERNEL_VAS
	if (vas != KERNEL_VAS)
	{
		os_printf("vas is not KERNEL_VAS in vm_vtop. :-(\n");
		while (1)
			;
	}
	return (uint32_t*) ((void*) vptr - V_L1PTBASE + P_L1PTBASE);
}

uint32_t *vm_ptov(struct vas *vas, uint32_t *vptr)
{
	// Hack. Assume it's all linearly mapped, and vas == KERNEL_VAS
	if (vas != KERNEL_VAS)
	{
		os_printf("vas is not KERNEL_VAS in vm_vtop. :-(\n");
		while (1)
			;
	}
	return (uint32_t*) ((void*) vptr + V_L1PTBASE - P_L1PTBASE);
}

struct vas *vm_get_current_vas()
{
	return vm_current_vas;
}

void vm_use_kernel_vas()
{
	vm_enable_vas((struct vas*) V_L1PTBASE);
}

int vm_allocate_page(struct vas *vas, void *vptr, int permission)
{
	CHECK_VPTR;

	// We have to save the current VAS and switch to the kernel VAS
	struct vas *prev_vas = vm_current_vas;
	vm_use_kernel_vas();

	// TODO: Check if the vas already has a mapping there.
	void *pptr = vm_get_free_frame();
	if (pptr == 0x0)
	{
		// We need to swap! (or something...)
		return VM_ERR_UNKNOWN; // For now, just fail
	}

	os_printf("mapping VA %x to PA %x\n", vptr, pptr);

	//LOG("Free frame is at: %X\n", pptr);
	int retval = vm_set_mapping(vas, vptr, pptr, permission);
	if (retval)
	{
		// Release the frame to prevent a memory leak
		os_printf("vm_set_mapping returned %d for 0x%X\n", retval, vptr);
		vm_release_frame(pptr);
		vm_enable_vas(prev_vas);
		return retval;
	}

	vm_enable_vas(prev_vas);
	return 0;
}

void *vm_allocate_pages(struct vas *vas, void *vptr, uint32_t nbytes,
		int permission)
{
	unsigned char *p = (unsigned char*) vptr;
	while (p - (unsigned char*) vptr < nbytes)
	{
		vm_allocate_page(vas, p, permission);
		p += BLOCK_SIZE;
	}
	return p;
}

#define VM_L1_GET_ENTRY(table,vptr) table[((unsigned int)vptr)>>20]
#define VM_L1_SET_ENTRY(table,vptr,ent) (table[((unsigned int)vptr)>>20]=ent)
#define VM_ENTRY_GET_FRAME(x) ((x)&~((PAGE_TABLE_SIZE<<1) - 1))
#define VM_ENTRY_GET_L2(x) ((x)&~0x1FF)
#define VM_L2_ENTRY(l2pt,vptr) ((uint32_t*)l2pt)[((unsigned int)vptr&0x000FF000)>>12]
#define VM_L2ENTRY_GET_FRAME(x) ((x)&0xFFFFF000)

int vm_free_page(struct vas *vas, void *vptr)
{
	CHECK_VPTR;

	// We have to save the current VAS
	struct vas *prev_vas = vm_current_vas;
	vm_enable_vas((struct vas*) KERNEL_VAS);

	// TODO: Check if it was actually allocated
	uint32_t entry = VM_L1_GET_ENTRY(vas->l1_pagetable, vptr);

	// Okay, it's a 4KB page. We need to walk the l2 page table.
	uint32_t *l2pt = vm_ptov(KERNEL_VAS, (uint32_t*) VM_ENTRY_GET_L2(entry));
	entry = VM_L2_ENTRY(l2pt, vptr);
	vm_release_frame((void*) VM_L2ENTRY_GET_FRAME(entry));
	//LOG("Releasing frame %X, l2pt=%X\n", VM_L2ENTRY_GET_FRAME(entry), l2pt);
	VM_L2_ENTRY(l2pt,vptr)= 0;
	//vas->l1_pagetable[(unsigned int)vptr>>20] = 0;

	vm_enable_vas(prev_vas);
	return 0;
}

int vm_pin(struct vas *vas, void *vptr)
{
	// FIXME: unimplemented
	return 0;
}

int vm_unpin(struct vas *vas, void *vptr)
{
	// FIXME: unimplemented
	return 0;
}

int vm_set_mapping(struct vas *vas, void *vptr, void *pptr, int permission)
{
	CHECK_VPTR;
	CHECK_PPTR;
	int perm = perm_mapping[permission];
	if (perm == -1)
		return VM_ERR_BADPERM;

	uint32_t cur_entry = vas->l1_pagetable[(unsigned int) vptr >> 20];
	if ((cur_entry & 3) == 2)
	{
		return VM_ERR_MAPPED;
	}
	if ((cur_entry & 3) == 0)
	{
		// We need to allocate a coarse page table
		uint32_t *vptr_coarse_pt = vm_alloc_coarse_page_table();
		vas->l1_pagetable[(unsigned int) vptr >> 20] = (uint32_t) vm_vtop(
		KERNEL_VAS, vptr_coarse_pt) | 1;
		cur_entry = vas->l1_pagetable[(unsigned int) vptr >> 20];
	}

	uint32_t *l2_pagetable = vm_ptov(KERNEL_VAS,
			(uint32_t*) VM_ENTRY_GET_L2(cur_entry));
	int l2_idx = ((unsigned int) vptr & 0x000FF000) >> 12;
	if (l2_pagetable[l2_idx])
	{
		return VM_ERR_MAPPED;
	}

	//perm &= ~(1<<10); // Clear AP[0] so we get an access exception.
	//vas->l1_pagetable[(unsigned int)vptr>>20] = (unsigned int)pptr | (perm<<10) | 2;
	// TODO: Permissions!
	int lvl2_perm = perm;	//perm_mapping[perm];
	int apx_bit = (lvl2_perm & 4) >> 2;
	int ap_bits = lvl2_perm & 3;
	l2_pagetable[l2_idx] = (unsigned int) pptr | (apx_bit << 9) | (ap_bits << 4)
			| 2;
	//os_printf("pptr: %X, idx=%d, l2pt=%X\n", pptr, l2_idx, l2_pagetable);
	//os_printf("permission=%d lvl2_perm=%X apx=%X ap=%X\n", permission, lvl2_perm, apx_bit, ap_bits);
	//l2_pagetable[l2_idx] = (unsigned int)pptr | (1<<4) | 2;
	return 0;
}

int vm_swap_free_mapping(struct vas *vas, void *vptr, uint32_t *ID)
{
	CHECK_VPTR;
	// TODO: If this is a paged frame, then we need to throw an error
	if ((vas->l1_pagetable[(unsigned int) vptr >> 20] & 3) == 2)
	{
		vas->l1_pagetable[(unsigned int) vptr >> 20] = 0;
	}
	else if ((vas->l1_pagetable[(unsigned int) vptr >> 20] & 3) == 1)
	{
		// We have to free the mapping in the L2 page table
		uint32_t *l2pt = vm_ptov(KERNEL_VAS,
				(uint32_t*) VM_ENTRY_GET_L2(
						vas->l1_pagetable[(unsigned int )vptr >> 20]));
		VM_L2_ENTRY(l2pt, vptr)= (uint32_t) ID;
	}
	return 0;
}

int vm_free_mapping(struct vas *vas, void *vptr)
{
	CHECK_VPTR;
	// TODO: If this is a paged frame, then we need to throw an error
	if ((vas->l1_pagetable[(unsigned int) vptr >> 20] & 3) == 2)
	{
		vas->l1_pagetable[(unsigned int) vptr >> 20] = 0;
	}
	else if ((vas->l1_pagetable[(unsigned int) vptr >> 20] & 3) == 1)
	{
		// We have to free the mapping in the L2 page table
		uint32_t *l2pt = vm_ptov(KERNEL_VAS,
				(uint32_t*) VM_ENTRY_GET_L2(
						vas->l1_pagetable[(unsigned int )vptr >> 20]));
		VM_L2_ENTRY(l2pt, vptr)= 0;
	}
	return 0;
}

// We're going to have to switch to the kernel's VAS, then copy it over.
int vm_map_shared_memory(struct vas *vas, void *this_ptr, struct vas *other_vas,
		void *other_ptr, int permission)
{
	if ((unsigned int) this_ptr & (BLOCK_SIZE - 1))
		return VM_ERR_BADV;
	if ((unsigned int) other_ptr & (BLOCK_SIZE - 1))
		return VM_ERR_BADP;

	struct vas *prev_vas = vm_current_vas;
	vm_enable_vas(KERNEL_VAS);

	if ((vas->l1_pagetable[(unsigned int) this_ptr >> 20] & 3) == 2)
	{
		return VM_ERR_MAPPED;
	}
	if (!other_vas->l1_pagetable[(unsigned int) other_ptr >> 20])
	{
		return VM_ERR_NOT_MAPPED;
	}
	if (!vas->l1_pagetable[(unsigned int) this_ptr >> 20])
	{
		// We need to allocate a coarse page table...
		uint32_t *vptr_coarse_pt = vm_alloc_coarse_page_table();
		vas->l1_pagetable[(unsigned int) this_ptr >> 20] = (uint32_t) vm_vtop(
		KERNEL_VAS, vptr_coarse_pt) | 1;
		//LOG("Allocated coarse page table.\n");
		//LOG("Should be zero: %X (%X)\n", vptr_coarse_pt[0], vptr_coarse_pt);
	}
	uint32_t *this_l2pt = vm_ptov(KERNEL_VAS,
			(uint32_t*) VM_ENTRY_GET_L2(
					vas->l1_pagetable[(unsigned int )this_ptr >> 20]));
	if (VM_L2_ENTRY(this_l2pt, this_ptr))
	{
		//LOG("Should be zero: %X (this_l2pt=%X, idx=%d)\n", VM_L2_ENTRY(this_l2pt, this_ptr), this_l2pt, ((unsigned int)this_ptr&0x000FF000)>>12);
		return VM_ERR_MAPPED;
	}
	uint32_t *other_l2pt = vm_ptov(KERNEL_VAS,
			(uint32_t*) VM_ENTRY_GET_L2(
					other_vas->l1_pagetable[(unsigned int )other_ptr >> 20]));
	if (!VM_L2_ENTRY(other_l2pt, other_ptr))
	{
		return VM_ERR_NOT_MAPPED;
	}

	int perm = perm_mapping[permission];
	if (perm == -1)
		return VM_ERR_BADPERM;

	// Well, this was remarkably easy.
	//unsigned int pptr = VM_ENTRY_GET_FRAME(other_vas->l1_pagetable[(unsigned int)other_ptr>>20]);
	unsigned int pptr = VM_L2ENTRY_GET_FRAME(
			VM_L2_ENTRY(other_l2pt, other_ptr));
	//os_printf("pptr: %X\n",pptr);
	//perm &= ~(1<<10); // Clear AP[0] so we get an access exception.
	//vas->l1_pagetable[(unsigned int)this_ptr>>20] = pptr | (perm<<10) | 2;
	VM_L2_ENTRY(this_l2pt, this_ptr)= pptr | (1<<4) | 2;
	LOG("%X\n", VM_L2_ENTRY(this_l2pt, this_ptr));

	vm_enable_vas(prev_vas);
	return 0;
}

void vm_enable_vas(struct vas *vas)
{
	vm_current_vas = vas;

	// Clear the BTAC
	// Performed by cleaning the caches, below
	//asm volatile("mcr p15, 0, %[r], c7, c5, 6" : : [r] "r" (0x0));

	// Flush the write caches
	asm volatile("MCR p15, 0, %[r], c7, c10, 4" : : [r] "r" (0x0));
	// sync barrier
	asm volatile("MCR p15, 0, %[r], c7, c10, 5" : : [r] "r" (0x0));
	// memory barrier

	//TTBR0
	asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (vas->l1_pagetable_phys));
	// Translation table 1 is currently ignored

	// Clean the caches (data & instruction)
	asm volatile("mcr p15, 0, %[r], c7, c14, 0" : : [r] "r" (0x0));
}

struct vas *vm_new_vas()
{
	if (!vm_vas_free_list)
	{
		return 0x0;
	}
	if (!vm_l1pt_free_list)
	{
		return 0x0;
	}
	struct vas *p = (struct vas*) vm_vas_free_list;
	vm_vas_free_list = vm_vas_free_list->next;

	os_printf("vm_l1pt_free_list=%X\n", vm_l1pt_free_list);
	p->l1_pagetable = (uint32_t*) vm_l1pt_free_list;
	vm_l1pt_free_list = vm_l1pt_free_list->next;

	p->l1_pagetable_phys = (unsigned int*) ((unsigned int) p->l1_pagetable
			- (V_L1PTBASE - P_L1PTBASE));

	// Zero out the page table
	os_memset(p->l1_pagetable, 0, PAGE_TABLE_SIZE);

	// Setup the static mappings...
	// The kernel (high & low addresses)
	//should be less than a MB
	p->l1_pagetable[P_KERNBASE >> 20] = 0 << 20 | 0x0400 | 2;

	//also map it to high memory at 0xf0000000
	p->l1_pagetable[V_KERNBASE >> 20] = 0 << 20 | 0x0400 | 2;
	p->l1_pagetable[(V_KERNBASE + 0x100000) >> 20] = 0x100000 | 0x0400 | 2;

	// Kernel datastructures
	//temporarily map where it is until we copy it in VAS
	p->l1_pagetable[P_KDSBASE >> 20] = P_KDSBASE | 0x0400 | 2;

	//1MB for static kernel data structures (stacks and l1 pt)
	p->l1_pagetable[V_KDSBASE >> 20] = P_KDSBASE | 0x0400 | 2;

	// Our 1MB page to store VAS datastructures
	p->l1_pagetable[V_L1PTBASE >> 20] = P_L1PTBASE | 0x0400 | 2;

	// 2MB of peripheral registers (so we get the serial port et. al.)
	p->l1_pagetable[PERIPHBASE >> 20] = PERIPHBASE | 0x0400 | 2;
	p->l1_pagetable[(PERIPHBASE + 0x100000) >> 20] = (PERIPHBASE + 0x100000)
			| 0x0400 | 2;
	return p;
}

int vm_free_vas(struct vas *vas)
{
	struct vm_free_list *n = (struct vm_free_list*) vas->l1_pagetable;
	n->next = vm_l1pt_free_list;
	vm_l1pt_free_list = n;

	n = (struct vm_free_list*) vas;
	n->next = vm_vas_free_list;
	vm_vas_free_list = n;
	return 0;
}
