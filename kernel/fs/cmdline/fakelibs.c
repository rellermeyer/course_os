#include<stdlib.h>
#include<stdio.h>
#include<string.h>

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
int os_memcpy(void *src, void *dst, int len)
{
	return memcpy(dst, src, len);
}

int os_memset(void *ptr, char c, int len)
{
	return memset(ptr, c, len);
}

int os_strcmp(char *s1, char *s2)
{
	return strcmp(s1, s2);
}

int os_strcpy(char *s1, char *s2)
{
	return strcpy(s1, s2);
}

int os_strlen(char *s)
{
	return strlen(s);
}

/* SD card */

void init_sd()
{
	// Noop
}

void sd_transmit(void *data, int addr)
{
	FILE *f = fopen("card.sd", "r+");
	fseek(f, addr, SEEK_SET);
	fwrite(data, 1, 512, f);
	fclose(f);
}

void sd_receive(void *data, int addr)
{
	FILE *f = fopen("card.sd", "r+");
	fseek(f, addr, SEEK_SET);
	fread(data, 1, 512, f);
	fclose(f);
}
