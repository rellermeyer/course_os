#include "vm.h"

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
	return 0x0;
}

int vm_free_vas(struct vas *vas) {
	return 0;
}
