include $(CURDIR)/../../config.mk

TOOLCHAIN_PATH:=$(CURDIR)/../../$(TOOLCHAIN_DIR)/$(BARE_METAL_TARGET)/bin
CC:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
AR:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-ar

OBJS=stdio/printf.o \
     stdio/vfprintf.o \
     stdio/fwrite.o \
     stdio/stdout.o \
     stdio/__stdio_write.o \
     stdio/__stdio_seek.o \
     stdio/__stdio_close.o \
     stdio/__stdio_exit.o \
     stdio/__stdout_write.o \
     stdio/__towrite.o \
     stdio/__lockfile.o \
     string/memcpy.o \
     string/memset.o \
     string/memchr.o \
     errno/__errno_location.o \
     errno/strerror.o \
     math/frexpl.o \
     math/frexp.o \
     multibyte/wctob.o \
     multibyte/wctomb.o \
     multibyte/wcrtomb.o \
     internal/syscall.o \
     internal/syscall_ret.o \
     internal/libc.o \
     dummy/dummy.o \
     env/__libc_start_main.o \
     env/__environ.o \
     malloc/malloc_syscalls.o \
     exit/exit.o \
     exit/_Exit.o \
     exit/assert.o \
     file_system/fs_syscalls.o \
     thread/thread.o

CFLAGS = -Os -pipe
CFLAGS += -std=c99 -ffreestanding -nostdinc
CFLAGS += -D_XOPEN_SOURCE=700

all: libc.a

libc.a: $(OBJS) 
	$(AR) rcs libc.a $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -Iarch/arm -Iinternal -I../include -c -mcpu=arm1176jzf-s $< -o $@

%.o: %.s
	$(CC) $(CFLAGS) -Iarch/arm -Iinternal -I../include -c -mcpu=arm1176jzf-s $< -o $@

clean: 
	rm -f stdio/*.o
	rm -f string/*.o
	rm -f errno/*.o
	rm -f math/*.o
	rm -f multibyte/*.o
	rm -f internal/*.o
	rm -f dummy/*.o
	rm -f env/*.o
	rm -f exit/*.o
	rm -f thread/*.o
	rm -f libc.a
