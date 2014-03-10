include $(CURDIR)/../config.mk

TOOLCHAIN_PATH:=$(CURDIR)/../$(TOOLCHAIN_DIR)/$(BARE_METAL_TARGET)/bin
CC:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
AS:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-as
LD:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-ld
OBJCOPY:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-objcopy
MKIMAGE:=$(CURDIR)/../u-boot/u-boot-$(UBOOT_VERSION)/tools/mkimage

OBJS=start.o uart.o startup.o

OFFSET:=0x24DB0
# printf "bootm 0x%X\n" $(expr $(stat -c%s ../u-boot/u-boot-2010.03/u-boot.bin) + 65536)

all: course_os.img

run: course_os.img
	echo $(OFFSET)
	${QEMU} -M versatilepb -m 128M -nographic -kernel course_os.img -append "bootm 0x24DB0"

kernel.elf: $(OBJS)
	$(LD) -T kernel.ld -Map=kernel.map $(OBJS) -o kernel.elf

kernel.bin: kernel.elf
	$(OBJCOPY) -O binary kernel.elf kernel.bin

kernel.img: kernel.bin
	$(MKIMAGE) -A arm -C none -O linux -T kernel -d kernel.bin -a 0x00100000 -e 0x00100000 kernel.img

course_os.img: kernel.img 
	cat ../u-boot/u-boot-$(UBOOT_VERSION)/u-boot.bin kernel.img > course_os.img

startup.o: startup.s
	$(AS) -mcpu=arm1176jzf-s -g startup.s -o startup.o

%.o: %.c
	$(CC) -c -mcpu=arm1176jzf-s $< -o $@

clean: 
	rm -f *.o
	rm -f *.img
	rm -f *.elf
	rm -f *.bin
	rm -f *.map
