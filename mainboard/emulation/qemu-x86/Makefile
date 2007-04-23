##
## This file is part of the LinuxBIOS project.
##
## Copyright (C) 2006-2007 coresystems GmbH
## Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH.
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

#
# VPD or SIP ROM or ... how does NVIDIA call it?
# Some space to cope with dirty southbridge tricks.
# Do we want to put our own stuff there, too?
#

$(obj)/linuxbios.vpd:
	$(Q)printf "  BUILD   DUMMY VPD\n"
	$(Q)dd if=/dev/zero of=$(obj)/linuxbios.vpd bs=256 count=1 $(SILENT)

#
# This is going to be the init RAM code.
#
# The initram file is always uncompressed. It belongs into the mainboard
# directory and is built from what was auto.c in v2.
#

$(obj)/linuxbios.initram: $(obj)/stage0.init $(obj)/stage0.o $(obj)/mainboard/$(MAINBOARDDIR)/initram.o
	$(Q)# initram links against stage0
	$(Q)printf "  LD      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(LD) -R $(obj)/stage0.o -Ttext 0x80000 \
		$(obj)/mainboard/$(MAINBOARDDIR)/initram.o \
		--entry=main -o $(obj)/linuxbios.initram.o
	$(Q)printf "  OBJCOPY $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(OBJCOPY) -O binary $(obj)/linuxbios.initram.o \
		$(obj)/linuxbios.initram

#
# Miscellaneous important targets.
#

$(obj)/mainboard/$(MAINBOARDDIR)/mainboard.o: $(obj)/mainboard/$(MAINBOARDDIR)/statictree.o

$(obj)/mainboard/$(MAINBOARDDIR)/statictree.o: $(obj)/mainboard/$(MAINBOARDDIR)/statictree.c
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(CFLAGS) $(LINUXBIOSINCLUDE) -c -o $@ $<

$(obj)/mainboard/$(MAINBOARDDIR)/statictree.c: $(src)/mainboard/$(MAINBOARDDIR)/dts $(obj)/util/dtc/dtc
	$(Q)printf "  DTC     $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(obj)/util/dtc/dtc -O lb mainboard/$(MAINBOARDDIR)/dts > $@

$(obj)/mainboard/$(MAINBOARDDIR)/option_table.c: $(obj)/util/options/build_opt_tbl $(src)/mainboard/$(MAINBOARDDIR)/cmos.layout
	$(Q)printf "  OPTIONS $(subst $(shell pwd)/,,$(@))\n"
	$(Q)mkdir -p $(obj)/mainboard/$(MAINBOARDDIR)
	$(Q)$(obj)/util/options/build_opt_tbl -b \
		--config $(src)/mainboard/$(MAINBOARDDIR)/cmos.layout \
		--option $(obj)/mainboard/$(MAINBOARDDIR)/option_table.c 

$(obj)/option_table: $(obj)/mainboard/$(MAINBOARDDIR)/option_table.o
	$(Q)printf "  OBJCOPY $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(OBJCOPY) -O binary $< $@ 

STAGE2_MAINBOARD_OBJ = mainboard.o vga.o 

STAGE2_CHIPSET_OBJ = 

$(obj)/mainboard/$(MAINBOARDDIR)/%.o: $(src)/nboard/$(MAINBOARDDIR)/%.c
	$(Q)mkdir -p $(obj)/mainboard/$(MAINBOARDDIR)
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(INITCFLAGS) -c $< -o $@

