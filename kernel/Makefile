include $(CURDIR)/../config.mk

TOOLCHAIN_PATH:=$(CURDIR)/../$(TOOLCHAIN_DIR)/$(BARE_METAL_TARGET)/bin
CC:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
AS:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-as
LD:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-ld
OBJCOPY:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-objcopy
MKIMAGE:=$(CURDIR)/../u-boot/u-boot-$(UBOOT_VERSION)/tools/mkimage


OBJS=start.o drivers/uart.o startup.o argparse.o klibc.o mmap.o stacks.o main.o interrupt.o hw_handlers.o process.o elf.o memory/mem_alloc.o loader.o tests.o

all: kernel.img

CFLAGS = -pipe
CFLAGS += -std=gnu99 -ffreestanding -nostdinc

run: flash.bin
	${QEMU} -M versatilepb -m 128M -nographic -kernel flash.bin

#boots the kernel at 0x210000
flash.bin: kernel.img sampleFile
	dd if=/dev/zero of=flash.bin bs=4k count=1536
	dd if=../u-boot/u-boot-$(UBOOT_VERSION)/u-boot.bin of=flash.bin conv=notrunc bs=4k
	dd if=kernel.img of=flash.bin conv=notrunc bs=4k seek=512	
	dd if=sampleFile of=flash.bin conv=notrunc bs=4k seek=2048

kernel.elf: $(OBJS)
	$(LD) -T kernel.ld -Map=kernel.map $(OBJS) -o kernel.elf

kernel.bin: kernel.elf
	$(OBJCOPY) -O binary kernel.elf kernel.bin

kernel.img: kernel.bin
	$(MKIMAGE) -A arm -C none -O linux -T kernel -d kernel.bin -a 0x00010000 -e 0x00010000 kernel.img

startup.o: startup.s
	$(AS) -mcpu=arm1176jzf-s -g startup.s -o startup.o

%.o: %.c
	$(CC) $(CFLAGS) -Iinclude -c -mcpu=arm1176jzf-s $< -o $@

sampleFile: sampleFile.c
	$(CC) $(CFLAGS) -Iinclude -c -mcpu=arm1176jzf-s sampleFile.c -o sampleFile

clean:
	rm -f *.o
	rm -f *.img
	rm -f *.elf
	rm -f *.bin
	rm -f *.map
	rm -f drivers/*.o
