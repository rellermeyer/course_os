include $(CURDIR)/../config.mk
.PHONY: all
all: configure_toolchain
# NOTE: things with make break if there are folder names in the path from
# root to the directory with course_os in it which contain spaces.

# ===================== Configuration =====================

# TODO should we have "-nostdinc"?
# Flags to give to the c compiler
CFLAGS += -pipe -std=gnu99 -ffreestanding \
    -Wall -Werror -Wno-error=unused-function -Wno-error=unused-label -Wno-error=unused-parameter -Wno-error=unused-variable -Wno-error=unused-value -Wno-error=unused-local-typedefs \
    -g -O0 -mtune=arm1176jzf-s -march=armv6zk -mfpu=vfp -fpic
LDFLAGS += -nostartfiles -fcommon -nolibc
# C flags if building for the real pi. This is mostly untested
# PI_CFLAGS = -mfpu=vfp -march=armv6zk -mtune=arm1176jzf-s -nostartfiles

# variables to define in the preprocessor.
# amount of memory the PMM should try to allocate, may or may not match the actual amount of
# memory in the system. Stuff will crash if this number is more than the actual amount of
# memory. TODO: autodetect this at boot
MEMORY = 1G
LOG_LEVEL ?= 4

# Available options:
# * Any custom definition you'd like to have enabled
# * MEM_DEBUG - Compiles in part of the code that will print debug information about memory management
# * LOG_LEVEL (number between 0 and 4)
DEFINITIONS = MEM_DEBUG LOG_LEVEL=${LOG_LEVEL}

test: DEFINITIONS += ENABLE_TESTS # if we execute the test: rule, enable tests before recompiling
SOURCEDIR = src
BUILDDIR = build
# every directory named `include` will have its contents autoincluded
INCLUDEDIR = include
CPU = cortex-a7

# Seeds the random order in which tests are run.
TESTS_SEED = $(shell date '+%s')

# =================== End Configuration ===================
TOOLCHAIN_PATH=$(CURDIR)/../$(TOOLCHAIN_DIR)/$(BARE_METAL_TARGET)/bin
CC=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
AS=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-as
LD=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gcc
OBJCOPY=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-objcopy
GDB=$(TOOLCHAIN_PATH)/$(BARE_METAL_TUPLE)-gdb
QEMU ?= $(CURDIR)/../qemu/qemu/bin/qemu-system-arm

DIRS = $(shell find $(SOURCEDIR)/ -type d -print)
C_GEN_SOURCE_FILES := $(SOURCEDIR)/test/test.c
C_SOURCE_FILES := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c)) $(C_GEN_SOURCE_FILES)
C_OBJECT_FILES := $(patsubst $(SOURCEDIR)/%.c, \
    $(BUILDDIR)/%.o, $(C_SOURCE_FILES))

S_SOURCE_FILES := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.s))
S_OBJECT_FILES := $(patsubst $(SOURCEDIR)/%.s, \
    $(BUILDDIR)/%.o, $(S_SOURCE_FILES))

MITKO_FILE := src/loader/simple_program.o

OBJECT_FILES := $(S_OBJECT_FILES) $(C_OBJECT_FILES)

INCLUDEDIRS := $(sort $(foreach dir, $(foreach dir1, $(DIRS), $(shell dirname $(dir1))), $(wildcard $(dir)/$(INCLUDEDIR))))
CFLAGS += $(foreach dir, $(INCLUDEDIRS), -I./$(dir))
CFLAGS += $(foreach def, $(DEFINITIONS), -D$(def))

.PHONY: configure_toolchain
configure_toolchain:
ifneq (, $(shell which "${BARE_METAL_TUPLE}-gcc" 2> /dev/null))
CC := $(shell which ${BARE_METAL_TUPLE}-gcc)
AS := $(shell which ${BARE_METAL_TUPLE}-as)
LD := $(shell which ${BARE_METAL_TUPLE}-gcc)
GDB := $(shell which ${BARE_METAL_TUPLE}-gdb)
OBJCOPY := $(shell which ${BARE_METAL_TUPLE}-objcopy)
else
ifeq ("$(wildcard $(CC))","")
	@echo "Installing toolchain"
	cd "${CURDIR}/../toolchain"; sh build.sh
endif
endif

.PHONY: configure_qemu
configure_qemu: 
ifneq (, $(shell which qemu-system-arm 2> /dev/null))
QEMU := $(shell which qemu-system-arm 2> /dev/null)
else 
ifeq ("$(wildcard $(QEMU))", "")
	@echo "Installing qemu"
	cd ${CURDIR}/../qemu; ./build.sh
endif
endif

.PHONY: configure
configure: configure_qemu configure_toolchain
	@echo "Configuring tools"
	

build:  $(BUILDDIR)/kernel.elf configure | builddir
build_pi: $(BUILDDIR)/kernelPi.img | builddir

test: build configure | builddir
	#${QEMU} -M versatilepb -cpu arm1176 -sd $(BUILDDIR)/card.sd -m $(MEMORY) -nographic -semihosting -kernel build/flash.bin -append "-load 0x410000 0x14000"
	${QEMU} -kernel $(BUILDDIR)/kernel.elf -m $(MEMORY) -serial stdio -monitor none -M raspi2 -cpu $(CPU) -nographic -append "-load 0x410000 0x14000" -semihosting

run: build configure | builddir
	# nographic to turn off the gui
	# monitor none to disable stdio monitoring so
	# serial stdio works
	#${QEMU} -M versatilepb -cpu arm1176 -sd $(BUILDDIR)/card.sd -m $(MEMORY) -nographic -semihosting -monitor none -serial stdio -kernel build/flash.bin -append "-load 0x410000 0x14000"
	${QEMU} -kernel $(BUILDDIR)/kernel.elf -m $(MEMORY) -serial stdio -monitor none -M raspi2 -cpu $(CPU) -nographic -append "-load 0x410000 0x14000" -semihosting

debug: build configure | builddir
	${QEMU} -kernel $(BUILDDIR)/kernel.elf -m $(MEMORY) -serial stdio -monitor none -M  raspi2 -cpu $(CPU) -nographic -append "-load 0x410000 0x14000" -semihosting -S -s

start_debug: build  configure | builddir
	$(GDB) -ex "target remote localhost:1234" -ex "symbol-file $(BUILDDIR)/kernel.sym"

$(BUILDDIR)/kernel.elf: $(OBJECT_FILES) $(MITKO_FILE) | builddir
	$(LD) -T linker/kernel.ld $(LDFLAGS) -Wl,-Map,kernel.map $^ -o $@
	$(OBJCOPY) --only-keep-debug $(BUILDDIR)/kernel.elf $(BUILDDIR)/kernel.sym
	$(OBJCOPY) --strip-debug $(BUILDDIR)/kernel.elf
	@echo "Mitko now puts his section ..."

# Begin Pi Make
# (aka, building for a real pi. TODO: This is mostly untested)
$(BUILDDIR)/kernelPi.elf: $(C_OBJECT_FILES) | builddir
	$(CC) -T kernelPi.ld -O2 $(PI_CFLAGS) $^ -o $@

$(BUILDDIR)/kernelPi.img: $(BUILDDIR)/kernelPi.elf | builddir
	$(OBJCOPY) $< -O binary $@
# End Pi Make

builddir:
	@mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.s | builddir
	@mkdir -p $(shell dirname $@)
	@echo Assembling $<
	@$(AS) -mcpu=arm1176jzf-s -g $< -o $@

$(SOURCEDIR)/test/test.c: dummy | builddir
	@echo Generating tests
	@$(SOURCEDIR)/test/generate_tests.sh $(TESTS_SEED)

# depend on dummy to always recompile. There aren't that many files atm anyway and
# when the definitions change, we have to recompile. TODO: move definitions to some kind of file
# so make knows that when it changed it should recompile.
$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c dummy configure_toolchain | builddir
	@mkdir -p $(shell dirname $@)
	@echo Compiling $<
	@echo ${CC}
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)
	rm -rf $(SOURCEDIR)/test/test.c

# use to force rebuild of certain recipes
dummy:;
