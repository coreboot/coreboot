CC = gcc
CFLAGS = -I ../include -O2

DEPS =  ../lib/_main.o hello.o ../lib/baremetal.a

hello.elf: $(DEPS)
	ld -defsym HEAPSIZE=0x100 -T elfImage.lds $(DEPS)  -o hello.elf

clean:
	rm hello.elf *.o
