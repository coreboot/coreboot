#
# NOTE: You need to add your libpci.a version to CFLAGS below if
# pci-userspace.c does not build.
#
# If you are building on AMD64, you have to use /usr/lib64/libpci.a instead of
# /usr/lib/...
#

CC       =  gcc
CFLAGS   =  -Wall -Ix86emu/include -O2 -g \
		-DLIBPCI_MAJOR_VERSION=2  \
		-DLIBPCI_MINOR_VERSION=1  \
		-DLIBPCI_MICRO_VERSION=11


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

x86emu/src/x86emu/libx86emu.a:
	$(MAKE) -C x86emu/src/x86emu/ -f makefile.linux

clean:
	$(MAKE) -C x86emu/src/x86emu/ -f makefile.linux clean
	rm -f *.o *~ testbios

distclean: clean
	$(MAKE) -C x86emu/src/x86emu/ -f makefile.linux clean
