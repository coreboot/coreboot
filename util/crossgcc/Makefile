all: build-i386 build-x64 build-armv7a build-aarch64 build-mips build-riscv

build-i386:
	bash ./buildgcc -G -p i386-elf

build-x64:
	bash ./buildgcc -G -p x86_64-elf

build-armv7a:
	bash ./buildgcc -G -p armv7a-eabi

build-aarch64:
	bash ./buildgcc -G -p aarch64-elf

build-mips:
	bash ./buildgcc -G -p mipsel-elf

build-riscv:
	bash ./buildgcc -G -p riscv-elf

.PHONY: build-i386-without-gdb
build-i386-without-gdb:
	bash ./buildgcc -p i386-elf

.PHONY: build-x64-without-gdb
build-x64-without-gdb:
	bash ./buildgcc -p x86_64-elf

.PHONY: build-armv7a-without-gdb
build-armv7a-without-gdb:
	bash ./buildgcc -p armv7a-eabi

.PHONY: build-aarch64-without-gdb
build-aarch64-without-gdb:
	bash ./buildgcc -p aarch64-elf

.PHONY: build-mips-without-gdb
build-mips-without-gdb:
	bash ./buildgcc -p mipsel-elf

.PHONY: build-riscv-without-gdb
build-riscv-without-gdb:
	bash ./buildgcc -p riscv-elf

clean:
	rm -rf xgcc

.PHONY: all build clean
