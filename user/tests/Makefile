include $(CURDIR)/../../config.mk

TOOLCHAIN_PATH:=$(CURDIR)/../../$(TOOLCHAIN_DIR)/$(BARE_METAL_TARGET)/bin
CC:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
LD:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-ld

CFLAGS = -Os -pipe
CFLAGS += -std=c99 -ffreestanding -nostdinc

SPECS = -specs "../gcc.specs"

all: test_1 test_2

test_1: test_1.o
	$(CC) $(SPECS) -Wl,-Ttext-segment=0x9ff08000 -L../libc -static -lc -o test_1 test_1.o

test_2: test_2.o
	$(CC) $(SPECS) -L../libc -static -lc -o test_2 test_2.o

%.o: %.c
	$(CC) $(SPECS) $(CFLAGS) -Wl,-Ttext-segment=0x9ff28000 -I../include -c -mcpu=arm1176jzf-s $< -o $@

clean:
	rm -f test_1.o
	rm -f test_1
	rm -f test_2.o
	rm -f test_2
