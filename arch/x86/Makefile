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

ifeq ($(CONFIG_ARCH_X86),y)

INITCFLAGS := $(CFLAGS) -I$(src)/include/arch/x86 -I$(src)/include \
		-I$(obj) \
		-fno-builtin

SILENT := >/dev/null 2>&1

#
# Build the ROM Image / LAR archive
#
# LinuxBIOS v3 is completely modular. One module, the bootblock (stage0),
# is mandatory. All modules are packed together in a LAR archive.
# The LAR archive may contain any number of stages, payloads and option ROMs.
#

ROM_SIZE := $(shell expr $(CONFIG_LINUXBIOS_ROMSIZE_KB) \* 1024)

$(obj)/linuxbios.rom: $(obj)/linuxbios.bootblock $(obj)/util/lar/lar lzma nrv2b $(obj)/linuxbios.initram $(obj)/linuxbios.stage2 $(obj)/option_table
	$(Q)rm -rf $(obj)/lar.tmp
	$(Q)mkdir $(obj)/lar.tmp
	$(Q)mkdir $(obj)/lar.tmp/normal
	$(Q)cp $(obj)/linuxbios.initram $(obj)/lar.tmp/normal/initram
	$(Q)cp $(obj)/linuxbios.stage2 $(obj)/lar.tmp/normal/stage2
	$(Q)cp $(obj)/option_table $(obj)/lar.tmp/normal/option_table
ifeq ($(CONFIG_PAYLOAD_NONE),y)
	$(Q)printf "  PAYLOAD none (as specified by user)\n"
else
	$(Q)# TODO: Print sth. other than $(CONFIG_PAYLOAD_FILE) if compressed.
	$(Q)if [ -r $(CONFIG_PAYLOAD_FILE) ]; then \
		printf "  PAYLOAD $(CONFIG_PAYLOAD_FILE)\n"; \
		cp $(CONFIG_PAYLOAD_FILE) $(obj)/lar.tmp/normal/payload; \
	else \
		printf "Error: payload file '$(CONFIG_PAYLOAD_FILE)' not found.\n"; \
		exit 1; \
	fi
endif
	$(Q)printf "  LAR     $(subst $(shell pwd)/,,$(@))\n"
	$(Q)cd $(obj)/lar.tmp && ../util/lar/lar -c ../linuxbios.rom . \
		-s $(ROM_SIZE) -b $(obj)/linuxbios.bootblock
	$(Q)# QEMU wants bios.bin:
	$(Q)# Run "qemu -L build/ -serial stdio -hda /dev/zero".
	$(Q)printf "  CP      $(subst $(shell pwd)/,,$(obj)/bios.bin)\n"
	$(Q)cp $@ $(obj)/bios.bin

$(obj)/linuxbios.bootblock: $(obj)/linuxbios.vpd $(obj)/stage0.init
	$(Q)printf "  BUILD   $(subst $(shell pwd)/,,$(@))\n"
	$(Q)cat $^ > $@

#
# LinuxBIOS stage0. This is the LinuxBIOS "boot block code".
# It enables Cache-as-RAM and parses the LAR archive for an
# initram module and the various stages and payload files.
#

STAGE0_LIB_OBJ       = uart8250.o mem.o elfboot.o lar.o delay.o vtxprintf.o \
		       vsprintf.o console.o
STAGE0_ARCH_X86_OBJ  = stage1.o serial.o archelfboot.o speaker.o \
		       udelay_io.o mc146818rtc.o

ifeq ($(CONFIG_CPU_I586),y)
	STAGE0_CAR_OBJ = stage0_i586.o
else
ifeq ($(CONFIG_CPU_AMD_GEODELX),y)
	STAGE0_CAR_OBJ = geodelx/stage0.o
else
	STAGE0_CAR_OBJ = stage0_i586.o
endif
endif


STAGE0_OBJ := $(patsubst %,$(obj)/lib/%,$(STAGE0_LIB_OBJ)) \
	      $(patsubst %,$(obj)/arch/x86/%,$(STAGE0_ARCH_X86_OBJ)) \
	      $(patsubst %,$(obj)/arch/x86/%,$(STAGE0_CAR_OBJ)) \
		$(STAGE0_MAINBOARD_OBJ)

$(obj)/stage0.o $(obj)/stage0.init: $(STAGE0_OBJ)
	$(Q)# We need to be careful. If stage0.o gets bigger than
	$(Q)# 0x4000 - 0x100, we will end up with a 4 gig file.
	$(Q)# I wonder if that behavior is on purpose.

	$(Q)# Note: we invoke gcc (instead of ld directly) here, as we hit
	$(Q)# strange problems in the past. It seems that only gcc knows how
	$(Q)# to properly invoke ld.
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) -nostdlib -static -T $(src)/arch/x86/ldscript.ld \
		$(STAGE0_OBJ) -o $(obj)/stage0.o

	$(Q)printf "  OBJCOPY $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(OBJCOPY) -O binary $(obj)/stage0.o $(obj)/stage0.init

	$(Q)printf "  TEST    $(subst $(shell pwd)/,,$(@))\n"
	$(Q)test `wc -c < $(obj)/stage0.init` -gt 16128 && \
		printf "Error. Bootblock got too big.\n" || true


#
# This is the rest of LinuxBIOS (v2: linuxbios_ram.rom).
# Is this maybe platform independent, except for the "drivers"?
# Where should it be built, maybe in device/?
#
# TODO: This should be compressed with the default compressor.
#

STAGE2_LIB_OBJ       = stage2.o clog2.o mem.o tables.o delay.o compute_ip_checksum.o

STAGE2_ARCH_X86_OBJ  = archtables.o linuxbios_table.o udelay_io.o
STAGE2_ARCH_X86_OBJ += pci_ops_auto.o pci_ops_conf1.o pci_ops_conf2.o
STAGE2_ARCH_X86_OBJ += keyboard.o i8259.o isa-dma.o

STAGE2_DYNAMIC_OBJ   = statictree.o

STAGE2_OBJ := $(patsubst %,$(obj)/lib/%,$(STAGE2_LIB_OBJ)) \
	      $(patsubst %,$(obj)/arch/x86/%,$(STAGE2_ARCH_X86_OBJ)) \
	      $(patsubst %,$(obj)/device/%,$(STAGE2_DEVICE_OBJ)) \
	      $(patsubst %,$(obj)/mainboard/$(MAINBOARDDIR)/%,$(STAGE2_MAINBOARD_OBJ)) \
	      $(patsubst %,$(obj)/mainboard/$(MAINBOARDDIR)/%,$(STAGE2_DYNAMIC_OBJ))

STAGE2_OBJ += $(STAGE2_CHIPSET_OBJ)

ifeq ($(CONFIG_PCI_OPTION_ROM_RUN),y)
ifeq ($(CONFIG_PCI_OPTION_ROM_RUN_X86EMU),y)
# x86emu wants libgcc
ifneq ($(strip $(CC)),)
LIBGCC_FILE_NAME := $(shell $(CC) -print-libgcc-file-name)
endif
endif
STAGE2_OBJ += $(obj)/util/x86emu/libx86emu.a $(LIBGCC_FILE_NAME)
endif

$(obj)/linuxbios.stage2: $(obj)/stage0.init $(STAGE2_OBJ)
	$(Q)# leave a .o with full symbols in it for debugging.
	$(Q)printf "  LD      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(LD) -R $(obj)/stage0.o -Ttext 0x1000 --entry=stage2 \
		  -o $(obj)/linuxbios.stage2.o $(STAGE2_OBJ)
	$(Q)printf "  OBJCOPY $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(OBJCOPY) -O binary $(obj)/linuxbios.stage2.o $(obj)/linuxbios.stage2


#
# Build rules.
#

$(obj)/arch/x86/%.o: $(src)/arch/x86/%.c
	$(Q)mkdir -p $(obj)/arch/x86
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(INITCFLAGS) -c $< -o $@

$(obj)/mainboard/$(MAINBOARDDIR)/%.o: $(src)/mainboard/$(MAINBOARDDIR)/%.c
	$(Q)mkdir -p $(obj)/mainboard/$(MAINBOARDDIR)
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(INITCFLAGS) -c $< -o $@

# Building asm stub.
$(obj)/arch/x86/stage0%.o: $(src)/arch/x86/stage0%.S
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) -E $(LINUXBIOSINCLUDE) $< \
		-o $(obj)/arch/x86/stage0_asm.s -DBOOTBLK=0x1f00 \
		-DRESRVED=0xf0 -DDATE=\"`date +%Y/%m/%d`\"
	$(Q)printf "  AS      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(AS) $(obj)/arch/x86/stage0_asm.s -o $@

$(obj)/arch/x86/geodelx/stage0.o: $(src)/arch/x86/geodelx/stage0.S
	$(Q)mkdir -p $(obj)/arch/x86/geodelx
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) -E $(LINUXBIOSINCLUDE) $< \
		-o $(obj)/arch/x86/stage0_asm.s -DBOOTBLK=0x1f00 \
		-DRESRVED=0xf0 -DDATE=\"`date +%Y/%m/%d`\"
	$(Q)printf "  AS      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(AS) $(obj)/arch/x86/stage0_asm.s -o $@

endif
