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

obj:=$(objdir)
src:=$(srcdir)

$(obj)/linuxbios.rom: $(obj)/linuxbios.lar $(obj)/stage0.init $(obj)/linuxbios.vpd
	@cat $(obj)/linuxbios.lar \
	     $(obj)/linuxbios.vpd \
	     $(obj)/stage0.init > \
	     $(obj)/linuxbios.rom

#
# This part takes care of compression. It should build the compression modules
# in a way that they can be put in the lar. FIXME it belongs in lib/ 
#

lzma:
	$(Q)echo "building lzma"


#
# initram is always uncompressed. It belongs into the mainboard directory
# and is build from what was auto.c in v2
#
	
initram:
	$(Q)echo "building initram"

#
# this is the rest of linuxbios (v2: linuxbios_ram.rom) 
# Is this maybe platform independent, except for the "drivers"? 
# Where should it be built?
#
# This needs to be compressed with the default compressor
#

linuxbios_ram: $(obj)/statictree.o
	$(Q)echo "building linuxbios_ram"

#
# The payload as we love it. Get it from somewhere.
# Is this a place to incorporate buildrom?
# 
# This needs to be compressed with the default compressor
#
	
payload:
	$(Q)echo "building payload"


#
# build the lar archive
#
	
$(obj)/linuxbios.lar: $(obj)/util/lar/lar lzma linuxbios.initram linuxbios_ram payload 
	$(Q)echo "Building LinuxBIOS archive..."
	$(Q)rm -rf $(obj)/lar.tmp
	$(Q)mkdir $(obj)/lar.tmp
	$(Q)mkdir $(obj)/lar.tmp/normal
	$(Q)cp $(obj)/linuxbios.initram $(obj)/lar.tmp/normal/initram
	$(Q)cd $(obj)/lar.tmp && ../util/lar/lar c ../linuxbios.lar.pre normal/initram 
	# TODO: dynamically pad the lar archive. bs is image size - bootblock size (8k)
	$(Q)dd if=$(obj)/linuxbios.lar.pre of=$(obj)/linuxbios.lar bs=253952 count=1 conv=sync


#
# this is going to be the enable car code, lar parser and such:
#

INITCFLAGS  := $(CFLAGS) -I$(src)/include/cpu/generic/x86 -I$(src)/include -fno-builtin 

$(obj)/stage0.init: 
	@echo Building linuxbios.init
	# Building asm stub
	@$(CC) -E $(LINUXBIOSINCLUDE) $(src)/arch/x86/stage0_i586.S -o $(obj)/stage0_i586.s -DBOOTBLK=0x1f00 -DRESRVED=0xf0 -DDATE=\"`date +%Y/%m/%d`\"
	@$(AS) $(obj)/stage0_i586.s -o $(obj)/stage0_i586.o
	# Building cachemain
	$(CC) $(INITCFLAGS) -c $(src)/arch/x86/cachemain.c -o $(obj)/cachemain.o
	# 
	@$(CC) $(INITCFLAGS) -c $(src)/lib/lar.c -o $(obj)/lar.o
	#
	# console lib
	@$(CC) $(INITCFLAGS) -c $(src)/arch/x86/console.c -o $(obj)/console.o
	@$(CC) $(INITCFLAGS) -c $(src)/arch/x86/serial.c -o $(obj)/serial.o
	@$(CC) $(INITCFLAGS) -c $(src)/console/vtxprintf.c -o $(obj)/vtxprintf.o
	@$(CC) $(INITCFLAGS) -c $(src)/lib/uart8250.c -o $(obj)/uart8250.o
	# other lib parts
	@$(CC) $(INITCFLAGS) -c $(src)/lib/memcpy.c -o $(obj)/memcpy.o

	@cd $(obj); $(CC) -m32 -nostdlib -static -T $(src)/arch/x86/ldscript.ld cachemain.o console.o uart8250.o \
		serial.o vtxprintf.o lar.o memcpy.o stage0_i586.o -o stage0.o
	@objcopy -O binary $(obj)/stage0.o  $(obj)/stage0.init.pre

	# Pad boot block to 0x2000 - 0x100 
	# we will probably remove this step -- not needed if we continue with ldscript.ld
	@dd if=$(obj)/stage0.init.pre of=$(obj)/stage0.init bs=7936 conv=sync
	@echo "Len: `wc -c < $(obj)/stage0.init.pre`"
	#
	@test `wc -c < $(obj)/stage0.init.pre` -gt 7936  && echo "Error. Bootblock got too big" || true




