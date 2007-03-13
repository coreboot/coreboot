##
## This file is part of the LinuxBIOS project.
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
PCBIOS_OBJ  = pcibios.o
BIOSEMU_OBJ = biosemu.o vm86.o

LIBX86EMU_OBJS = $(patsubst %,$(obj)/util/x86emu/x86emu/%,$(X86EMU_OBJ)) \
		 $(patsubst %,$(obj)/util/x86emu/pcbios/%,$(PCBIOS_OBJ)) \
		 $(patsubst %,$(obj)/util/x86emu/%,$(BIOSEMU_OBJ))

$(obj)/util/x86emu/libx86emu.a: $(LIBX86EMU_OBJS)
	$(Q)printf "  AR      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(AR) q $@ $^

$(obj)/util/x86emu/x86emu/%.o: $(src)/util/x86emu/x86emu/%.c
	$(Q)mkdir -p $(obj)/util/x86emu/x86emu/
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(INITCFLAGS) -I$(src)/util/x86emu/include -c $< -o $@

$(obj)/util/x86emu/pcbios/%.o: $(src)/util/x86emu/pcbios/%.c
	$(Q)mkdir -p $(obj)/util/x86emu/pcbios
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(INITCFLAGS) -I$(src)/util/x86emu/include -c $< -o $@

$(obj)/util/x86emu/%.o: $(src)/util/x86emu/%.c
	$(Q)mkdir -p $(obj)/util/x86emu
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(INITCFLAGS) -I$(src)/util/x86emu/include -c $< -o $@


