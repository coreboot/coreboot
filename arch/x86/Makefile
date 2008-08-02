##
## This file is part of the coreboot project.
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
		-I$(obj) -fno-builtin  

SILENT := >/dev/null 2>&1

#
# Build the ROM Image / LAR archive
#
# coreboot v3 is completely modular. One module, the bootblock (stage0),
# is mandatory. All modules are packed together in a LAR archive.
# The LAR archive may contain any number of stages, payloads and option ROMs.
#

ROM_SIZE := $(shell expr $(CONFIG_COREBOOT_ROMSIZE_KB) \* 1024)

LARFILES_NOCOMPRESS := coreboot.initram:normal/initram option_table:normal/option_table
LARFILES_COMPRESSIBLE := coreboot.stage2:normal/stage2

DECOMPRESSORS :=
ifeq ($(CONFIG_COMPRESSION_LZMA),y)
DECOMPRESSORS += lzma.o
endif
ifeq ($(CONFIG_COMPRESSION_NRV2B),y)
DECOMPRESSORS += nrv2b.o
endif

COMPRESSFLAG :=
ifeq ($(CONFIG_DEFAULT_COMPRESSION_LZMA),y)
COMPRESSFLAG := -C lzma
endif
ifeq ($(CONFIG_DEFAULT_COMPRESSION_NRV2B),y)
COMPRESSFLAG := -C nrv2b
endif

$(obj)/coreboot.rom $(obj)/coreboot.map: $(obj)/coreboot.bootblock $(obj)/util/lar/lar lzma nrv2b $(obj)/coreboot.initram $(obj)/coreboot.stage2 $(obj)/option_table
	$(Q)printf "  LAR     $(subst $(shell pwd)/,,$(@))\n"
	$(Q)rm -f $(obj)/coreboot.rom
	$(Q)cd $(obj) && \
		./util/lar/lar -e -c coreboot.rom \
			-s $(ROM_SIZE) -b coreboot.bootblock \
			$(LARFILES_NOCOMPRESS)
	$(Q)cd $(obj) && \
		./util/lar/lar -e $(COMPRESSFLAG) -a coreboot.rom \
			$(LARFILES_COMPRESSIBLE)
ifeq ($(CONFIG_PAYLOAD_NONE),y)
	$(Q)printf "  PAYLOAD none (as specified by user)\n"
else
	$(Q)if [ -r $(CONFIG_PAYLOAD_FILE) ]; then \
		printf "  PAYLOAD $(CONFIG_PAYLOAD_FILE) $(COMPRESSFLAG)\n"; \
	else \
		printf "Error: payload file '$(CONFIG_PAYLOAD_FILE)' not found.\n"; \
		exit 1; \
	fi
	$(Q)$(obj)/util/lar/lar $(PARSEELF) $(COMPRESSFLAG) -a \
		$(obj)/coreboot.rom $(CONFIG_PAYLOAD_FILE):normal/payload;
endif
ifeq ($(CONFIG_ZERO_AFTER_PAYLOAD),y)
	$(Q)printf "  ZEROING lar -z ./coreboot.rom\n"
	$(Q)cd $(obj) && ./util/lar/lar -z ./coreboot.rom
endif
	$(Q)# QEMU wants bios.bin:
	$(Q)# Run "qemu -L build/ -serial stdio -hda /dev/zero".
	$(Q)printf "  CP      $(subst $(shell pwd)/,,$(obj)/bios.bin)\n"
	$(Q)cp $@ $(obj)/bios.bin
	$(Q)echo "Coreboot ROM Image:" > $(obj)/coreboot.map
	$(Q)$(obj)/util/lar/lar -l $(obj)/coreboot.rom >> $(obj)/coreboot.map
	$(Q)(echo; echo "Stage 0/1 Map:") >> $(obj)/coreboot.map
	$(Q)cat $(obj)/stage0.init.map >> $(obj)/coreboot.map
	$(Q)(echo; echo "Stage Initram Map:") >> $(obj)/coreboot.map
	$(Q)cat $(obj)/coreboot.initram.map >> $(obj)/coreboot.map
	$(Q)(echo; echo "Stage 2 Map:") >> $(obj)/coreboot.map
	$(Q)cat $(obj)/coreboot.stage2.map >> $(obj)/coreboot.map


$(obj)/coreboot.bootblock: $(obj)/coreboot.vpd $(obj)/stage0.init
	$(Q)printf "  BUILD   $(subst $(shell pwd)/,,$(@))\n"
	$(Q)cat $^ > $@

#
# Coreboot stage0. This is the coreboot "boot block code".
# It enables Cache-as-RAM and parses the LAR archive for an
# initram module and the various stages and payload files.
#


STAGE0_LIB_OBJ       = uart8250.o mem.o lar.o delay.o vtxprintf.o \
		       vsprintf.o console.o string.o $(DECOMPRESSORS)
STAGE0_ARCH_X86_OBJ  = stage1.o serial.o speaker.o \
		       udelay_io.o mc146818rtc.o post_code.o

ifeq ($(CONFIG_PAYLOAD_ELF_LOADER),y)
STAGE0_LIB_OBJ      += elfboot.o
STAGE0_ARCH_X86_OBJ += archelfboot.o
endif

ifeq ($(CONFIG_CPU_I586),y)
	STAGE0_CAR_OBJ = stage0_i586.o
else
ifeq ($(CONFIG_CPU_AMD_GEODELX),y)
	STAGE0_CAR_OBJ = geodelx/stage0.o
	STAGE0_ARCH_X86_OBJ += geodelx/stage1.o
	STAGE0_ARCH_X86_OBJ += ../../northbridge/amd/geodelx/geodelxinit.o
else
ifeq ($(CONFIG_CPU_AMD_K8),y)
	STAGE0_CAR_OBJ = amd/stage0.o
	STAGE0_ARCH_X86_OBJ += amd/k8/stage1.o
endif
endif
endif

ifeq ($(CONFIG_PAYLOAD_NO_PREPARSE_ELF), y)
	PARSEELF =
else
	PARSEELF = -e
endif

STAGE0_OBJ := $(patsubst %,$(obj)/lib/%,$(STAGE0_LIB_OBJ)) \
	      $(patsubst %,$(obj)/arch/x86/%,$(STAGE0_ARCH_X86_OBJ)) \
	      $(patsubst %,$(obj)/arch/x86/%,$(STAGE0_CAR_OBJ)) \
	      $(STAGE0_MAINBOARD_OBJ) $(STAGE0_CHIPSET_OBJ)

$(obj)/stage0.o $(obj)/stage0.init $(obj)/stage0-prefixed.o: $(STAGE0_OBJ)
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

	$(Q)# Do another OBJCOPY to get a copy with renamed symbols 
	$(Q)# for XIP code.
	$(Q)printf "  OBJCOPY $(subst $(shell pwd)/,,$(@)) (prefixing stage0)\n"
	$(Q)$(OBJCOPY) --prefix-symbols=stage0_ $(obj)/stage0.o $(obj)/stage0-prefixed.o

	$(Q)printf "  TEST    $(subst $(shell pwd)/,,$(@))\n"
	$(Q)test `wc -c < $(obj)/stage0.init` -gt 20224 && \
		printf "Error. Bootblock got too big.\n" || true
	$(Q)printf "  NM      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(NM) $(obj)/stage0.o | sort -u > $(obj)/stage0.init.map



#
# This is the rest of coreboot (v2: coreboot_ram.rom).
# Is this maybe platform independent, except for the "drivers"?
# Where should it be built, maybe in device/?
#
# TODO: This should be compressed with the default compressor.
#

STAGE2_LIB_OBJ       = stage2.o clog2.o mem.o tables.o delay.o \
		       compute_ip_checksum.o string.o

STAGE2_ARCH_X86_OBJ  = archtables.o coreboot_table.o udelay_io.o
STAGE2_ARCH_X86_OBJ += pci_ops_auto.o pci_ops_conf1.o pci_ops_conf2.o
STAGE2_ARCH_X86_OBJ += keyboard.o i8259.o isa-dma.o

ifeq ($(CONFIG_PIRQ_TABLE),y)
STAGE2_ARCH_X86_OBJ += pirq_routing.o
endif

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

# To reduce code duplication, always make sure STAGE2_OBJ does not contain
# any object from STAGE0_OBJ.

STAGE2_OBJ_NEEDED = $(filter-out $(STAGE0_OBJ), $(STAGE2_OBJ))

$(obj)/coreboot.stage2 $(obj)/coreboot.stage2.map: $(obj)/stage0.o $(STAGE2_OBJ_NEEDED)
	$(Q)# leave a .o with full symbols in it for debugging.
	$(Q)printf "  LD      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(LD) -R $(obj)/stage0.o -Ttext 0x2000 --entry=stage2 \
		  -o $(obj)/coreboot.stage2 $(STAGE2_OBJ_NEEDED)
	$(Q)$(NM) $(obj)/coreboot.stage2 | sort -u > $(obj)/coreboot.stage2.map

#
# Build rules.
#

$(obj)/arch/x86/%.o: $(src)/arch/x86/%.c
	$(Q)mkdir -p $(dir $@)
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(INITCFLAGS) -c $< -o $@
	
# Building asm stub.
$(obj)/arch/x86/stage0%.o: $(src)/arch/x86/stage0%.S
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) -E $(COREBOOTINCLUDE) $< \
		-o $(obj)/arch/x86/stage0_asm.s -DBOOTBLK=0x1f00 \
		-DRESRVED=0xf0 -DDATE=\"`date +%Y/%m/%d`\"
	$(Q)printf "  AS      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(AS) $(obj)/arch/x86/stage0_asm.s -o $@

$(obj)/arch/x86/geodelx/stage0.o: $(src)/arch/x86/geodelx/stage0.S
	$(Q)mkdir -p $(dir $@)
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) -E $(COREBOOTINCLUDE) $< \
		-o $(obj)/arch/x86/stage0_asm.s -DBOOTBLK=0x1f00 \
		-DRESRVED=0xf0 -DDATE=\"`date +%Y/%m/%d`\"
	$(Q)printf "  AS      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(AS) $(obj)/arch/x86/stage0_asm.s -o $@


# NOTE HACK. Stefan will fix this :-)
$(obj)/arch/x86/amd/stage0.o: $(src)/arch/x86/amd/stage0.S
	$(Q)mkdir -p $(dir $@)
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) -E $(COREBOOTINCLUDE) $< \
		-I $(src)/include/arch/x86/amd/k8 \
		-o $(obj)/arch/x86/stage0_asm.s -DBOOTBLK=0x1f00 \
		-DRESRVED=0xf0 -DDATE=\"`date +%Y/%m/%d`\"
	$(Q)printf "  AS      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(AS) $(obj)/arch/x86/stage0_asm.s -o $@

$(obj)/coreboot.initram $(obj)/coreboot.initram.map: $(obj)/stage0.init $(obj)/stage0-prefixed.o $(INITRAM_OBJ)
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@)) (XIP)\n"
	$(Q)$(CC) $(INITCFLAGS) -D_SHARED -fPIE -c -combine $(INITRAM_OBJ) -o $(obj)/coreboot.initram_partiallylinked.o
	$(Q)# initram links against stage0
	$(Q)printf "  LD      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(LD) -Ttext 0 --entry main -N -R $(obj)/stage0-prefixed.o \
		$(obj)/coreboot.initram_partiallylinked.o -o $(obj)/coreboot.initram
	$(Q)printf "  NM      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(NM) $(obj)/coreboot.initram | sort -u > $(obj)/coreboot.initram.map

endif
