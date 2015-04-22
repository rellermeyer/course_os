linclude $(CURDIR)/../../config.mk

TOOLCHAIN_PATH:=$(CURDIR)/../../$(TOOLCHAIN_DIR)/$(BARE_METAL_TARGET)/bin
CC:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
LD:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-ld

CFLAGS = -Os -pipe
CFLAGS += -std=c99 -ffreestanding -nostdinc

SPECS = -specs "../gcc.specs"

hello: hello.o
	$(CC) $(SPECS) -L../libc -static -lc -o hello hello.o
	
%.o: %.c
	$(CC) $(SPECS) $(CFLAGS) -I../include -c -mcpu=arm1176jzf-s $< -o $@

clean:
	rm -f hello.o
	rm -f hello
