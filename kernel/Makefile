
include $(CURDIR)/../config.mk

TOOLCHAIN_PATH:=$(CURDIR)/../$(TOOLCHAIN_DIR)/$(BARE_METAL_TARGET)/bin
CC:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
AS:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-as
LD:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
OBJCOPY:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-objcopy
MKIMAGE:=$(CURDIR)/../u-boot/u-boot-$(UBOOT_VERSION)/tools/mkimage
GDB:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gdb

PIOBJS = start.o pi_light.o startup.o drivers/uart_pi.o argparse.o klibc.o interrupt.o hw_handlers.o process.o elf.o loader.o memory/mem_alloc.o memory/allocator.o tests.o mmap.o vm/frame.o vm/vm.o drivers/timer.o data_structures/bitvector.o data_structures/array_list.o data_structures/priority_queue.o data_structures/linked_list.o data_structures/hash_map.o process/scheduler.o tests/test_mem_alloc.o tests/test_priority_queue.o tests/test_hash_map.o tests/test_vm.o fs/open_table.o fs/fat16/file.o tests/test_fs.o drivers/mmci.o

OBJS=start.o drivers/uart.o startup.o argparse.o klibc.o interrupt.o hw_handlers.o process.o elf.o loader.o memory/mem_alloc.o memory/allocator.o tests.o mmap.o vm/frame.o vm/vm.o drivers/timer.o process/scheduler.o data_structures/bitvector.o data_structures/array_list.o data_structures/linked_list.o data_structures/priority_queue.o data_structures/hash_map.o tests/test_mem_alloc.o tests/test_priority_queue.o tests/test_hash_map.o tests/test_vm.o fs/open_table.o fs/fat16/file.o tests/test_fs.o drivers/mmci.o

all: flash.bin card.sd

make_Pi: kernelPi.img

CFLAGS += -pipe
CFLAGS += -std=gnu99 
CFLAGS += -ffreestanding 
CFLAGS += -nostdinc
CFLAGS += -Wall
CFLAGS += -Werror
PFLAGS += -mfpu=vfp
#PFLAGS += -mfloat-abi=hard
PFLAGS += -march=armv6zk
PFLAGS += -mtune=arm1176jzf-s
PFLAGS += -nostartfiles
CFLAGS += -g


run: flash.bin card.sd
	${QEMU} -M versatilepb -sd card.sd -m 128M -nographic -kernel flash.bin -append "-load 0x410000 0x14000"

run-debug: flash.bin card.sd
	${QEMU} -S -s -M versatilepb -sd card.sd -m 128M -nographic -kernel flash.bin -append "-load 0x410000 0x14000"

gdb:
	${GDB} -ex "target remote :1234" kernel.elf

#boots the kernel at 0x210000
flash.bin: kernel.img script.img
	dd if=/dev/zero of=flash.bin bs=4k count=1536
	dd if=../u-boot/u-boot-$(UBOOT_VERSION)/u-boot.bin of=flash.bin conv=notrunc bs=4k
	dd if=kernel.img of=flash.bin conv=notrunc bs=4k seek=512
	dd if=script.img of=flash.bin conv=notrunc bs=4k seek=575

# 32678*4096 = 128MiB
card.sd: fs-cmdline userhello
	dd if=/dev/zero of=card.sd conv=notrunc bs=4096 count=32768
	fs/cmdline/buildfs ../user/hello/ ../user/tests

userhello:
	make -C ../user/hello

fs-cmdline:
	make -C fs/cmdline

kernel.elf: $(OBJS)
	$(LD) -T kernel.ld -nostartfiles -Wl,-Map,kernel.map $(OBJS) -o kernel.elf

kernel.bin: kernel.elf
	$(OBJCOPY) -O binary kernel.elf kernel.bin

kernel.img: kernel.bin
	$(MKIMAGE) -A arm -C none -O linux -T kernel -d kernel.bin -a 0x00010000 -e 0x00010000 kernel.img

# Begin Pi Make
kernelPi.elf: $(PIOBJS)
	$(CC) -T kernelPi.ld -O2 $(PFLAGS) $(PIOBJS) -o kernelPi.elf

kernelPi.img: kernelPi.elf
	$(OBJCOPY) kernelPi.elf -O binary kernelPi.img

# End Pi Make

script.img: uboot-commands.ubt
	$(MKIMAGE) -A arm -C none -T script -d uboot-commands.ubt -a 0x10000 -e 0x10000 script.img

course_os.img: kernel.img
	cat ../u-boot/u-boot-$(UBOOT_VERSION)/u-boot.bin kernel.img > course_os.img

startup.o: startup.s
	$(AS) -mcpu=arm1176jzf-s -g startup.s -o startup.o

%.o: %.c
	$(CC) $(CFLAGS) -Iinclude -c -mcpu=arm1176jzf-s $< -o $@

clean:
	rm -f $(OBJS)
	rm -f *.img
	rm -f *.elf
	rm -f *.bin
	rm -f *.map
	rm -f drivers/*.o
	rm -f fs/cmdline/buildfs
	rm -f vm/*.o
	rm -f tests/*.o
	rm -f memory/*.o
	rm -f data_structures/*.o
	rm -f mq/*.o
	@echo "I'm persisting the SD card 'card.sd', you'll have to remove that yourself if you want to rebuild it."
	@echo "('rm sd.card')"
