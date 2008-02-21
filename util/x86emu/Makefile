##
## This file is part of the coreboot project.
##
## Copyright (C) 2007 coresystems GmbH
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

X86EMU_OBJ  = debug.o decode.o fpu.o ops.o ops2.o prim_ops.o sys.o
BIOSEMU_OBJ = biosemu.o pcbios/pcibios.o
VM86_OBJ    = vm86.o vm86_gdt.o

ifeq ($(CONFIG_PCI_OPTION_ROM_RUN_X86EMU),y)
LIBX86EMU_OBJS = $(patsubst %,$(obj)/util/x86emu/x86emu/%,$(X86EMU_OBJ)) \
		 $(patsubst %,$(obj)/util/x86emu/%,$(BIOSEMU_OBJ))
endif

ifeq ($(CONFIG_PCI_OPTION_ROM_RUN_VM86),y)
LIBX86EMU_OBJS := $(patsubst %,$(obj)/util/x86emu/%,$(VM86_OBJ))
endif

$(obj)/util/x86emu/libx86emu.a: $(LIBX86EMU_OBJS) $(src)/.config
	$(Q)printf "  AR      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)rm -f $@ # otherwise we always add to the archive
	$(Q)$(AR) qcs $@ $(LIBX86EMU_OBJS)

#
# This rule is also valid for all subdirectories
#

$(obj)/util/x86emu/%.o: $(src)/util/x86emu/%.c
	$(Q)mkdir -p $(dir $@)
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(INITCFLAGS) -I$(src)/util/x86emu/include -c $< -o $@

