#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* Memory management */

void *kmalloc(unsigned int len)
{
	return malloc(len);
}

void kfree(void *ptr)
{
	free(ptr);
}

/* klibc functions */

// GAH src is the 1st argument.
void *os_memcpy(void *src, void *dst, int len)
{
	return memcpy(dst, src, len);
}

void *os_memset(void *ptr, char c, int len)
{
	return memset(ptr, c, len);
}

int os_strcmp(char *s1, char *s2)
{
	return strcmp(s1, s2);
}

char *os_strcpy(char *s1, char *s2)
{
	return strcpy(s1, s2);
}

int os_strlen(char *s)
{
	return strlen(s);
}

/* SD card */

int init_sd()
{
	// Noop
	return 0;
}

int sd_transmit(void *data, uint32_t addr)
{
	FILE *f = fopen("card.sd", "r+");
	fseek(f, addr, SEEK_SET);
	fwrite(data, 1, 512, f);
	fclose(f);
	return 0;
}

int sd_receive(void *data, uint32_t addr)
{
	FILE *f = fopen("card.sd", "r+");
	fseek(f, addr, SEEK_SET);
	fread(data, 1, 512, f);
	fclose(f);
	return 0;
}
