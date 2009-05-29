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

#X86EMU_INCLUDE = -I $(src)/util/x86emu/include  

X86EMU_SRC  = debug.c decode.c fpu.c ops.c ops2.c prim_ops.c sys.c
ifeq ($(CONFIG_PCI_OPTION_ROM_RUN_X86EMU),y)
BIOSEMU_SRC = biosemu.c pcbios/pcibios.c
endif

ifeq ($(CONFIG_PCI_OPTION_ROM_RUN_YABEL),y)
BIOSEMU_SRC  = biosemu.c debug.c device.c mem.c io.c interrupt.c 
#TODO: add vbe.c, currently not needed...
#BIOSEMU_SRC +=vbe.c
BIOSEMU_SRC +=pmm.c
#PH: TODO: remove the compat files??
BIOSEMU_SRC  += compat/functions.c
X86EMU_INCLUDE += -I $(src)/util/x86emu/yabel
X86EMU_INCLUDE += -I $(src)/util/x86emu
#TODO: remove these, these are .h files from slof, to make the merge easier...
X86EMU_INCLUDE += -I $(src)/util/x86emu/yabel/compat
endif
REALMODE_SRC    = x86.c x86_asm.S

ifeq ($(CONFIG_PCI_OPTION_ROM_RUN_X86EMU),y)
LIBX86EMU_SRC=$(patsubst %,x86emu/%,$(X86EMU_SRC)) $(BIOSEMU_SRC)
endif

ifeq ($(CONFIG_PCI_OPTION_ROM_RUN_YABEL),y)
LIBX86EMU_SRC=$(patsubst %,x86emu/%,$(X86EMU_SRC)) $(patsubst %,yabel/%,$(BIOSEMU_SRC))
endif

ifeq ($(CONFIG_PCI_OPTION_ROM_RUN_REALMODE),y)
LIBX86EMU_SRC=$(REALMODE_SRC)
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
	$(Q)$(CC) -Werror $(INITCFLAGS) $(X86EMU_INCLUDE) -I$(src)/util/x86emu/include -c $< -o $@

