#include "file.h"
#include "bitvector.h"
#include "vm.h"
#include "klibc.h"
#include "swap_fs.h"

int swapfs_init(int npages)
{
	int fd; int nbytes;
	// Initialize the free bitmap
	free_bitvector = make_vector(npages);

	// The permissions parameter doesn't quite work right now (BUG?)
	if (kcreate("/swap", 'w', 1) < 0){
		return -1;
	}
	
	// For now, we have to fill the file with that much blank storage.
	if ((fd = kopen("/swap", 'w')) < 0){
		return -1;
	}
	void *empty_page = kmalloc(BLOCK_SIZE);

	// seems useless to me...?
	for (int i = 0; i<npages; i++) {
		if((nbytes = kwrite(fd, empty_page, BLOCK_SIZE)) < 0){
			return -1;
		} 
	}

	kfree(empty_page);
	kclose(fd);

	return nbytes*npages;// success 
}

int swapfs_store(void *page, uint32_t *id)
{
	int idx; int fd;;
	if ((idx = bv_firstFree(free_bitvector)) < 0) {
		return -1;
	}
	
	// Error if invalid index
	if (bv_set(idx, free_bitvector)){
		return -1;
	}

	if ((fd = kopen("/swap", 'w')) < 0){
		return -1;
	}

	// Error if seeks beyond boundaries
	if (kseek(fd, idx*BLOCK_SIZE) < 0){ 
		return -1; 
	}

	if(kwrite(fd, page, BLOCK_SIZE) < 0){
		return 0;
	}
	kclose(fd);

	return idx; // success
} 

int swapfs_retreive(void *page, uint32_t *id)
{
	int fd;
	if (!bv_get(*id, free_bitvector)) {
		return -1;
	}

	if ((fd = kopen("/swap", 'r')) < 0){
		return -1;
	}
	
	kseek(fd, (*id)*BLOCK_SIZE);
	kread(fd, page, BLOCK_SIZE);
	kclose(fd);

	return *id; // success
}

int swapfs_disable(){
	bv_free(free_bitvector);
	if (kdelete("/swap") < 0){
		return -1;
	}

	return 1; // success
}
