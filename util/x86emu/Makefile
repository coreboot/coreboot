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

X86EMU_SRC  = debug.c decode.c fpu.c ops.c ops2.c prim_ops.c sys.c
BIOSEMU_SRC = biosemu.c pcbios/pcibios.c
VM86_SRC    = vm86.c vm86_gdt.c 

ifeq ($(CONFIG_PCI_OPTION_ROM_RUN_X86EMU),y)
LIBX86EMU_SRC=$(X86EMU_SRC) $(BIOSEMU_SRC)
endif

ifeq ($(CONFIG_PCI_OPTION_ROM_RUN_VM86),y)
LIBX86EMU_SRC=$(VM86_SRC)
endif

LIBX86EMU_OBJS = $(patsubst %.c,$(obj)/util/x86emu/%.o,$(LIBX86EMU_SRC))
# needed for kscope
PCIROM_SRC += $(patsubst %,$(src)/util/x86emu/%,$(LIBX86EMU_SRC)) 


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

