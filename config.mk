TOOLCHAIN_DIR=toolchain

BARE_METAL_TUPLE=arm-none-eabi
BARE_METAL_TARGET:=$(BARE_METAL_TUPLE)

QEMU=qemu-system-arm

UBOOT_VERSION=2014.10

#CFLAGS = -mcpu=arm1136j-s
CFLAGS = -mcpu=arm1176jz-s
