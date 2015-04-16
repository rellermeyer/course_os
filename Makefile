
toolchain:
	cd ./toolchain/ && ./build.sh
.PHONY: toolchain
u-boot:
	$(MAKE) -C u-boot
.PHONY: u-boot
kernel:
	$(MAKE) -C kernel
	$(MAKE) -C user/libc
	$(MAKE) -C user/hello
.PHONY: kernel
clean:
	$(MAKE) -C u-boot clean
	$(MAKE) -C kernel clean
	$(MAKE) -C user/libc clean
	$(MAKE) -C user/hello clean
.PHONY: clean
build: u-boot kernel
.PHONY: build
run: 
	@$(MAKE) -C ./kernel run
.PHONY: run

	 