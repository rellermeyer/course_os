#include "vm.h"
//#include "frame.h"
#include "klibc.h"

struct vm_free_frame {
	void *next; // Next free frame
};

struct vm_free_frame *vm_free_list;

int vm_build_free_frame_list(void *start, void *end) {
	vm_free_list = start;
	struct vm_free_frame *p = start;
	for (p=start; (void*)p<end; p=(void*)p+BLOCK_SIZE) {
		p->next = (void*)p+BLOCK_SIZE;
		if ((unsigned int)((void*)p+BLOCK_SIZE) >= (unsigned int)end) {
			os_printf("Too much.\n");
			p->next = 0x0;
		}
		os_printf("0x%X -> 0x%X (0x%X), 0x%X\n",p, p->next, (void*)p+BLOCK_SIZE, end);
	}
	return 0;
}

void *vm_get_free_frame() {
	// TODO: Check if there are no frames
	void *p = vm_free_list;
	vm_free_list = vm_free_list->next;
	return p;
}

void vm_release_frame(void *p) {
	// TODO: Check if p is actually a valid frame
	struct vm_free_frame *flist = p;
	flist->next = vm_free_list;
	vm_free_list = p;
}

int vm_count_free_frames() {
	os_printf("free frame list address: %X\n", vm_free_list);
	int cnt = 0;
	struct vm_free_frame *p = vm_free_list;
	while ((p = p->next)) {
		os_printf("0x%X -> 0x%X\n", p, p->next);
		cnt++;
	}
	os_printf("Returning %d\n",cnt);
	return cnt;
}
