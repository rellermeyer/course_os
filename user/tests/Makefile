include $(CURDIR)/../../config.mk

TOOLCHAIN_PATH:=$(CURDIR)/../../$(TOOLCHAIN_DIR)/$(BARE_METAL_TARGET)/bin
CC:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
LD:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-ld

CFLAGS += -Os -pipe
CFLAGS += -std=c99 -ffreestanding -nostdinc

SPECS = -specs "../gcc.specs"

PROGS  = test_0
PROGS += test_1
PROGS += test_2
PROGS += test_3
PROGS += test_4
PROGS += test_5
PROGS += test_6
PROGS += test_7
PROGS += test_8
PROGS += test_9
PROGS += test_10
PROGS += test_11
PROGS += test_12

all: $(PROGS)

$(PROGS): %: %.o
	$(CC) $(SPECS) -Wl,-Ttext-segment=0x9ff08000 -L../libc -static -lc $< -o $@

%.o: %.c
	$(CC) $(SPECS) $(CFLAGS) -I../include -c $< -o $@

clean:
	rm -f $(foreach prog,$(PROGS),$(prog).o)
	rm -f $(PROGS)

