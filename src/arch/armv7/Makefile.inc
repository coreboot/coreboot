################################################################################
##
## This file is part of the coreboot project.
##
## Copyright (C) 2012 The ChromiumOS Authors
## Copyright (C) 2012 Alexandru Gagniuc <mr.nuke.me@gmail.com>
## Copyright (C) 2009-2010 coresystems GmbH
## Copyright (C) 2009 Ronald G. Minnich
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
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
################################################################################

# Take care of subdirectories
# subdirs-y += boot
# subdirs-y += init
subdirs-y += lib
# subdirs-y += smp

################################################################################
# Build the final rom image
COREBOOT_ROM_DEPENDENCIES:=
ifeq ($(CONFIG_PAYLOAD_ELF),y)
COREBOOT_ROM_DEPENDENCIES+=$(CONFIG_PAYLOAD_FILE)
endif
#ifeq ($(CONFIG_AP_CODE_IN_CAR),y)
#COREBOOT_ROM_DEPENDENCIES+=$(objcbfs)/coreboot_ap.elf
#endif

extract_nth=$(word $(1), $(subst |, ,$(2)))

ifneq ($(CONFIG_UPDATE_IMAGE),y)
prebuild-files = \
	$(foreach file,$(cbfs-files), \
	$(CBFSTOOL) $@.tmp \
	add$(if $(filter stage,$(call extract_nth,3,$(file))),-stage)$(if $(filter payload,$(call extract_nth,3,$(file))),-payload) \
	$(call extract_nth,1,$(file)) \
	$(call extract_nth,2,$(file)) $(if $(filter-out stage payload,$(call extract_nth,3,$(file))),$(call extract_nth,3,$(file))) \
	$(call extract_nth,4,$(file)) &&)
prebuilt-files = $(foreach file,$(cbfs-files), $(call extract_nth,1,$(file)))

$(obj)/coreboot.pre1: $(objcbfs)/bootblock.bin $$(prebuilt-files) $(CBFSTOOL)
	$(CBFSTOOL) $@.tmp create arm $(CONFIG_COREBOOT_ROMSIZE_KB)K $(objcbfs)/bootblock.bin 64 $$(( $(CONFIG_ROM_SIZE) - $(CONFIG_CBFS_SIZE) ))
	$(prebuild-files) true
	mv $@.tmp $@
else
.PHONY: $(obj)/coreboot.pre1
$(obj)/coreboot.pre1: $(CBFSTOOL)
	mv $(obj)/coreboot.rom $@
endif

$(obj)/coreboot.rom: $(obj)/coreboot.pre $(objcbfs)/coreboot_ram.elf $(CBFSTOOL) $(call strip_quotes,$(COREBOOT_ROM_DEPENDENCIES))
	@printf "    CBFS       $(subst $(obj)/,,$(@))\n"
	cp $(obj)/coreboot.pre $@.tmp
	if [ -f $(objcbfs)/coreboot_ap.elf ]; \
	then \
		$(CBFSTOOL) $@.tmp add-stage $(objcbfs)/coreboot_ap.elf $(CONFIG_CBFS_PREFIX)/coreboot_ap $(CBFS_COMPRESS_FLAG); \
	fi
	$(CBFSTOOL) $@.tmp add-stage $(objcbfs)/coreboot_ram.elf $(CONFIG_CBFS_PREFIX)/coreboot_ram $(CBFS_COMPRESS_FLAG)
ifeq ($(CONFIG_PAYLOAD_NONE),y)
	@printf "    PAYLOAD    \e[1;31mnone (as specified by user)\e[0m\n"
endif
ifeq ($(CONFIG_PAYLOAD_ELF),y)
	@printf "    PAYLOAD    $(CONFIG_PAYLOAD_FILE) (compression: $(CBFS_PAYLOAD_COMPRESS_NAME))\n"
	$(CBFSTOOL) $@.tmp add-payload $(CONFIG_PAYLOAD_FILE) $(CONFIG_CBFS_PREFIX)/payload $(CBFS_PAYLOAD_COMPRESS_FLAG)
endif
ifeq ($(CONFIG_INCLUDE_CONFIG_FILE),y)
	@printf "    CONFIG     $(DOTCONFIG)\n"
	if [ -f $(DOTCONFIG) ]; then \
	echo "# This image was built using git revision" `git rev-parse HEAD` > $(obj)/config.tmp ; \
	sed -e '/^#/d' -e '/^ *$$/d' $(DOTCONFIG) >> $(obj)/config.tmp ; \
	$(CBFSTOOL) $@.tmp add $(obj)/config.tmp config raw; rm -f $(obj)/config.tmp ; fi
endif
	mv $@.tmp $@
	@printf "    CBFSPRINT  $(subst $(obj)/,,$(@))\n\n"
	$(CBFSTOOL) $@ print

bootsplash.jpg-file := $(call strip_quotes,$(CONFIG_BOOTSPLASH_FILE))
bootsplash.jpg-type := bootsplash

################################################################################
# armv7 specific tools

################################################################################
# Common recipes for all stages

$(objcbfs)/%.bin: $(objcbfs)/%.elf
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	$(OBJCOPY) -O binary $< $@

$(objcbfs)/%.elf: $(objcbfs)/%.debug
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	cp $< $@.tmp
	$(NM) -n $@.tmp | sort > $(basename $@).map
	$(OBJCOPY) --strip-debug $@.tmp
	$(OBJCOPY) --add-gnu-debuglink=$< $@.tmp
	mv $@.tmp $@

################################################################################
# Build the coreboot_ram (stage 2)

$(objcbfs)/coreboot_ram.debug: $(objgenerated)/coreboot_ram.o $(src)/arch/x86/coreboot_ram.ld
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD) -m elf_i386 -o $@ -L$(obj) $< -T $(src)/arch/x86/coreboot_ram.ld
else
	$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(src)/arch/x86/coreboot_ram.ld $<
endif

$(objgenerated)/coreboot_ram.o: $(obj)/arch/x86/lib/c_start.ramstage.o $$(driver-objs) $(objgenerated)/ramstage.a $(LIBGCC_FILE_NAME)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD) -m elf_i386 -r -o $@ $(obj)/arch/x86/lib/c_start.ramstage.o $(driver-objs) --wrap __divdi3 --wrap __udivdi3 --wrap __moddi3 --wrap __umoddi3 --start-group $(objgenerated)/ramstage.a $(LIBGCC_FILE_NAME) --end-group
else
	$(CC) -nostdlib -r -o $@ $(obj)/arch/x86/lib/c_start.ramstage.o $(driver-objs) -Wl,--wrap,__divdi3 -Wl,--wrap,__udivdi3 -Wl,--wrap,__moddi3 -Wl,--wrap,__umoddi3 -Wl,--start-group $(objgenerated)/ramstage.a $(LIBGCC_FILE_NAME) -Wl,--end-group
endif

$(objgenerated)/ramstage.a: $$(ramstage-objs)
	@printf "    AR         $(subst $(obj)/,,$(@))\n"
	rm -f $@
	$(AR) cr $@ $^

################################################################################
# Ramstage for AP CPU (AMD K8, obsolete?)

#$(objcbfs)/coreboot_ap.debug: $(objgenerated)/coreboot_ap.o $(src)/arch/x86/init/ldscript_apc.lb
#	@printf "    CC         $(subst $(obj)/,,$(@))\n"
#	$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(src)/arch/x86/init/ldscript_apc.lb $<

#$(objgenerated)/coreboot_ap.o: $(src)/mainboard/$(MAINBOARDDIR)/ap_romstage.c $(OPTION_TABLE_H)
#	@printf "    CC         $(subst $(obj)/,,$(@))\n"
#	$(CC) -MMD $(CFLAGS) -I$(src) -D__PRE_RAM__ -I. -I$(obj) -c $< -o $@

################################################################################
# done

# For various headers imported from Linux
CFLAGS += -D__KERNEL__
CFLAGS += -D__LINUX_ARM_ARCH__=7
INCLUDES += -Isrc/include/linux
INCLUDES += -Isrc/include/linux/uapi

crt0s = $(src)/arch/armv7/start.S
ldscripts =
ldscripts += $(src)/arch/armv7/romstage.ld

#crt0s += $(src)/cpu/x86/fpu_enable.inc
# FIXME: CONFIG_NEON or something similar for ARM?
#ifeq ($(CONFIG_SSE),y)
#crt0s += $(src)/cpu/x86/sse_enable.inc
#endif

crt0s += $(cpu_incs)
crt0s += $(cpu_incs-y)

ifeq ($(CONFIG_LLSHELL),y)
crt0s += $(src)/arch/x86/llshell/llshell.inc
endif

crt0s += $(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.pre.inc: $(src)/mainboard/$(MAINBOARDDIR)/romstage.c $(OPTION_TABLE_H) $(obj)/build.h $(obj)/config.h
	@printf "    CC         romstage.inc\n"
	$(CC) -MMD $(CFLAGS) -D__PRE_RAM__ -I$(src) -I. -I$(obj) -c -S $< -o $@

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc: $(obj)/mainboard/$(MAINBOARDDIR)/romstage.pre.inc
	@printf "    POST       romstage.inc\n"
	sed -e 's/\.rodata/.rom.data/g' -e 's/\^\.text/.section .rom.text/g' \
		-e 's/\^\.section \.text/.section .rom.text/g' $^ > $@.tmp
	mv $@.tmp $@

# Things that appear in every board
romstage-srcs += $(objgenerated)/crt0.s
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/mainboard.c
ifeq ($(CONFIG_GENERATE_PIRQ_TABLE),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/irq_tables.c
endif
ifeq ($(CONFIG_BOARD_HAS_HARD_RESET),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/reset.c
endif
ifeq ($(CONFIG_GENERATE_ACPI_TABLES),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/acpi_tables.c
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/dsdt.asl
# make doesn't have arithmetic operators or greater-than comparisons
ifeq ($(subst 5,4,$(CONFIG_ACPI_SSDTX_NUM)),4)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/ssdt2.asl
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/ssdt3.asl
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/ssdt4.asl
endif
ifeq ($(CONFIG_ACPI_SSDTX_NUM),5)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/ssdt5.asl
endif
ifeq ($(CONFIG_BOARD_HAS_FADT),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/fadt.c
endif
endif

ifeq ($(CONFIG_HAVE_BUS_CONFIG),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/get_bus_conf.c
endif

################################################################################
# Build the final rom image

$(obj)/coreboot.pre: $(objcbfs)/romstage_xip.elf $(obj)/coreboot.pre1 $(CBFSTOOL)
	@printf "    CBFS       $(subst $(obj)/,,$(@))\n"
	cp $(obj)/coreboot.pre1 $@.tmp
	$(CBFSTOOL) $@.tmp add-stage $(objcbfs)/romstage_xip.elf \
		$(CONFIG_CBFS_PREFIX)/romstage x $(shell cat $(objcbfs)/base_xip.txt)
	mv $@.tmp $@

################################################################################
# Build the bootblock

#bootblock_lds = $(src)/arch/armv7/ldscript_fallback_cbfs.lb
bootblock_lds = $(src)/arch/armv7/lib/id.lds
#bootblock_lds = $(src)/arch/armv7/romstage.ld
bootblock_lds += $(chipset_bootblock_lds)

bootblock_inc += $(src)/arch/armv7/lib/id.inc
bootblock_inc += $(chipset_bootblock_inc)

# FIXME: CONFIG_NEON or something similar for ARM?
#ifeq ($(CONFIG_SSE),y)
#bootblock_inc += $(src)/cpu/x86/sse_enable.inc
#endif
#bootblock_inc += $(objgenerated)/bootblock.inc

$(objgenerated)/bootblock.ld: $$(bootblock_lds) $(obj)/ldoptions
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	printf '$(foreach ldscript,ldoptions $(bootblock_lds),INCLUDE "$(ldscript)"\n)' > $@

$(objgenerated)/bootblock_inc.S: $$(bootblock_inc)
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	printf '$(foreach crt0,$(bootblock_inc),#include "$(crt0)"\n)' > $@

$(objgenerated)/bootblock.o: $(objgenerated)/bootblock.s
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -Wa,-acdlns -c -o $@ $<  > $(basename $@).disasm

$(objgenerated)/bootblock.s: $(objgenerated)/bootblock_inc.S $(obj)/config.h $(obj)/build.h
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -MMD -x assembler-with-cpp -E -I$(src)/include -I$(src)/arch/x86/include -I$(obj) -include $(obj)/build.h -include $(obj)/config.h -I. -I$(src) $< -o $@

#$(objgenerated)/bootblock.inc: $(src)/arch/x86/init/$(subst ",,$(CONFIG_BOOTBLOCK_SOURCE)) $(objutil)/romcc/romcc $(OPTION_TABLE_H)
#	@printf "    ROMCC      $(subst $(obj)/,,$(@))\n"
#	$(CC) $(INCLUDES) -MM -MT$(objgenerated)/bootblock.inc \
#		$< > $(objgenerated)/bootblock.inc.d
#	$(ROMCC) -c -S $(bootblock_romccflags) $(ROMCCFLAGS) -I. $(INCLUDES) $< -o $@

$(objcbfs)/bootblock.debug: $(objgenerated)/bootblock.o $(objgenerated)/bootblock.ld
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD) -m elf_i386 -static -o $@.tmp -L$(obj) $< -T $(objgenerated)/bootblock.ld
else
	$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(objgenerated)/bootblock.ld $<
endif

################################################################################
# Build the romstage

# FIXME(dhendrix): added debug printf's
$(objcbfs)/romstage_null.debug: $$(romstage-objs) $(objgenerated)/romstage_null.ld
	@printf "    FOO\n"
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD) -nostdlib -nostartfiles -static -o $@ -L$(obj) $(romstage-objs) -T $(objgenerated)/romstage_null.ld
else
	$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(objgenerated)/romstage_null.ld $(romstage-objs)
endif
	@printf "    BAR\n"

$(objcbfs)/romstage_xip.debug: $$(romstage-objs) $(objgenerated)/romstage_xip.ld
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
	$(LD) -nostdlib -nostartfiles -static -o $@ -L$(obj) $(romstage-objs) -T $(objgenerated)/romstage_xip.ld
else
	$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(objgenerated)/romstage_xip.ld $(romstage-objs)
endif

$(objgenerated)/romstage_null.ld: $$(ldscripts) $(obj)/ldoptions
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	rm -f $@
	printf "ROMSTAGE_BASE = 0x0;\n" > $@.tmp
	printf '$(foreach ldscript,ldoptions $(ldscripts),INCLUDE "$(ldscript:$(obj)/%=%)"\n)' >> $@.tmp
	mv $@.tmp $@

$(objgenerated)/romstage_xip.ld: $(objgenerated)/romstage_null.ld $(objcbfs)/base_xip.txt
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	rm -f $@
	sed -e 's/^/ROMSTAGE_BASE = /g' -e 's/$$/;/g' $(objcbfs)/base_xip.txt > $@.tmp
	sed -e '/ROMSTAGE_BASE/d' $(objgenerated)/romstage_null.ld >> $@.tmp
	mv $@.tmp $@

$(objcbfs)/base_xip.txt: $(obj)/coreboot.pre1 $(objcbfs)/romstage_null.bin
	@printf "    generating base_xip.txt\n"
	rm -f $@
	$(CBFSTOOL) $(obj)/coreboot.pre1 locate $(objcbfs)/romstage_null.bin $(CONFIG_CBFS_PREFIX)/romstage $(CONFIG_XIP_ROM_SIZE) > $@.tmp \
	 || { echo "The romstage is larger than XIP size. Please expand the CONFIG_XIP_ROM_SIZE" ; exit 1; }
	sed -e 's/^/0x/g' $@.tmp > $@.tmp2
	rm $@.tmp
	mv $@.tmp2 $@

$(objgenerated)/crt0.romstage.S: $$(crt0s)
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	printf '$(foreach crt0,$(crt0s),#include "$(crt0:$(obj)/%=%)"\n)' > $@

$(objgenerated)/crt0.romstage.o: $(objgenerated)/crt0.s
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -Wa,-acdlns -c -o $@ $<  > $(basename $@).disasm

$(objgenerated)/crt0.s: $(objgenerated)/crt0.romstage.S $(obj)/config.h $(obj)/build.h
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -MMD -x assembler-with-cpp -E -I$(src)/include -I$(src)/arch/armv7/include -I$(obj) -include $(obj)/config.h -include $(obj)/build.h -I. -I$(src) $< -o $@

