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

ifeq ($(CONFIG_ARCH_X86),y)

INITCFLAGS := $(CFLAGS) -I$(src)/include/cpu/generic/x86 -I$(src)/include \
		-fno-builtin

SILENT := &> /dev/null

#
# Build the final ROM image.
#
# These are the main LinuxBIOS components. Do not change the order unless you
# know exactly what you are doing.
#

LINUXBIOS_COMPONENTS := linuxbios.lar linuxbios.vpd stage0.init

$(obj)/linuxbios.rom: $(patsubst %,$(obj)/%,$(LINUXBIOS_COMPONENTS))
	$(Q)cat $^ > $@

#
# Build the LAR archive.
#
# LinuxBIOS v3 is completely modular and based on a very small startup
# code (stage0) plus a LAR archive. The LAR archive may contain any number
# of stages, payloads and option ROMs.
#

$(obj)/linuxbios.lar: $(obj)/util/lar/lar lzma $(obj)/linuxbios.initram $(obj)/linuxbios.stage2 payload
	$(Q)printf "Building LinuxBIOS archive... \n"
	$(Q)rm -rf $(obj)/lar.tmp
	$(Q)mkdir $(obj)/lar.tmp
	$(Q)mkdir $(obj)/lar.tmp/normal
	$(Q)cp $(obj)/linuxbios.initram $(obj)/lar.tmp/normal/initram
	$(Q)cp $(obj)/linuxbios.stage2 $(obj)/lar.tmp/normal/stage2
	$(Q)cp $(PAYLOAD) $(obj)/lar.tmp/normal/payload
	$(Q)printf "  "
	$(Q)cd $(obj)/lar.tmp && ../util/lar/lar c ../linuxbios.lar.pre normal/initram normal/stage2 normal/payload
	$(Q)# TODO: Dynamically pad the LAR archive. bs is image size - bootblock size (16k)
	$(Q)dd if=$(obj)/linuxbios.lar.pre of=$(obj)/linuxbios.lar \
		  bs=245760 count=1 conv=sync $(SILENT)


#
# LinuxBIOS stage0. This is the LinuxBIOS "boot block code".
# It enables Cache-as-RAM and parses the LAR archive for an
# initram module and the various stages and payload files.
#

STAGE0_CONSOLE_OBJ   = $(obj)/vtxprintf.o $(obj)/vsprintf.o
STAGE0_LIB_OBJ       = $(obj)/uart8250.o $(obj)/mem.o $(obj)/elfboot.o \
		       $(obj)/lar.o
STAGE0_ARCH_X86_OBJ  = $(obj)/cachemain.o $(obj)/console.o $(obj)/serial.o \
		       $(obj)/archelfboot.o


ifeq ($(CONFIG_CAR_TYPE_I586),y)
  STAGE0_CAR_OBJ = $(obj)/stage0_i586.o
else
  STAGE0_CAR_OBJ = $(obj)/stage0_i586.o
endif

STAGE0_OBJ := $(STAGE0_CONSOLE_OBJ) $(STAGE0_LIB_OBJ) $(STAGE0_ARCH_X86_OBJ) $(STAGE0_CAR_OBJ)

$(obj)/stage0.init: $(STAGE0_OBJ)
	$(Q)printf "Building stage0.init... "

	$(Q)# We need to be careful. If stage0.o gets bigger than 
	$(Q)# 0x4000 - 0x100, we will end up with a 4 gig file. 
	$(Q)# I wonder if that behavior is on purpose.

	$(Q)$(CC) -nostdlib -static -T $(src)/arch/x86/ldscript.ld \
		$(STAGE0_OBJ) -o $(obj)/stage0.o

	$(Q)objcopy -O binary $(obj)/stage0.o $(obj)/stage0.init

	$(Q)test `wc -c < $(obj)/stage0.init` -gt 16128 && \
		printf "Error. Bootblock got too big.\n" || true
	$(Q)printf "done\n"


#
# This is the rest of LinuxBIOS (v2: linuxbios_ram.rom).
# Is this maybe platform independent, except for the "drivers"?
# Where should it be built, maybe in device/?
#
# TODO: This should be compressed with the default compressor.
#

STAGE2_LIB_OBJ       = $(obj)/stage2.o $(obj)/clog2.o $(obj)/mem.o $(obj)/malloc.o \
		       $(obj)/tables.o
STAGE2_DEVICE_OBJ    = $(obj)/device.o $(obj)/device_util.o \
		       $(obj)/root_device.o
STAGE2_ARCH_X86_OBJ  = $(obj)/archtables.o $(obj)/linuxbios_table.o
STAGE2_MAINBOARD_OBJ = $(obj)/mainboard.o
STAGE2_DYNAMIC_OBJ   = $(obj)/statictree.o

STAGE2_OBJ := $(STAGE2_LIB_OBJ) $(STAGE2_DEVICE_OBJ) $(STAGE2_ARCH_X86_OBJ)
STAGE2_OBJ += $(STAGE2_MAINBOARD_OBJ) $(STAGE2_DYNAMIC_OBJ) $(STAGE2_CHIPSET_OBJ)

$(obj)/linuxbios.stage2: $(obj)/stage0.init $(STAGE2_OBJ)
	$(Q)printf "Building linuxbios.stage2... "
	$(Q)# leave a .o with full symbols in it for debugging.
	$(Q)$(LD) -R $(obj)/stage0.o -Ttext 0x1000 --entry=stage2 \
		  -o $(obj)/linuxbios.stage2.o $(STAGE2_OBJ) 
	$(Q)objcopy -O binary $(obj)/linuxbios.stage2.o $(obj)/linuxbios.stage2
	$(Q)printf "done\n"

#
# The payload as we love it. Get it from somewhere.
# Is this a place to incorporate buildrom?
#
# TODO: This is not implemented yet.
# TODO: This needs to be compressed with the default compressor.
#

payload:
	$(Q)printf "Building payload... skipped\n"


# build rules
#
$(obj)/%.o: $(src)/arch/x86/%.c
	$(Q)$(CC) $(INITCFLAGS) -c $< -o $@

$(obj)/%.o: $(src)/mainboard/$(MAINBOARDDIR)/%.c
	$(Q)$(CC) $(INITCFLAGS) -c $< -o $@

# Building asm stub
$(obj)/stage0%.o: $(src)/arch/x86/stage0%.S
	$(Q)$(CC) -E $(LINUXBIOSINCLUDE) $< \
		-o $(obj)/stage0_asm.s -DBOOTBLK=0x1f00 -DRESRVED=0xf0 \
		-DDATE=\"`date +%Y/%m/%d`\"
	$(Q)$(AS) $(obj)/stage0_asm.s -o $@

endif
