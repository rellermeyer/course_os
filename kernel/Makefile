include $(CURDIR)/../config.mk

# ===================== Configuration =====================

# Flags to give to the c compiler
CFLAGS += -pipe -std=gnu99 -ffreestanding -nostdinc -Wall -Werror -g
PI_CFLAGS = -mfpu=vfp -march=armv6zk -mtune=arm1176jzf-s -nostartfiles

# variables to define in the preprocessor.
MEMORY = 128M
DEFINITIONS =
test: DEFINITIONS += ENABLE_TESTS # if we execute the test: rule, enable tests before recompiling
KERNEL_PARAMS = root=/dev/ram mem=$(MEMORY)
SOURCEDIR = src
BUILDDIR = build
# every directory named `include` will have it's contents autoincluded
INCLUDEDIR = include

# =================== End Configuration ===================

TOOLCHAIN_PATH:=$(CURDIR)/../$(TOOLCHAIN_DIR)/$(BARE_METAL_TARGET)/bin
CC:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
AS:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-as
LD:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
OBJCOPY:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-objcopy
MKIMAGE:=$(CURDIR)/../u-boot/u-boot-$(UBOOT_VERSION)/tools/mkimage
GDB:=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gdb

DIRS = $(shell find $(SOURCEDIR)/ -type d -print)
C_SOURCE_FILES := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
C_OBJECT_FILES := $(patsubst $(SOURCEDIR)/%.c, \
    $(BUILDDIR)/%.o, $(C_SOURCE_FILES))

S_SOURCE_FILES := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.s))
S_OBJECT_FILES := $(patsubst $(SOURCEDIR)/%.s, \
    $(BUILDDIR)/%.o, $(S_SOURCE_FILES))

OBJECT_FILES := $(S_OBJECT_FILES) $(C_OBJECT_FILES)

INCLUDEDIRS :=  $(sort $(foreach dir, $(foreach dir1, $(DIRS), $(shell dirname $(dir1))), $(wildcard $(dir)/$(INCLUDEDIR))))
CFLAGS += $(foreach dir, $(INCLUDEDIRS), -I./$(dir))
CFLAGS += $(foreach def, $(DEFINITIONS), -D$(def))

test: build | builddir
	${QEMU} -M versatilepb -cpu arm1176 -sd $(BUILDDIR)/card.sd -m $(MEMORY) -nographic -kernel build/flash.bin -append "-load 0x410000 0x14000" > test.log & echo $$! > server.PID;
	# montior the output, save exit code, remove logfile, kill qemu,
	# remove pid file used to kill qemu, exit with correct exit code
	$(SOURCEDIR)/test/monitor_tests.sh;e=$$?; rm test.log; kill $$(cat server.PID); rm server.PID; exit $$e


run: build | builddir
	${QEMU} -M versatilepb -cpu arm1176 -sd $(BUILDDIR)/card.sd -m $(MEMORY) -nographic -kernel build/flash.bin -append "-load 0x410000 0x14000"

build: $(BUILDDIR)/card.sd $(BUILDDIR)/flash.bin | builddir
build_pi: $(BUILDDIR)/kernelPi.img | builddir

$(BUILDDIR)/flash.bin: $(BUILDDIR)/kernel.img $(BUILDDIR)/bootloader.img
	dd if=/dev/zero of=$@ bs=4k count=1536
	dd if=../u-boot/u-boot-$(UBOOT_VERSION)/u-boot.bin of=$@ conv=notrunc bs=4k
	# kernel
	dd if=$(BUILDDIR)/kernel.img of=$@ conv=notrunc bs=4k seek=512
	# bootloader
	dd if=$(BUILDDIR)/bootloader.img of=$@ conv=notrunc bs=4k seek=575

$(BUILDDIR)/card.sd: | builddir
	dd if=/dev/zero of=$@ conv=notrunc bs=4096 count=32768

$(BUILDDIR)/kernel.elf: $(OBJECT_FILES) | builddir
	$(LD) -T linker/kernel.ld -nostartfiles -Wl,-Map,kernel.map $(OBJECT_FILES) -o $@

$(BUILDDIR)/kernel.bin: $(BUILDDIR)/kernel.elf | builddir
	$(OBJCOPY) -O binary $< $@

$(BUILDDIR)/kernel.img: $(BUILDDIR)/kernel.bin | builddir
	$(MKIMAGE) -A arm -C none -O linux -T kernel -d $< -a 0x00010000 -e 0x00010000 $@

# Begin bootloader Make
uboot_configuration/uboot-commands.ubt: $(and $(KERNEL_PARAMS),dummy)
	uboot_configuration/generate.sh $(KERNEL_PARAMS)

$(BUILDDIR)/bootloader.img: uboot_configuration/uboot-commands.ubt | builddir
	$(MKIMAGE) -A arm -C none -T script -d $< -a 0x10000 -e 0x10000 $@
# End bootloader make

# Begin Pi Make
$(BUILDDIR)/kernelPi.elf: $(C_OBJECT_FILES) | builddir
	$(CC) -T kernelPi.ld -O2 $(PI_CFLAGS) $(C_OBJECT_FILES) -o $@

$(BUILDDIR)/kernelPi.img: $(BUILDDIR)/kernelPi.elf | builddir
	$(OBJCOPY) $< -O binary $@
# End Pi Make

builddir:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.s | builddir
	@mkdir -p $(shell dirname $@)
	@echo Assembling $<
	@$(AS) -mcpu=arm1176jzf-s -g $< -o $@

$(BUILDDIR)/test/test.o: $(SOURCEDIR)/test/test.c dummy | builddir
	@$(SOURCEDIR)/test/generate_tests.sh
	@mkdir -p $(shell dirname $@)
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@


# depend on dummy to always recompile. There aren't that many files atm anyway and
# when the definitions change, we have to recompile. TODO: move definitions to some kind of file
# so make knows that when it changed it should recompile.
$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c dummy | builddir
	@mkdir -p $(shell dirname $@)
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)

# use to force rebuild of certain recipes
dummy:;