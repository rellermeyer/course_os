TOOLCHAIN_DIR=toolchain

BARE_METAL_TUPLE=arm-none-eabi
BARE_METAL_TARGET:=$(BARE_METAL_TUPLE)

QEMU=./qemu/qemu/bin/qemu-system-arm

#CFLAGS = -mcpu=arm1136j-s
CFLAGS = -mcpu=arm1176jz-s
