CFLAGS   =  -Ix86emu/include -O2 -g

OBJECTS  =  testbios.o helper_exec.o inthandler.o pci-userspace.o
LIBS     =  x86emu/src/x86emu/libx86emu.a
LIBS    +=  /usr/lib/libpci.a

all: testbios

testbios: $(OBJECTS) $(LIBS)
	cc -o testbios $(OBJECTS) $(LIBS)
 
helper_exec.o: helper_exec.c test.h

clean:
	rm -f *.o *~ testbios
