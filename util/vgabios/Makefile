##
## SPDX-License-Identifier: GPL-2.0-only

# NOTE: You need to add your libpci.a version to CFLAGS below if
# pci-userspace.c does not build.
#
# If you are building on AMD64, you have to use /usr/lib64/libpci.a instead of
# /usr/lib/...
#

TOP     ?= ../..
OUT     ?= build

CC      ?=  gcc
CFLAGS  ?= -O2 -g -fomit-frame-pointer

CFLAGS  += -Wall -Wundef -Wstrict-prototypes -Wmissing-prototypes
CFLAGS  += -Wwrite-strings -Wredundant-decls -Wstrict-aliasing -Wshadow -Wextra
CFLAGS  += -Wno-unused-but-set-variable

# TODO check host architecture
CBCFLAGS  = -Wno-sign-compare -Wno-unused-parameter -Wno-format

INCLUDES  = -Iinclude -I$(OUT)/include
INCLUDES += -I$(TOP)/src/device/oprom/include/
INCLUDES += -I$(TOP)/src/device/oprom/yabel
INCLUDES += -include $(TOP)/src/commonlib/include/commonlib/loglevel.h
INCLUDES += -include stdtypes.h -include pci-userspace.h
INCLUDES += -include $(TOP)/src/include/kconfig.h

CBINCLUDES = -I$(TOP)/src -include include/stdtypes.h -include $(TOP)/src/include/endian.h
CBINCLUDES += -include stdio.h -include sys/io.h

SOURCE  = testbios.c
SOURCE += pci-userspace.c
SOURCE += device.c

X86EMU  = x86emu/sys.c x86emu/decode.c x86emu/ops.c x86emu/ops2.c
X86EMU += x86emu/prim_ops.c x86emu/fpu.c x86emu/debug.c

X86EMU += yabel/interrupt.c
X86EMU += yabel/mem.c
X86EMU += yabel/io.c
X86EMU += yabel/pmm.c
X86EMU += yabel/biosemu.c
X86EMU += yabel/debug.c
#X86EMU += yabel/device.c # For now we need a local copy :-(



X86EMU_DIR = $(TOP)/src/device/oprom
X86EMU_SOURCE = $(addprefix $(X86EMU_DIR)/, $(X86EMU))
OBJECTS:=$(addprefix $(OUT)/,$(SOURCE:.c=.o)) $(addprefix $(OUT)/, $(X86EMU:.c=.o))

LIBS=-lpci

all: dep testbios

$(OBJECTS): includes

testbios: $(OBJECTS)
	printf "    LINK  $(notdir $@)\n"
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

dep: $(SOURCE) $(X86EMU_SOURCE) Makefile
	$(CC) $(CFLAGS) $(INCLUDES) -MM $(SOURCE) > .dependencies
	$(CC) $(CFLAGS) $(INCLUDES) $(CBCFLAGS) $(CBINCLUDES) -MM $(X86EMU_SOURCE) >> .dependencies

# Make all the dummy include files (that are in reality
# covered by all the -include statements above)
includes:
	mkdir -p $(OUT)/include/device
	mkdir -p $(OUT)/include/arch
	touch $(OUT)/include/device/device.h
	touch $(OUT)/include/device/pci.h
	touch $(OUT)/include/device/pci_ops.h
	touch $(OUT)/include/device/resource.h
	touch $(OUT)/include/arch/io.h
	touch $(OUT)/include/timer.h
	touch $(OUT)/include/types.h

clean:
	rm -f $(OBJECTS) *~ testbios
	rm -rf $(OUT)

distclean: clean
	rm -f .dependencies

$(OUT)/%.o: $(X86EMU_DIR)/%.c
	printf "    CC    (x86emu) $(notdir $<)\n"
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CBCFLAGS) $(INCLUDES) $(CBINCLUDES) -c -o $@ $<

$(OUT)/%.o: %.c
	printf "    CC    $(notdir $<)\n"
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

.PHONY: all includes clean distclean
.SILENT:

-include .dependencies
