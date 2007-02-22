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
# VPD or SIP ROM or ... how does nvidia call it? 
# Some space to cope with dirty southbridge tricks. 
# Do we want to put our own stuff there, too?
#

$(obj)/linuxbios.vpd:
	@printf "Building dummy VPD ..."
	@dd if=/dev/zero of=$(obj)/linuxbios.vpd bs=256 count=1
	@printf "ok.\n"


#
# this is going to be the init ram code:
#

$(obj)/linuxbios.initram: $(obj)/stage0.o
	$(Q)echo Building linuxbios.initram
	#
	# main
	$(Q)$(CC) $(INITCFLAGS) -c $(src)/mainboard/$(MAINBOARDDIR)/initram.c -o $(obj)/initram.o
	#
	# console lib
	#$(Q)$(CC) $(INITCFLAGS) -c $(src)/arch/x86/console.c -o $(obj)/console.o
	#$(Q)$(CC) $(INITCFLAGS) -c $(src)/arch/x86/serial.c -o $(obj)/serial.o
	#$(Q)$(CC) $(INITCFLAGS) -c $(src)/console/vtxprintf.c -o $(obj)/vtxprintf.o
	#$(Q)$(CC) $(INITCFLAGS) -c $(src)/lib/uart8250.c -o $(obj)/uart8250.o

	#cd $(obj); $(Q)$(LD) -Ttext 0x00000000 -s --oformat binary initram.o console.o uart8250.o \
	#	serial.o vtxprintf.o -o $(obj)/linuxbios.initram

	$(Q)$(LD) -R $(obj)/stage0.o -Ttext 0x80000  $(obj)/initram.o \
		-o $(obj)/linuxbios.initram.o

	$(Q)objcopy -O binary $(obj)/linuxbios.initram.o $(obj)/linuxbios.initram

	$(Q)chmod 644 $(obj)/linuxbios.initram


# miscellaneous important targets. 
$(obj)/mainboard.o: $(obj)/statictree.o

$(obj)/statictree.o:  $(obj)/statictree.c
	$(CC) $(CFLAGS) $(LINUXBIOSINCLUDE) -c -o $(obj)/statictree.o $(obj)/statictree.c

$(obj)/statictree.c: mainboard/$(MAINBOARDDIR)/dts $(obj)/util/dtc/dtc
	$(obj)/util/dtc/dtc -O lb mainboard/$(MAINBOARDDIR)/dts >$(obj)/statictree.c



