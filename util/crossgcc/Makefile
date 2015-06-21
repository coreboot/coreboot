all: build-i386 build-x64 build-armv7a build-aarch64 build-mips build-riscv

build-i386:
	bash ./buildgcc -G -p i386-elf $(if $(BUILDJOBS),-j $(BUILDJOBS))

build-x64:
	bash ./buildgcc -G -p x86_64-elf $(if $(BUILDJOBS),-j $(BUILDJOBS))

build-armv7a:
	bash ./buildgcc -G -p armv7a-eabi $(if $(BUILDJOBS),-j $(BUILDJOBS))

build-aarch64:
	bash ./buildgcc -G -p aarch64-elf $(if $(BUILDJOBS),-j $(BUILDJOBS))

build-mips:
	bash ./buildgcc -G -p mipsel-elf $(if $(BUILDJOBS),-j $(BUILDJOBS))

build-riscv:
	bash ./buildgcc -G -p riscv-elf $(if $(BUILDJOBS),-j $(BUILDJOBS))

.PHONY: build-i386-without-gdb
build-i386-without-gdb:
	bash ./buildgcc -p i386-elf $(if $(BUILDJOBS),-j $(BUILDJOBS))

.PHONY: build-x64-without-gdb
build-x64-without-gdb:
	bash ./buildgcc -p x86_64-elf $(if $(BUILDJOBS),-j $(BUILDJOBS))

.PHONY: build-armv7a-without-gdb
build-armv7a-without-gdb:
	bash ./buildgcc -p armv7a-eabi $(if $(BUILDJOBS),-j $(BUILDJOBS))

.PHONY: build-aarch64-without-gdb
build-aarch64-without-gdb:
	bash ./buildgcc -p aarch64-elf $(if $(BUILDJOBS),-j $(BUILDJOBS))

.PHONY: build-mips-without-gdb
build-mips-without-gdb:
	bash ./buildgcc -p mipsel-elf $(if $(BUILDJOBS),-j $(BUILDJOBS))

.PHONY: build-riscv-without-gdb
build-riscv-without-gdb:
	bash ./buildgcc -p riscv-elf $(if $(BUILDJOBS),-j $(BUILDJOBS))

clean:
	rm -rf xgcc
	rm -rf build-*
	rm -rf binutils-* gcc-* gmp-* libelf-* mpc-* mpfr-*
	rm -rf llvm-* clang-tools-* cfe-* compiler-rt-*
	rm -rf acpica-*
	rm -rf gdb-*

distclean: clean
	rm -rf tarballs

.PHONY: all build clean distclean
