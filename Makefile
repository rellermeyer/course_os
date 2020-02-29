all: toolchain kernel

toolchain:
	cd ./toolchain && ./build.sh
.PHONY: toolchain

libc:
	$(MAKE) -C user/libc

kernel: libc
	$(MAKE) -C kernel
	$(MAKE) -C user/hello
.PHONY: kernel

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C user/libc clean
	$(MAKE) -C user/hello clean
.PHONY: clean

build:
	@$(MAKE) -C ./kernel build

run:
	@$(MAKE) -C ./kernel run

test:
	@$(MAKE) -C ./kernel test

docs:
	doxygen doxyfile
	$(MAKE) -C latex pdf
	mv ./latex/refman.pdf course_os_docs.pdf
