##
## This file is part of the LinuxBIOS project.
##
## Copyright (C) 2006-2007 coresystems GmbH
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
	$(Q)printf "Building dummy VPD... "
	$(Q)dd if=/dev/zero of=$(obj)/linuxbios.vpd bs=256 count=1 $(SILENT)
	$(Q)printf "done\n"

#
# This is going to be the init RAM code.
#
# The initram file is always uncompressed. It belongs into the mainboard
# directory and is built from what was auto.c in v2.
#

$(obj)/linuxbios.initram: $(obj)/stage0.init $(obj)/stage0.o $(obj)/initram.o
	$(Q)printf "Building linuxbios.initram... "
	$(Q)# initram links against stage0
	$(Q)$(LD) -R $(obj)/stage0.o -Ttext 0x80000 $(obj)/initram.o \
		--entry=main -o $(obj)/linuxbios.initram.o
	$(Q)objcopy -O binary $(obj)/linuxbios.initram.o \
		$(obj)/linuxbios.initram
	$(Q)printf "done\n"

#
# Miscellaneous important targets.
#

$(obj)/mainboard.o: $(obj)/statictree.o

$(obj)/statictree.o: $(obj)/statictree.c
	$(Q)$(CC) $(CFLAGS) $(LINUXBIOSINCLUDE) -c -o $(obj)/statictree.o $(obj)/statictree.c

$(obj)/statictree.c: mainboard/$(MAINBOARDDIR)/dts $(obj)/util/dtc/dtc
	$(Q)$(obj)/util/dtc/dtc -O lb mainboard/$(MAINBOARDDIR)/dts >$(obj)/statictree.c 2>/dev/null

STAGE2_CHIPSET_OBJ = 
# chipset
include $(src)/northbridge/intel/i440bxemulation/Makefile


