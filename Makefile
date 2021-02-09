include $(CURDIR)/config.mk
all: toolchain build

./toolchain/indicator:
	bash -c "cd ./toolchain && ./build.sh"
	echo "remove this file to force make to rebuild the toolchain" > ./toolchain/indicator

./qemu/indicator:
	cd ./qemu && ./build.sh
	echo "remove this file to force make to rebuild qemu" > ./toolchain/indicator

.PHONY: toolchain
toolchain: ./toolchain/indicator
.PHONY: qemu
qemu: ./qemu/indicator

requirements: toolchain qemu

.PHONY: clean
clean:
	$(MAKE) -C kernel clean
	@rm ./toolchain/indicator
	@rm -rf ./toolchain/arm-none-aebi
	@rm ./qemu/indicator

.PHONY: build
build: toolchain
	@$(MAKE) -C ./kernel build

.PHONY: run
run: toolchain
	@$(MAKE) -C ./kernel run

.PHONY: test
test: toolchain
	@$(MAKE) -C ./kernel test

.PHONY: docs
docs:
	doxygen Doxyfile
	xdg-open docs/index.html
