##
## This file is part of the coreboot project.
##
## Copyright (C) 2006-2007 coresystems GmbH
## (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
## Copyright (C) 2007-2008 Carl-Daniel Hailfinger
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
		-I$(obj) -I$(src)/mainboard/$(MAINBOARDDIR)/-fno-builtin  

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


STAGE0_LIB_SRC       = uart8250.c mem.c lar.c delay.c vtxprintf.c \
		       vsprintf.c console.c string.c $(DECOMPRESSORS)
STAGE0_ARCH_X86_SRC  = stage1.c serial.c \
		       udelay_io.c mc146818rtc.c post_code.c \
			pci_ops_conf1.c 
# speaker.c \

ifeq ($(CONFIG_PAYLOAD_ELF_LOADER),y)
STAGE0_LIB_SRC      += elfboot.c
STAGE0_ARCH_X86_SRC += archelfboot.c
endif

ifeq ($(CONFIG_CPU_I586),y)
	STAGE0_CAR_OBJ = i586/stage0.o
else
ifeq ($(CONFIG_CPU_AMD_GEODELX),y)
	STAGE0_CAR_OBJ = geodelx/stage0.o
	STAGE0_ARCH_X86_SRC += geodelx/stage1.c
	STAGE0_ARCH_X86_SRC += ../../northbridge/amd/geodelx/geodelxinit.c
else
ifeq ($(CONFIG_CPU_AMD_K8),y)
	STAGE0_CAR_OBJ = amd/stage0.o
	STAGE0_ARCH_X86_SRC += amd/k8/stage1.c
else
ifeq ($(CONFIG_CPU_VIA_C7),y)
	STAGE0_CAR_OBJ = via/stage0.o
endif
endif
endif
endif

ifeq ($(CONFIG_PAYLOAD_NO_PREPARSE_ELF), y)
	PARSEELF =
else
	PARSEELF = -e
endif

ifeq ($(CONFIG_WHOLE_PROGRAM_COMPILE), y)
	COMBINEFLAGS = -fwhole-program
else
	COMBINEFLAGS =
endif

STAGE0_SRC := $(patsubst %,$(src)/lib/%,$(STAGE0_LIB_SRC)) \
	      $(patsubst %,$(src)/arch/x86/%,$(STAGE0_ARCH_X86_SRC)) \
	      $(STAGE0_MAINBOARD_SRC) $(STAGE0_CHIPSET_SRC)

STAGE0_OBJ := $(patsubst $(src)/%.c,$(obj)/%.o,$(STAGE0_SRC)) \
	      $(patsubst %,$(obj)/arch/x86/%,$(STAGE0_CAR_OBJ))

$(obj)/stage0.o $(obj)/stage0.init $(obj)/stage0-prefixed.o: $(STAGE0_OBJ)
	$(Q)# We need to be careful. If stage0.o gets bigger than
	$(Q)# 0x4000 - 0x100, we will end up with a 4 gig file.
	$(Q)# I wonder if that behavior is on purpose.

	$(Q)# .data and .bss must be empty because they aren't handled
	$(Q)printf "  CHECK   stage0 (non-empty writable/allocatable sections)\n"
	$(Q)./util/sectionchecker/sectionchecker $(OBJDUMP) $(READELF) $(STAGE0_OBJ)

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

STAGE2_LIB_SRC       = stage2.c clog2.c mem.c tables.c delay.c \
		       compute_ip_checksum.c string.c

STAGE2_ARCH_X86_SRC  = archtables.c coreboot_table.c multiboot.c udelay_io.c
STAGE2_ARCH_X86_SRC += pci_ops_auto.c
STAGE2_ARCH_X86_SRC += keyboard.c i8259.c isa-dma.c

ifeq ($(CONFIG_PIRQ_TABLE),y)
STAGE2_ARCH_X86_SRC += pirq_routing.c
endif

STAGE2_DYNAMIC_SRC   = $(obj)/mainboard/$(MAINBOARDDIR)/statictree.c

STAGE2_SRC := $(patsubst %,$(src)/lib/%,$(STAGE2_LIB_SRC)) \
	      $(patsubst %,$(src)/arch/x86/%,$(STAGE2_ARCH_X86_SRC)) \
	      $(patsubst %,$(src)/device/%,$(STAGE2_DEVICE_SRC)) \
	      $(patsubst %,$(src)/mainboard/$(MAINBOARDDIR)/%,$(STAGE2_MAINBOARD_SRC))

STAGE2_SRC += $(STAGE2_CHIPSET_SRC)

STAGE2_OBJ := $(patsubst $(src)/%.c,$(obj)/%.o,$(STAGE2_SRC))

# This one is special because the static tree object ends up in the mainboard
# dir of the object tree.
STAGE2_OBJ += $(patsubst $(obj)/%.c,$(obj)/%.o,$(STAGE2_DYNAMIC_SRC))

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

$(obj)/coreboot.stage2 $(obj)/coreboot.stage2.map: $(obj)/stage0.o $(STAGE2_OBJ_NEEDED) $(STAGE2_SRC)
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
$(obj)/arch/x86/%/stage0.o: $(src)/arch/x86/%/stage0.S
	$(Q)mkdir -p $(dir $@)
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) -E $(COREBOOTINCLUDE) $< \
		-o $(obj)/arch/x86/stage0_asm.s -DBOOTBLK=0x1f00 \
		-DRESRVED=0xf0 -DDATE=\"`date +%Y/%m/%d`\"
	$(Q)printf "  AS      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(AS) $(obj)/arch/x86/stage0_asm.s -o $@

$(obj)/coreboot.initram $(obj)/coreboot.initram.map: $(obj)/stage0.init $(obj)/stage0-prefixed.o $(INITRAM_SRC)
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@)) (XIP)\n"
	$(Q)$(CC) $(INITCFLAGS) -fPIE -c -combine $(COMBINEFLAGS) $(INITRAM_SRC) -o $(obj)/coreboot.initram_partiallylinked.o

	$(Q)# .data and .bss must be empty because they aren't handled
	$(Q)printf "  CHECK   initram (non-empty writable/allocatable sections)\n"
	$(Q)./util/sectionchecker/sectionchecker $(OBJDUMP) $(READELF) $(obj)/coreboot.initram_partiallylinked.o

	$(Q)printf "  WRAP    $(subst $(shell pwd)/,,$(@)) (PIC->non-PIC)\n"
	$(Q)$(NM) --undefined-only $(obj)/coreboot.initram_partiallylinked.o |\
		grep -v _GLOBAL_OFFSET_TABLE_ | grep " U " | sed "s/^ *U //" |\
		$(src)/util/picwrapper/picwrapper > $(obj)/initram_picwrapper.s
	$(Q)printf "  AS      initram_picwrapper.s\n"
	$(Q)$(AS) $(obj)/initram_picwrapper.s -o $(obj)/initram_picwrapper.o
	$(Q)# initram links against stage0
	$(Q)printf "  LD      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(LD) -Ttext 0 --entry main -N -R $(obj)/stage0-prefixed.o \
		$(obj)/coreboot.initram_partiallylinked.o \
		$(obj)/initram_picwrapper.o -o $(obj)/coreboot.initram
	$(Q)printf "  NM      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(NM) $(obj)/coreboot.initram | sort -u > $(obj)/coreboot.initram.map

endif
