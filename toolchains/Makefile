include $(CURDIR)/../config.mk

CROSSTOOL_VERSION=1.9.3
CROSSTOOL_DIR:=crosstool-ng-$(CROSSTOOL_VERSION)
CROSSTOOL_ARCHIVE:=$(CROSSTOOL_DIR).tar.bz2
CROSSTOOL_URL:=http://crosstool-ng.org/download/crosstool-ng/$(CROSSTOOL_ARCHIVE)

CROSSTOOL_TARGET=crosstool-ng
CROSSTOOL_TARGET_ABS=$(CURDIR)/$(CROSSTOOL_TARGET)

EABI_TARGET_ABS=$(CURDIR)/$(EABI_TARGET)

all: toolchain_eabi

toolchain_eabi: $(CROSSTOOL_TARGET)
	cd $(CROSSTOOL_TARGET); bin/ct-ng arm-unknown-eabi
	sed -i 's/CT_MPFR_VERSION="2.4.2"/CT_MPFR_VERSION="3.0.0"/g' $(CROSSTOOL_TARGET)/.config
	sed -i 's/CT_CC_LANG_CXX=y/# CT_CC_LANG_CXX is not set/g' $(CROSSTOOL_TARGET)/.config
	sed -i 's~CT_PREFIX_DIR="$${HOME}/x-tools/$${CT_TARGET}"~CT_PREFIX_DIR="$(EABI_TARGET_ABS)"~g' $(CROSSTOOL_TARGET)/.config
	#sed -i 's~CT_WORK_DIR="$${CT_TOP_DIR}/targets"~CT_WORK_DIR="/tmp/ct-ng/targets"~g' $(CROSSTOOL_TARGET)/.config
	cd $(CROSSTOOL_TARGET); bin/ct-ng -j build	
	rm -rf /local/ct-ng/targets

$(CROSSTOOL_TARGET): $(CROSSTOOL_DIR)
	echo $(CROSSTOOL_TARGET_ABS)
	cd $(CROSSTOOL_DIR); ./configure --prefix $(CROSSTOOL_TARGET_ABS)
	cd $(CROSSTOOL_DIR); make && make install	 
	rm -f $(CROSSTOOL_ARCHIVE)

$(CROSSTOOL_ARCHIVE): 
	wget $(CROSSTOOL_URL)


$(CROSSTOOL_DIR): $(CROSSTOOL_ARCHIVE)
	tar -xvf $(CROSSTOOL_ARCHIVE)

clean: 
	rm -f $(CROSSTOOL_ARCHIVE)
	rm -rf $(CROSSTOOL_DIR)
	rm -rf $(CROSSTOOL_TARGET)
