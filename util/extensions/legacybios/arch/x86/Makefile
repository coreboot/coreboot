# Copyright (C) 2003 Stefan Reinauer
#
# See the file "COPYING" for further information about
# the copyright and warranty status of this work.
#

VPATH      := $(TOPDIR)/include:$(BUILDDIR):$(TOPDIR)/arch/$(ARCH)/include

BINARIES   := legacybios.multiboot legacybios legacybios.full


all: core link $(BINARIES)

LDFLAGS_OPENBIOS = -nostdlib -Wl,-N -Wl,-Ttext -Wl,100000

OBJS_GENERIC   =  lib.o legacybios.o console.o pcibios.o malloc.o pci.o libx86emu.a
OBJS_MULTIBOOT =  mboot.o multiboot.o
OBJS_PLAIN     =  boot.o plainboot.o
OBJS_BUILTIN   =  boot.o builtin.o

INCLUDES := -Iinclude -I$(TOPDIR)/include -I$(BUILDDIR)

include $(TOPDIR)/Rules.make

core:
	echo -e "\nCompiling $(ARCH) architecture specific binaries"

link: $(OBJS_MULTIBOOT) $(OBJS_PLAIN) $(OBJS_BUILTIN) $(OBJS_GENERIC)
	cd $(BUILDDIR) && if [ ! -r legacybios.multiboot -o  ! -r legacybios \
		-o ! -r legacybios.full ]; then echo -e "\nLinking:"; fi

legacybios.multiboot: $(OBJS_MULTIBOOT) $(OBJS_GENERIC)
	echo -ne "  multiboot image for grub... "
	cd $(BUILDDIR) && $(CC) $(LDFLAGS_OPENBIOS) -o $@.nostrip $^ -lgcc
	echo -e "\tok"
	cd $(BUILDDIR) && objcopy -O binary -R .note -R .comment $@.nostrip $@
  
legacybios: $(OBJS_PLAIN) $(OBJS_GENERIC)
	echo -n "  elf image (for LinuxBIOS)... "
	cd $(BUILDDIR) && $(CC)  $(LDFLAGS_OPENBIOS) -o $@.nostrip $^ -lgcc
	echo -e "\tok"
	cd $(BUILDDIR) && objcopy -R .note -R .comment $@.nostrip $@ && strip -s $@


rom.h: vgabios.rom
	echo -n "  generating linkable rom..."
	cd $(BUILDDIR) && ./bin2hex < $^ > $@
	echo -e "\tok"

legacybios.full: $(OBJS_BUILTIN) $(OBJS_GENERIC)
	echo -n "  elf image with rom... "
	cd $(BUILDDIR) && $(CC) $(LDFLAGS_OPENBIOS) -o $@.nostrip $(OBJS_BUILTIN) $(OBJS_GENERIC) -lgcc
	echo -e "\tok"
	cd $(BUILDDIR) && objcopy -R .note -R .comment $@.nostrip $@ && strip -s $@


# dependencies. these are so small that we write them manually.

multiboot.o: multiboot.c types.h config.h multiboot.h
boot.o: boot.S multiboot.h
mboot.o: mboot.S multiboot.h
console.o: console.c types.h config.h
plainboot.o: types.h config.h
builtin.o: rom.h
