CC       =  gcc
CFLAGS   =  -Wall -Ix86emu/include -O2 -g

INTOBJS  =  int10.o int15.o int16.o int1a.o inte6.o
OBJECTS  =  testbios.o helper_exec.o helper_mem.o $(INTOBJS)

LIBS     =  x86emu/src/x86emu/libx86emu.a

# user space pci is the only option right now.
OBJECTS += pci-userspace.o
LIBS    +=  /usr/lib/libpci.a

all: testbios

testbios: $(OBJECTS) $(LIBS)
	$(CC) -o testbios $(OBJECTS) $(LIBS)
 
helper_exec.o: helper_exec.c test.h

clean:
	rm -f *.o *~ testbios
