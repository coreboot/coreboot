all: build-i386 build-armv7a

build-i386:
	bash ./buildgcc -G -p i386-elf

build-armv7a:
	bash ./buildgcc -G -p armv7a-eabi

build-aarch64:
	bash ./buildgcc -G -p aarch64-elf

.PHONY: build-i386-without-gdb
build-i386-without-gdb:
	bash ./buildgcc -p i386-elf

.PHONY: build-armv7a-without-gdb
build-armv7a-without-gdb:
	bash ./buildgcc -p armv7a-eabi

.PHONY: build-aarch64-without-gdb
build-aarch64-without-gdb:
	bash ./buildgcc -p aarch64-elf

clean:
	rm -rf xgcc

.PHONY: all build clean
