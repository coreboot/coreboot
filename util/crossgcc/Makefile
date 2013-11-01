all: build-i386 build-armv7a

build-i386:
	bash ./buildgcc -G -p i386-elf

build-armv7a:
	bash ./buildgcc -G -p armv7a-eabi

.PHONY: build-i386-without-gdb
build-i386-without-gdb:
	bash ./buildgcc -p i386-elf

.PHONY: build-armv7a-without-gdb
build-armv7a-without-gdb:
	bash ./buildgcc -p armv7a-eabi

clean:
	rm -rf xgcc

.PHONY: all build clean
