CC = gcc
CFLAGS = -I include -O2

DEPS =  bootselect.o rom_fill_inbuf.o chooser.o   lib/baremetal.a

bootselect.elf: $(DEPS)
	ld -defsym HEAPSIZE=0 -T elfImage.lds $(DEPS)  -o bootselect.elf

clean:
	rm bootselect.elf *.o
