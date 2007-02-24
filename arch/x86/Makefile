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

INITCFLAGS  := $(CFLAGS) -I$(src)/include/cpu/generic/x86 -I$(src)/include \
	       -fno-builtin 

SILENT := &> /dev/null

# Build the final ROM image
#
# These are the main linuxbios components. Do not change the order unless you
# know exactly what you are doing.
#

LINUXBIOS_COMPONENTS := linuxbios.lar linuxbios.vpd stage0.init

$(obj)/linuxbios.rom: $(patsubst %,$(obj)/%,$(LINUXBIOS_COMPONENTS))
	$(Q)cat $^ > $@ 

#
# LinuxBIOS v3 is completely modular and based on a very small startup
# code (stage0) plus a LAR archive. The LAR archive may contain any number
# of stages, payloads and option roms.
#
# Build the LAR archive:
#

$(obj)/linuxbios.lar: $(obj)/util/lar/lar lzma $(obj)/linuxbios.initram $(obj)/linuxbios.stage2 payload
	$(Q)echo "Building LinuxBIOS archive..."
	$(Q)rm -rf $(obj)/lar.tmp
	$(Q)mkdir $(obj)/lar.tmp
	$(Q)mkdir $(obj)/lar.tmp/normal
	$(Q)cp $(obj)/linuxbios.initram $(obj)/lar.tmp/normal/initram
	$(Q)cp $(obj)/linuxbios.stage2 $(obj)/lar.tmp/normal/stage2
	$(Q)cp $(PAYLOAD) $(obj)/lar.tmp/normal/payload
	$(Q)cd $(obj)/lar.tmp && ../util/lar/lar c ../linuxbios.lar.pre normal/initram normal/stage2 normal/payload
	# TODO: dynamically pad the lar archive. bs is image size - bootblock size (8k)
	$(Q)dd if=$(obj)/linuxbios.lar.pre of=$(obj)/linuxbios.lar \
		  bs=245760 count=1 conv=sync $(SILENT)



#
# LinuxBIOS stage0. This is the LinuxBIOS "boot block code".
# It enables Cache-as-RAM and parses the LAR archive for an
# initram module and the various stages and payload files.
#

$(obj)/stage0.init: 
	$(Q)echo "Building stage0.init"
	$(Q)# Building asm stub
	$(Q)$(CC) -E $(LINUXBIOSINCLUDE) $(src)/arch/x86/stage0_i586.S -o $(obj)/stage0_i586.s -DBOOTBLK=0x1f00 -DRESRVED=0xf0 -DDATE=\"`date +%Y/%m/%d`\"
	$(Q)$(AS) $(obj)/stage0_i586.s -o $(obj)/stage0_i586.o
	$(Q)# Building cachemain
	$(Q)$(CC) $(INITCFLAGS) -c $(src)/arch/x86/cachemain.c -o $(obj)/cachemain.o

	$(Q)$(CC) $(INITCFLAGS) -c $(src)/lib/lar.c -o $(obj)/lar.o
	
	$(Q)# console lib
	$(Q)$(CC) $(INITCFLAGS) -c $(src)/arch/x86/console.c -o $(obj)/console.o
	$(Q)$(CC) $(INITCFLAGS) -c $(src)/arch/x86/serial.c -o $(obj)/serial.o
	$(Q)$(CC) $(INITCFLAGS) -c $(src)/arch/x86/archelfboot.c -o $(obj)/archelfboot.o
	$(Q)$(CC) $(INITCFLAGS) -c $(src)/console/vtxprintf.c -o $(obj)/vtxprintf.o
	$(Q)$(CC) $(INITCFLAGS) -c $(src)/console/vsprintf.c -o $(obj)/vsprintf.o
	$(Q)$(CC) $(INITCFLAGS) -c $(src)/lib/uart8250.c -o $(obj)/uart8250.o
	$(Q)# other lib parts
	$(Q)$(CC) $(INITCFLAGS) -c $(src)/lib/mem.c -o $(obj)/mem.o
	$(Q)$(CC) $(INITCFLAGS) -c $(src)/lib/newelfboot.c -o $(obj)/newelfboot.o

	$(Q)cd $(obj); $(CC) -m32 -nostdlib -static        \
		-T $(src)/arch/x86/ldscript.ld cachemain.o \
		console.o uart8250.o serial.o archelfboot.o vtxprintf.o  \
		vsprintf.o lar.o newelfboot.o mem.o stage0_i586.o -o stage0.o

	$(Q)objcopy -O binary $(obj)/stage0.o  $(obj)/stage0.init.pre

	# Pad boot block to 0x4000 - 0x100 
	# we will probably remove this step -- not needed if we continue with ldscript.ld

	$(Q)dd if=$(obj)/stage0.init.pre of=$(obj)/stage0.init \
		bs=16128 conv=sync $(SILENT)
	$(Q)echo "Len: `wc -c < $(obj)/stage0.init.pre`"

	$(Q)test `wc -c < $(obj)/stage0.init.pre` -gt 16128  && echo "Error. Bootblock got too big" || true





#
# This is the rest of linuxbios (v2: linuxbios_ram.rom) 
# Is this maybe platform independent, except for the "drivers"? 
# Where should it be built, maybe in device/ ?
#
# TODO: This should be compressed with the default compressor
#

$(obj)/linuxbios.stage2: $(obj)/stage0.init $(obj)/statictree.o
	$(Q)echo "Building linuxbios.stage2"
	$(Q)# main
	$(Q)$(CC) $(INITCFLAGS) -c $(src)/lib/stage2.c -o $(obj)/stage2.o
	$(Q)$(CC) $(INITCFLAGS) -c $(src)/device/device.c -o $(obj)/device.o
	$(Q)$(CC) $(INITCFLAGS) -c $(src)/device/device_util.c -o $(obj)/device_util.o
	$(Q)$(CC) $(INITCFLAGS) -c $(src)/device/root_device.c -o $(obj)/root_device.o
	$(Q)$(CC) $(INITCFLAGS) -c $(src)/lib/mem.c -o $(obj)/mem.o
	$(Q)$(CC) $(INITCFLAGS) -c $(src)/lib/malloc.c -o $(obj)/malloc.o
	$(Q)$(CC) $(INITCFLAGS) -c $(src)/lib/clog2.c -o $(obj)/clog2.o
	$(Q)$(CC) $(INITCFLAGS) -c -I$(src) $(obj)/statictree.c -o $(obj)/statictree.o
	$(Q)$(CC) $(INITCFLAGS) -c mainboard/$(MAINBOARDDIR)/mainboard.c -o $(obj)/mainboard.o

	$(Q)# leave a .o with full symbols in it for debugging.
	$(Q)cd $(obj); $(LD) -R $(obj)/stage0.o -Ttext 0x1000  \
		--entry=stage2 -o $(obj)/linuxbios.stage2.o \
		stage2.o device.o device_util.o root_device.o  \
		 mem.o malloc.o clog2.o mainboard.o \
		statictree.o
	$(Q)objcopy -O binary $(obj)/linuxbios.stage2.o $(obj)/linuxbios.stage2

	$(Q)chmod 644 $(obj)/linuxbios.stage2

#
# The payload as we love it. Get it from somewhere.
# Is this a place to incorporate buildrom?
# 
# TODO: This is not implemented yet.
# TODO: This needs to be compressed with the default compressor
#
	
payload:
	$(Q)echo "Building payload... skipped"




