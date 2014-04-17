#include "include/mmap.h"
#include "include/vmlayout.h"
#include "include/klibc.h"
#include "include/pmap.h"

unsigned int next_frame = PMSTART;
unsigned int next_page = PCIEND;

/*
 * APX/AP bits for a page table entry are at bits 9 and 4&5. The
 * meaning is the same as for a section entry.
 * 0 01 (0x0010) =  read-write privileded modes, no access otherwise
 *
 * bits 0 and 1 determine the page type:
 * 0 = unmapped, translation fault
 * 1 = large page (64K)			(XN is bit 15)
 * 2 = small page (4K), executable	(XN is bit 0)
 * 3 = small page (4K), not-executable  (XN is bit 0)
 * 
 * 256 entries, one for each 4KB in the 1MB covered by the table
 */

unsigned int get_page(unsigned int num_pages){
	unsigned int frame = next_frame;
	unsigned int page = next_page;

	if(next_frame + (num_pages*FRAMESIZE) < (unsigned int)PMEND){
		next_frame = next_frame + (num_pages * FRAMESIZE);
		next_page = next_page + (num_pages * FRAMESIZE);
	}
	else
		return 0;

	os_printf("\npage= %x", page);
	unsigned int l1_idx = page>>20;
	os_printf("\nl1_idx= %d\n", l1_idx);
	unsigned int mb = l1_idx<<20;
	os_printf("mb= %x\n", mb);

	unsigned int l2_idx_s = (page-mb)>>12;
	os_printf("l2_idx_s= %d\n", l2_idx_s);

	int l2_idx_e = l2_idx_s + num_pages;

	unsigned int * l2pt_addr = v_first_level_pt[l1_idx];
	os_printf("l2addr= %x\n", l2pt_addr);


	int i;
	for(i = l2_idx_s; i<l2_idx_e; i++){
		l2pt_addr[i] = frame | 0x0010 | 2;
		os_printf("%x\n", l2pt_addr[i]);
		frame += FRAMESIZE;
	}

	return page;
}

