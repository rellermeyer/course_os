include $(CURDIR)/../../config.mk

TOOLCHAIN_PATH:=$(CURDIR)/../../$(TOOLCHAIN_DIR)/$(BARE_METAL_TARGET)/bin
CC:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
INCL:=$(CURDIR)/../include/
MAKE:=$(CURDIR)/../
CFLAGS = -pipe -std=c99 -ffreestanding

default: tests

tests: test_vm.o test_hash_map.o test_mem_alloc.o test_priority_queue.o test_fs.o
	$(MAKE)Makefile
	$(CC) $(CFLAGS) -lc -o tests test_hash_map.o test_mem_alloc.o test_priority_queue.o test_vm.o test_fs.o

test_fs.o: test_fs.c $(INCL)/tests.h $(INCL)/file.h ../fs/cmdline/file.h
	$(CC) $(CFLAGS) -lc -o test_fs test_fs.c

test_hash_map.o: test_hash_map.c $(INCL)/tests.h $(INCL)/tests/test_hash_map.h $(INCL)hash_map.h $(INCL)/klibc.h
	$(CC) $(CFLAGS) -lc -o test_hash_map test_hash_map.c 

test_mem_alloc.o: test_mem_alloc.c $(INCL)/tests.h $(INCL)/tests/test_mem_alloc.h $(INCL)/klibc.h $(INCL)/vm.h
	$(CC) $(CFLAGS) -lc -o test_mem_alloc test_mem_alloc.c

test_priority_queue.o: test_priority_queue.c $(INCL)/tests.h $(INCL)/tests/test_priority_queue.h $(INCL)/priority_queue.h $(INCL)/klibc.h
	$(CC) $(CFLAGS) -lc -o test_priority_queue test_priority_queue.c

test_vm.o: test_vm.c $(INCL)/tests.h $(INCL)/klibc.h $(INCL)/memory.h $(INCL)/vm.h
	$(CC) $(CFLAGS) -lc -o test_vm test_vm.c

%.o: %.c
	$(CC) $(CFLAGS) -c -mcpu=arm1176jzf-s $< -o $@

clean:
	rm -f *.o
	rm -f tests
	rm -f test_fs
	rm -f test_vm
	rm -f test_hash_map
	rm -f test_mem_alloc
	rm -f test_priority_queue