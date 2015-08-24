#include "file.h"
#include "bitvector.h"
#include "vm.h"
#include "klibc.h"
#include "swap_fs.h"

char *generate_filename(uint8_t ssid)
{
	char *output;
	if (ssid < 10)
	{ // single digit
		char swapfile[9] =
		{ '/', 's', 'w', 'a', 'p', '0', '0', ssid + 48, 0 };
		output = swapfile;
	}
	else if (ssid < 100)
	{ // double digit
		int tmp = ssid / 10;
		char swapfile[9] =
		{ '/', 's', 'w', 'a', 'p', '0', tmp + 48, (ssid % 10) + 48, 0 };
		output = swapfile;
	}
	else
	{ // triple digit
		int tmp = ssid / 10;
		int tmp2 = ssid / 100;
		char swapfile[9] =
		{ '/', 's', 'w', 'a', 'p', tmp2 + 48, tmp + 48, ssid + 48, 0 };
		output = swapfile;
	}
	return output;
}

int32_t swapfs_init(int npages, uint8_t ssid)
{
	int fd;
	int nbytes;
	// Initialize the free bitmap
//	free_bitvector = make_vector(npages);
	bv_arr[ssid] = make_vector(npages);

	// making new files instead of a tree b/c the FS can't do it
	char *swapfile = generate_filename(ssid);

	// The permissions parameter doesn't quite work right now (BUG?)
	if (kcreate(swapfile, 'w', 1) < 0)
	{
		return -1;
	}

	// For now, we have to fill the file with that much blank storage.
	if ((fd = kopen(swapfile, 'w')) < 0)
	{
		return -1;
	}
	void *empty_page = kmalloc(BLOCK_SIZE);

	// seems useless to me...?
	for (int i = 0; i < npages; i++)
	{
		if ((nbytes = kwrite(fd, empty_page, BLOCK_SIZE)) < 0)
		{
			return -1;
		}
	}

	kfree(empty_page);
	kclose(fd);

	return nbytes * npages; // success 
}

int64_t swapfs_store(void *page, uint32_t *id, uint8_t ssid)
{
	int32_t fd;
	char *swapfile;
	int32_t b;
	// Get the index to store the page into
	*id = *((uint32_t*) page) & 0xFFFFFF00;

	if ((b = bv_isfree(*id, bv_arr[ssid])) <= 0)
	{
		if (b == -1)
		{
			os_printf("invalid index [swapfs_store]");
		}
		else
		{
			os_printf("index not free [swapfs_store]");
		}

		return -1;
	}

	// Error if invalid index
	if (bv_set(*id, bv_arr[ssid]) < 0)
	{
		return -1;
	}

	swapfile = generate_filename(ssid);
	// Error if failed to open
	if ((fd = kopen(swapfile, 'w')) < 0)
	{
		return -1;
	}

	// Error if seeks beyond boundaries
	if (kseek(fd, (*id) * BLOCK_SIZE) < 0)
	{
		return -1;
	}

	if (kwrite(fd, page, BLOCK_SIZE) < 0)
	{
		return -1;
	}
	kclose(fd);

	return *id; // success
}

int64_t swapfs_retreive(void *page, uint32_t *id, uint8_t ssid)
{
	int fd;
	char *swapfile;
	//checks if index is free
	if (!bv_get(*id, bv_arr[ssid]))
	{
		return -1;
	}

	swapfile = generate_filename(ssid);

	// not sure if I should make the index free here...
	bv_lower(*id, bv_arr[ssid]);

	if ((fd = kopen(swapfile, 'r')) < 0)
	{
		return -1;
	}

	kseek(fd, (*id) * BLOCK_SIZE);
	kread(fd, page, BLOCK_SIZE);
	kclose(fd);

	return 1; // success
}

int32_t swapfs_disable(uint8_t ssid)
{
	char *swapfile;
	bv_free(bv_arr[ssid]);
	swapfile = generate_filename(ssid);
	if (kdelete(swapfile, 0) < 0)
	{
		return -1;
	}

	return 1; // success
}
