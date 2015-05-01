#include "file.h"
#include "bitvector.h"
#include "vm.h"
#include "swap_fs.h"

static bit_vector *free_bitvector;

int swapfs_init(int npages)
{
	// Initialize the free bitmap
	free_bitvector = make_vector(npages);

	// For now, we have to fill the file with that much blank storage.
	int fd = kopen("/swap", 'w');
	void *empty_page = kmalloc(BLOCK_SIZE);
	int i;
	for (i=0; i<npages; i++) {
		kwrite(fd, empty_page, BLOCK_SIZE);
	}
	kfree(empty_page);
	kclose(fd);
}

int swapfs_store(void *page, uint32_t *id)
{
	int idx = bv_firstFree(free_bitvector);
	if (idx < 0) return -1;
	bv_set(idx, free_bitvector);

	int fd = kopen("/swap", 'w');
	kseek(fd, idx*BLOCK_SIZE);
	kwrite(fd, page, BLOCK_SIZE);
	kclose(fd);
	return idx;
}

int swapfs_receive(void *page, uint32_t *id)
{
	if (!bv_get(id, free_bitvector)) {
		return -1;
	}
	int fd = kopen("/swap", 'w');
	kseek(fd, *id*BLOCK_SIZE);
	kread(fd, page, BLOCK_SIZE);
	kclose(fd);
	return *id; // Success
}
