#
# NOTE: You need to add your libpci.a version to CFLAGS below if
# pci-userspace.c does not build.
#
# If you are building on AMD64, you have to use /usr/lib64/libpci.a instead of
# /usr/lib/...
#

CC       =  gcc
CFLAGS   = -O2 -g -fomit-frame-pointer
CFLAGS  += -Wall -Wundef -Wstrict-prototypes -Wmissing-prototypes
CFLAGS  += -Wwrite-strings -Wredundant-decls -Wno-trigraphs
CFLAGS  += -Wstrict-aliasing -Wshadow -Wextra

INCLUDES = -Iinclude -I../../src/device/oprom/include/

INTOBJS  = int10.o int15.o int16.o int1a.o inte6.o
X86EMUOBJS  = sys.o decode.o ops.o ops2.o prim_ops.o fpu.o debug.o
OBJS  =  testbios.o helper_exec.o helper_mem.o $(INTOBJS) $(X86EMUOBJS)

# user space pci is the only option right now.
OBJS += pci-userspace.o

LIBS=-lpci

all: testbios

testbios: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

helper_exec.o: helper_exec.c test.h

clean:
	rm -f *.o *~ testbios

%.o: ../../src/devices/oprom/x86emu/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -include stdio.h -c -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

