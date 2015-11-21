include $(CURDIR)/../../config.mk

TOOLCHAIN_PATH:=$(CURDIR)/../../$(TOOLCHAIN_DIR)/$(BARE_METAL_TARGET)/bin
CC:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
LD:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-ld

CFLAGS = -Os -pipe
CFLAGS += -std=c99 -ffreestanding -nostdinc

PROGS  = hello
PROGS += hello1
PROGS += hello2

SPECS = -specs "../gcc.specs"

all: $(PROGS)

$(PROGS): %: %.o
	$(CC) $(SPECS) -Wl,-Ttext-segment=0x9ff08000 -L../libc -static -lc $< -o $@

%.o: %.c
	$(CC) $(SPECS) $(CFLAGS) -I../include -c -mcpu=arm1176jzf-s $< -o $@

clean:
	rm -f $(foreach prog,$(PROGS),$(prog).o)
	rm -f $(PROGS)
