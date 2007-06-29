##
## This file is part of the LinuxBIOS project.
##
## Copyright (C) 2006-2007 coresystems GmbH
## (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
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
STAGE0_MAINBOARD_OBJ := \
		$(obj)/southbridge/amd/cs5536/stage1.o\
		$(obj)/arch/x86/geodelx/stage1.o \
		$(obj)/mainboard/$(MAINBOARDDIR)/stage1.o

$(obj)/linuxbios.vpd:
	$(Q)printf "  BUILD   DUMMY VPD\n"
	$(Q)dd if=/dev/zero of=$(obj)/linuxbios.vpd bs=256 count=1 $(SILENT)

INITRAM_OBJ =   $(obj)/mainboard/$(MAINBOARDDIR)/initram.o \
		$(obj)/arch/x86/geodelx/geodelx.o

$(obj)/linuxbios.initram: $(obj)/stage0.init $(obj)/stage0.o $(INITRAM_OBJ)
	$(Q)# initram links against stage0
	$(Q)printf "  LD      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(LD) -R $(obj)/stage0.o -Ttext 0x80000 $(INITRAM_OBJ) \
		--entry=main -o $(obj)/linuxbios.initram.o
	$(Q)printf "  OBJCOPY $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(OBJCOPY) -O binary $(obj)/linuxbios.initram.o \
		$(obj)/linuxbios.initram

#
# Miscellaneous important targets.
#

$(obj)/mainboard/$(MAINBOARDDIR)/statictree.o: $(obj)/mainboard/$(MAINBOARDDIR)/statictree.c $(obj)/statictree.h
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(INITCFLAGS) $(LINUXBIOSINCLUDE) -c -o $@ $<

$(obj)/mainboard/$(MAINBOARDDIR)/statictree.c: $(src)/mainboard/$(MAINBOARDDIR)/dts $(obj)/util/dtc/dtc
	$(Q)printf "  DTC     $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(obj)/util/dtc/dtc -O lb mainboard/$(MAINBOARDDIR)/dts > $@

$(obj)/statictree.h: $(src)/mainboard/$(MAINBOARDDIR)/dts $(obj)/util/dtc/dtc
	$(Q)printf "  DTC     $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(obj)/util/dtc/dtc -O lbh mainboard/$(MAINBOARDDIR)/dts > $@

$(obj)/mainboard/$(MAINBOARDDIR)/option_table.c: $(obj)/util/options/build_opt_tbl $(src)/mainboard/$(MAINBOARDDIR)/cmos.layout
	$(Q)printf "  OPTIONS $(subst $(shell pwd)/,,$(@))\n"
	$(Q)mkdir -p $(obj)/mainboard/$(MAINBOARDDIR)
	$(Q)$(obj)/util/options/build_opt_tbl -b \
		--config $(src)/mainboard/$(MAINBOARDDIR)/cmos.layout \
		--option $(obj)/mainboard/$(MAINBOARDDIR)/option_table.c 

$(obj)/option_table: $(obj)/mainboard/$(MAINBOARDDIR)/option_table.o
	$(Q)printf "  OBJCOPY $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(OBJCOPY) -O binary $< $@ 

STAGE2_MAINBOARD_OBJ = 

STAGE2_CHIPSET_OBJ =  		$(obj)/arch/x86/geodelx/cpu.o

$(obj)/mainboard/$(MAINBOARDDIR)/%.o: $(src)/mainboard/$(MAINBOARDDIR)/%.c
	$(Q)mkdir -p $(obj)/mainboard/$(MAINBOARDDIR)
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(INITCFLAGS) -c $< -o $@

