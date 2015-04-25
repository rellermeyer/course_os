include $(CURDIR)/../config.mk

TOOLCHAIN_PATH:=$(CURDIR)/../$(TOOLCHAIN_DIR)/$(BARE_METAL_TARGET)/bin
CC:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
AS:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-as
LD:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
OBJCOPY:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-objcopy
MKIMAGE:=$(CURDIR)/../u-boot/u-boot-$(UBOOT_VERSION)/tools/mkimage

OBJS=start.o drivers/uart.o startup.o argparse.o klibc.o interrupt.o hw_handlers.o process.o elf.o loader.o memory/mem_alloc.o tests.o mmap.o vm/frame.o vm/vm.o drivers/timer.o data_structures/bitvector.o data_structures/hashtable.o mq/streams.o

all: kernel.img

CFLAGS += -pipe
CFLAGS += -std=gnu99 
CFLAGS += -ffreestanding 
CFLAGS += -nostdinc
CFLAGS += -Wall
#CFLAGS += -Werror

run: flash.bin
#	(cat uboot-commands.ubt; cat) | ${QEMU} -M versatilepb -m 128M -nographic -kernel flash.bin -append "-load 0x410000 0x14000"
	${QEMU} -M versatilepb -sd sd.img -m 128M -nographic -kernel flash.bin -append "-load 0x410000 0x14000"

#boots the kernel at 0x210000
#for now, sticks the hello binary at 0x240000
flash.bin: kernel.img script.img
	dd if=/dev/zero of=sd.img conv=notrunc bs=128M count=1
	dd if=/dev/zero of=flash.bin bs=4k count=1536
	dd if=../u-boot/u-boot-$(UBOOT_VERSION)/u-boot.bin of=flash.bin conv=notrunc bs=4k
	dd if=kernel.img of=flash.bin conv=notrunc bs=4k seek=512
	dd if=script.img of=flash.bin conv=notrunc bs=4k seek=575
#	dd if=../user/hello/hello of=flash.bin conv=notrunc bs=4k seek=576

kernel.elf: $(OBJS)
	$(LD) -T kernel.ld -nostartfiles -Wl,-Map,kernel.map $(OBJS) -o kernel.elf

kernel.bin: kernel.elf
	dd if=/dev/zero of=kernel.bin bs=4k count=64
	$(OBJCOPY) -O binary kernel.elf kernel2.bin
	dd if=kernel2.bin of=kernel.bin conv=notrunc bs=4k
	dd if=../user/hello/hello of=kernel.bin conv=notrunc bs=4k seek=16

kernel.img: kernel.bin
	$(MKIMAGE) -A arm -C none -O linux -T kernel -d kernel.bin -a 0x00010000 -e 0x00010000 kernel.img

script.img: uboot-commands.ubt
	$(MKIMAGE) -A arm -C none -T script -d uboot-commands.ubt -a 0x10000 -e 0x10000 script.img

course_os.img: kernel.img
	cat ../u-boot/u-boot-$(UBOOT_VERSION)/u-boot.bin kernel.img > course_os.img

startup.o: startup.s
	$(AS) -mcpu=arm1176jzf-s -g startup.s -o startup.o

%.o: %.c
	$(CC) $(CFLAGS) -Iinclude -c -mcpu=arm1176jzf-s $< -o $@

clean:
	rm -f *.o
	rm -f *.img
	rm -f *.elf
	rm -f *.bin
	rm -f *.map
	rm -f drivers/*.o
	rm -f vm/*.o
