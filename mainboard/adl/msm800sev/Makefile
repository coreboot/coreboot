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

STAGE0_MAINBOARD_OBJ := $(obj)/mainboard/$(MAINBOARDDIR)/stage1.o

STAGE2_MAINBOARD_OBJ =

$(obj)/linuxbios.vpd:
	$(Q)printf "  BUILD   DUMMY VPD\n"
	$(Q)dd if=/dev/zero of=$(obj)/linuxbios.vpd bs=256 count=1 $(SILENT)

INITRAM_OBJ =   $(obj)/mainboard/$(MAINBOARDDIR)/initram.o \
		$(obj)/northbridge/amd/geodelx/raminit.o \
		$(obj)/southbridge/amd/cs5536/smbus_initram.o \
		$(obj)/arch/x86/geodelx/geodelx.o

# Next Quest: Make a single rule out of those:
$(obj)/mainboard/$(MAINBOARDDIR)/initram.o: $(src)/mainboard/$(MAINBOARDDIR)/initram.c
	$(Q)$(CC) $(INITCFLAGS) -D_SHARED -fPIE -c $< -o $@
$(obj)/northbridge/amd/geodelx/raminit.o: $(src)/northbridge/amd/geodelx/raminit.c
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(INITCFLAGS) -D_SHARED -fPIE -c $< -o $@
$(obj)/southbridge/amd/cs5536/smbus_initram.o: $(src)/southbridge/amd/cs5536/smbus_initram.c
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(INITCFLAGS) -D_SHARED -fPIE -c $< -o $@
$(obj)/arch/x86/geodelx/geodelx.o: $(src)/arch/x86/geodelx/geodelx.c
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(INITCFLAGS) -D_SHARED -fPIE -c $< -o $@

$(obj)/linuxbios.initram $(obj)/linuxbios.initram.map: $(obj)/stage0.init $(obj)/stage0-prefixed.o $(INITRAM_OBJ)
	$(Q)# initram links against stage0
	$(Q)printf "  LD      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(LD) --entry main -N -R $(obj)/stage0-prefixed.o \
		$(INITRAM_OBJ) -o $(obj)/linuxbios.initram.o
	$(Q)printf "  OBJCOPY $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(OBJCOPY) -O binary $(obj)/linuxbios.initram.o \
		$(obj)/linuxbios.initram
	$(Q)printf "  NM      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(NM) $(obj)/linuxbios.initram.o | sort -u > $(obj)/linuxbios.initram.map

