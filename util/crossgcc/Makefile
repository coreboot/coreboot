all: build

build:
	bash ./buildgcc -G -p i386-elf
	bash ./buildgcc -G -p armv7a-eabi

.PHONY: build-without-gdb
build-without-gdb:
	bash ./buildgcc -p i386-elf
	bash ./buildgcc -p armv7a-eabi

clean:
	rm -rf xgcc

.PHONY: all build clean
