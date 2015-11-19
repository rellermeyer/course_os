include $(CURDIR)/../config.mk

UBOOT_DIR:=u-boot-$(UBOOT_VERSION)
UBOOT_ARCHIVE:=$(UBOOT_DIR).tar.bz2
UBOOT_URL:=ftp://ftp.denx.de/pub/u-boot/$(UBOOT_ARCHIVE)

export PATH:=$(CURDIR)/../$(TOOLCHAIN_DIR)/$(BARE_METAL_TARGET)/bin:$(PATH)

all: $(UBOOT_DIR)/u-boot.bin

run: $(UBOOT_DIR)/u-boot.bin
	${QEMU} -M versatilepb -m 128M -nographic -kernel $(UBOOT_DIR)/u-boot.bin

$(UBOOT_DIR)/u-boot.bin: $(UBOOT_DIR)
	-cd $(UBOOT_DIR); patch --silent --forward -p1 < ../versatile.patch
	cd $(UBOOT_DIR); make versatileqemu_config ARCH=arm CROSS_COMPILE=$(BARE_METAL_TUPLE)-
	cd $(UBOOT_DIR); make all ARCH=arm CROSS_COMPILE=$(BARE_METAL_TUPLE)-	

$(UBOOT_DIR): $(UBOOT_ARCHIVE)
	tar xvf $(UBOOT_ARCHIVE)

$(UBOOT_ARCHIVE):
	wget $(UBOOT_URL)
	 
clean:
	rm -rf $(UBOOT_DIR)

