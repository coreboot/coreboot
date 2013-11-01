all: buildi386 buildarm

buildi386:
	bash ./buildgcc -G -p i386-elf

buildarm:
	bash ./buildgcc -G -p armv7a-eabi

.PHONY: buildi386-without-gdb
buildi386-without-gdb:
	bash ./buildgcc -p i386-elf

.PHONY: buildarm-without-gdb
buildarm-without-gdb:
	bash ./buildgcc -p armv7a-eabi

clean:
	rm -rf xgcc

.PHONY: all build clean
