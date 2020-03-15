include $(CURDIR)/../config.mk

all: build

CPU = cortex-a7
CFLAGS += -pipe -std=gnu99 -ffreestanding -Wall -Werror -g -O0 -mcpu=arm1176jzf-s -march=armv6zk -mfpu=vfp -fpic -nostartfiles -nodefaultlibs
TOOLCHAIN_PATH:=$(CURDIR)/../$(TOOLCHAIN_DIR)/$(BARE_METAL_TARGET)/bin
CC:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
AS:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-as
LD:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
OBJCOPY:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-objcopy
GDB:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gdb
CC:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc

build:
	@$(CC) $(CFLAGS) -o ./swi/swi ./swi/swi.c
.PHONY: build
