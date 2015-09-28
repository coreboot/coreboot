#
# NOTE: You need to add your libpci.a version to CFLAGS below if
# pci-userspace.c does not build.
#
# If you are building on AMD64, you have to use /usr/lib64/libpci.a instead of
# /usr/lib/...
#

TOP     ?= ../..

CC      ?=  gcc
CFLAGS  ?= -O2 -g -fomit-frame-pointer

CFLAGS  += -Wall -Wundef -Wstrict-prototypes -Wmissing-prototypes
CFLAGS  += -Wwrite-strings -Wredundant-decls -Wstrict-aliasing -Wshadow -Wextra

# TODO check host architecture
CBCFLAGS  = -DCONFIG_ARCH_X86=1 -Wno-sign-compare -Wno-unused-but-set-variable -Wno-unused-parameter

INCLUDES  = -Iinclude -I$(TOP)/src/device/oprom/include/
CBINCLUDES = -I$(TOP)/src --include include/stdtypes.h
CBINCLUDES += --include $(TOP)/src/commonlib/include/commonlib/loglevel.h
CBINCLUDES += -include stdio.h

SOURCE  = int10.c int15.c int16.c int1a.c inte6.c testbios.c
SOURCE += helper_exec.c helper_mem.c pci-userspace.c

X86EMU  = sys.c decode.c ops.c ops2.c prim_ops.c fpu.c debug.c
X86EMU_DIR = $(TOP)/src/device/oprom/x86emu
X86EMU_SOURCE = $(addprefix $(X86EMU_DIR)/, $(X86EMU))
OBJECTS:=$(SOURCE:.c=.o) $(X86EMU:.c=.o)

LIBS=-lpci

all: dep testbios

testbios: $(OBJECTS)
	printf "    LINK  $(notdir $@)\n"
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

dep: $(SOURCE) $(X86EMU_SOURCE) Makefile
	$(CC) $(CFLAGS) $(INCLUDES) -MM $(SOURCE) > .dependencies
	$(CC) $(CFLAGS) $(INCLUDES) $(CBCFLAGS) $(CBINCLUDES) -MM $(X86EMU_SOURCE) >> .dependencies

clean:
	rm -f *.o *~ testbios

distclean: clean
	rm -f .dependencies

%.o: $(X86EMU_DIR)/%.c
	printf "    CC    (x86emu) $(notdir $<)\n"
	$(CC) $(CFLAGS) $(CBCFLAGS) $(INCLUDES) $(CBINCLUDES) -c -o $@ $<

%.o: %.c
	printf "    CC    $(notdir $<)\n"
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

.PHONY: all clean distclean
.SILENT:

-include .dependencies
