
include $(CURDIR)/../config.mk

TOOLCHAIN_PATH:=$(CURDIR)/../$(TOOLCHAIN_DIR)/$(BARE_METAL_TARGET)/bin
CC:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
AS:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-as
LD:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
OBJCOPY:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-objcopy
MKIMAGE:=$(CURDIR)/../u-boot/u-boot-$(UBOOT_VERSION)/tools/mkimage
GDB:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gdb

COMMONOBJS = build/start.o build/startup.o build/argparse.o             \
			 build/klibc.o build/interrupt.o build/process.o            \
			 build/hw_handlers.o build/elf.o build/loader.o             \
			 build/memory/mem_alloc.o build/memory/allocator.o          \
			 build/mmap.o build/vm/frame.o build/vm/vm.o                \
			 build/drivers/timer.o build/data_structures/bitvector.o    \
			 build/data_structures/array_list.o 						\
			 build/data_structures/priority_queue.o                     \
			 build/data_structures/linked_list.o                        \
			 build/data_structures/hash_map.o build/process/scheduler.o \
			 build/fs/open_table.o build/fs/fat16/file.o                \
			 build/drivers/mmci.o



TESTOBJS = build/tests.o build/tests/test_mem_alloc.o build/tests/test_hash_map.o \
		   build/tests/test_priority_queue.o build/tests/test_vm.o                \
		   build/tests/test_fs.o

PIOBJS = build/pi_light.o build/drivers/uart_pi.o
OBJS = build/drivers/uart.o

OBJS += $(COMMONOBJS) $(TESTOBJS)
PIOBJS += $(COMMONOBJS) $(TESTOBJS)


all: build/flash.bin build/card.sd

make_Pi: build/kernelPi.img

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

run: build/flash.bin build/card.sd | builddir
	${QEMU} -M versatilepb -cpu arm1176 -sd card.sd -m 128M -nographic -kernel build/flash.bin -append "-load 0x410000 0x14000"

test: build/flash.bin build/card.sd | builddir
	${QEMU} -M versatilepb -cpu arm1176 -sd card.sd -m 128M -nographic -kernel build/flash.bin -append "-load 0x410000 0x14000"

run-debug: build/flash.bin build/card.sd | builddir
	${QEMU} -S -s -M versatilepb -cpu arm1176 -sd card.sd -m 128M -nographic -kernel build/flash.bin -append "-load 0x410000 0x14000"

gdb: | builddir
	${GDB} -ex "target remote :1234" build/kernel.elf

builddir:
	mkdir -p build

#boots the kernel at 0x210000
build/flash.bin: build/kernel.img build/script.img | builddir
	dd if=/dev/zero of=$@ bs=4k count=1536
	dd if=../u-boot/u-boot-$(UBOOT_VERSION)/u-boot.bin of=build/flash.bin conv=notrunc bs=4k
	dd if=build/kernel.img of=$@ conv=notrunc bs=4k seek=512
	dd if=build/script.img of=$@ conv=notrunc bs=4k seek=575

# 32678*4096 = 128MiB
build/card.sd: fs-cmdline userhello | builddir
	dd if=/dev/zero of=$@ conv=notrunc bs=4096 count=32768
	fs/cmdline/buildfs ../user/hello/

userhello:
	make -C ../user/hello

fs-cmdline:
	make -C fs/cmdline

build/kernel.elf: $(OBJS) | builddir
	$(LD) -T kernel.ld -nostartfiles -Wl,-Map,kernel.map $(OBJS) -o $@

build/kernel.bin: build/kernel.elf | builddir
	$(OBJCOPY) -O binary $< $@

build/kernel.img: build/kernel.bin | builddir
	$(MKIMAGE) -A arm -C none -O linux -T kernel -d $< -a 0x00010000 -e 0x00010000 $@

# Begin Pi Make
build/kernelPi.elf: $(PIOBJS) | builddir
	$(CC) -T kernelPi.ld -O2 $(PFLAGS) $(PIOBJS) -o $@

build/kernelPi.img: build/kernelPi.elf | builddir
	$(OBJCOPY) $< -O binary $@

# End Pi Make

build/script.img: uboot_configuration/uboot-commands.ubt | builddir
	$(MKIMAGE) -A arm -C none -T script -d $< -a 0x10000 -e 0x10000 $@

build/course_os.img: build/kernel.img | builddir
	cat ../u-boot/u-boot-$(UBOOT_VERSION)/u-boot.bin $< > $@

build/startup.o: startup.s | builddir
	@mkdir -p $(shell dirname $@)
	$(AS) -mcpu=arm1176jzf-s -g $< -o $@

build/%.o: %.c | builddir
	@mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

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
	rm -rf build
	@echo "I'm persisting the SD card 'card.sd', you'll have to remove that yourself if you want to rebuild it."
	@echo "('rm sd.card')"
