################################################################################
##
## This file is part of the coreboot project.
##
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
## Foundation, Inc.
##

################################################################################
# i386 specific tools
NVRAMTOOL:=$(objutil)/nvramtool/nvramtool

OPTION_TABLE_H:=
ifeq ($(CONFIG_HAVE_OPTION_TABLE),y)

cbfs-files-y += cmos_layout.bin
cmos_layout.bin-file = $(obj)/cmos_layout.bin
cmos_layout.bin-type = 0x01aa

$(obj)/cmos_layout.bin: $(NVRAMTOOL) $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout
	@printf "    OPTION     $(subst $(obj)/,,$(@))\n"
	$(NVRAMTOOL) -y $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout -L $@

OPTION_TABLE_H:=$(obj)/option_table.h

$(OPTION_TABLE_H): $(NVRAMTOOL) $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout
	@printf "    OPTION     $(subst $(obj)/,,$(@))\n"
	$(NVRAMTOOL) -y $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout -H $@
endif # CONFIG_HAVE_OPTION_TABLE

stripped_vgabios_id = $(call strip_quotes,$(CONFIG_VGA_BIOS_ID))
cbfs-files-$(CONFIG_VGA_BIOS) += pci$(stripped_vgabios_id).rom
pci$(stripped_vgabios_id).rom-file := $(call strip_quotes,$(CONFIG_VGA_BIOS_FILE))
pci$(stripped_vgabios_id).rom-type := optionrom

cbfs-files-$(CONFIG_INTEL_MBI) += mbi.bin
mbi.bin-file := $(call strip_quotes,$(CONFIG_MBI_FILE))
mbi.bin-type := mbi

ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32)$(CONFIG_ARCH_ROMSTAGE_X86_64),y)
CBFSTOOL_PRE1_OPTS = -m x86 -s $(CONFIG_ROM_SIZE) \
		     -o $$(( $(CONFIG_ROM_SIZE) - $(CONFIG_CBFS_SIZE) ))
# Make sure that segment for .car.data is ignored while adding romstage.
CBFSTOOL_PRE_OPTS  = -b $(shell cat $(objcbfs)/base_xip.txt) -S ".car.data"
endif

## Calculate the base address of CBFS for later comparisons
CBFS_BASE_ADDRESS=$(call int-add, $(call int-subtract, 0xffffffff $(CONFIG_CBFS_SIZE)) 1)

###############################################################################
# bootblock
###############################################################################

ifeq ($(CONFIG_ARCH_BOOTBLOCK_X86_32)$(CONFIG_ARCH_BOOTBLOCK_X86_64),y)

bootblock-srcs += $(src)/arch/x86/failover.ld
bootblock-srcs += $(src)/cpu/x86/16bit/entry16.ld
bootblock-srcs += $(src)/cpu/x86/16bit/reset16.ld
bootblock-srcs += $(src)/arch/x86/id.ld
ifeq ($(CONFIG_CPU_INTEL_FIRMWARE_INTERFACE_TABLE),y)
bootblock-srcs += $(src)/cpu/intel/fit/fit.ld
endif

# TODO: Why can't this use the real bootblock-y += xxx.S mechanism instead?
bootblock_inc = $(src)/arch/x86/prologue.inc
bootblock_inc += $(src)/cpu/x86/16bit/entry16.inc
bootblock_inc += $(src)/cpu/x86/16bit/reset16.inc
bootblock_inc += $(src)/cpu/x86/32bit/entry32.inc
bootblock_inc += $(src)/arch/x86/id.inc
ifeq ($(CONFIG_CPU_INTEL_FIRMWARE_INTERFACE_TABLE),y)
bootblock_inc += $(src)/cpu/intel/fit/fit.inc
endif
bootblock_inc += $(chipset_bootblock_inc)

ifeq ($(CONFIG_SSE),y)
bootblock_inc += $(src)/cpu/x86/sse_enable.inc
endif
bootblock_inc += $(objgenerated)/bootblock.inc
bootblock_inc += $(src)/arch/x86/walkcbfs.S

bootblock_romccflags := -mcpu=i386 -O2 -D__PRE_RAM__ -D__BOOTBLOCK__
ifeq ($(CONFIG_SSE),y)
bootblock_romccflags := -mcpu=k7 -msse -O2 -D__PRE_RAM__ -D__BOOTBLOCK__
endif

$(objgenerated)/bootblock.ld: $$(filter %.ld,$$(bootblock-objs))
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	cat $^ >> $@.tmp
	mv $@.tmp $@

$(objgenerated)/bootblock_inc.S: $$(bootblock_inc)
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	printf '$(foreach crt0,$(bootblock_inc),#include "$(crt0)"\n)' > $@

$(objgenerated)/bootblock.o: $(objgenerated)/bootblock.s
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_bootblock) $(CFLAGS_bootblock) -c -o $@ $<  > $(basename $@).disasm

$(objgenerated)/bootblock.s: $(objgenerated)/bootblock_inc.S $(obj)/config.h $(obj)/build.h
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_bootblock) -MMD -x assembler-with-cpp -E -I$(src)/include -I$(src)/arch/x86/include -I$(obj) -include $(obj)/build.h -include $(obj)/config.h -I. -I$(src) $< -o $@

$(objgenerated)/bootblock.inc: $(src)/arch/x86/$(subst ",,$(CONFIG_BOOTBLOCK_SOURCE)) $(objutil)/romcc/romcc $(OPTION_TABLE_H) $(KCONFIG_AUTOHEADER)
	@printf "    ROMCC      $(subst $(obj)/,,$(@))\n"
	$(CC_bootblock) $(CPPFLAGS_bootblock) -MM -MT$(objgenerated)/bootblock.inc \
		$< > $(objgenerated)/bootblock.inc.d
	$(ROMCC) -c -S $(bootblock_romccflags) -I. $(CPPFLAGS_bootblock) $< -o $@

$(objcbfs)/bootblock.debug: $(objgenerated)/bootblock.o $(objgenerated)/bootblock.ld
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_ARCH_BOOTBLOCK_X86_32),y)
	$(LD_bootblock) -m elf_i386 --oformat elf32-i386 -static -o $@ -L$(obj) $< -T $(objgenerated)/bootblock.ld
else
	$(LD_bootblock) -m elf_x86_64 --oformat elf64-x86-64 -static -o $@ -L$(obj) $< -T $(objgenerated)/bootblock.ld
endif


endif # CONFIG_ARCH_BOOTBLOCK_X86_32 / CONFIG_ARCH_BOOTBLOCK_X86_64

###############################################################################
# romstage
###############################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32)$(CONFIG_ARCH_ROMSTAGE_X86_64),y)

crt0s = $(src)/arch/x86/prologue.inc
romstage-srcs += $(src)/arch/x86/romstage.ld
crt0s += $(src)/cpu/x86/32bit/entry32.inc
romstage-srcs += $(src)/cpu/x86/32bit/entry32.ld

crt0s += $(src)/cpu/x86/fpu_enable.inc
ifeq ($(CONFIG_SSE),y)
crt0s += $(src)/cpu/x86/sse_enable.inc
endif

crt0s += $(cpu_incs)
crt0s += $(cpu_incs-y)

crt0s += $(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc

ifeq ($(CONFIG_ROMCC),y)
crt0s += $(src)/arch/x86/crt0_romcc_epilogue.inc
endif

ifeq ($(CONFIG_ROMCC),y)
ifeq ($(CONFIG_MMX),y)
	ifeq ($(CONFIG_SSE),y)
		ROMCCFLAGS := -mcpu=p4 -O2 # MMX, SSE
	else
		ROMCCFLAGS := -mcpu=p2 -O2 # MMX, !SSE
	endif
else
	ROMCCFLAGS := -mcpu=i386 -O2 # !MMX, !SSE
endif

$(objcbfs)/romstage%.bin: $(objcbfs)/romstage%.elf
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	$(OBJCOPY_romstage) -O binary $< $@

$(objcbfs)/romstage%.elf: $(objcbfs)/romstage%.debug
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	cp $< $@.tmp
	$(OBJCOPY_romstage) --strip-debug $@.tmp
	$(OBJCOPY_romstage) --add-gnu-debuglink=$< $@.tmp
	mv $@.tmp $@

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc: $(src)/mainboard/$(MAINBOARDDIR)/romstage.c $(objutil)/romcc/romcc $(OPTION_TABLE_H) $(obj)/build.h $(obj)/config.h
	printf "    ROMCC      romstage.inc\n"
	$(ROMCC) -c -S $(ROMCCFLAGS) -D__ROMSTAGE__ -D__PRE_RAM__ -I. $(CPPFLAGS_romstage) $< -o $@
else

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.pre.inc: $(src)/mainboard/$(MAINBOARDDIR)/romstage.c $(OPTION_TABLE_H) $(obj)/build.h $(obj)/config.h
	@printf "    CC         romstage.inc\n"
	$(CC_romstage) $(CPPFLAGS_romstage) $(CFLAGS_romstage) -MMD -D__ROMSTAGE__ -D__PRE_RAM__ -I$(src) -I. -I$(obj) -c -S $< -o $@

$(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc: $(obj)/mainboard/$(MAINBOARDDIR)/romstage.pre.inc
	@printf "    POST       romstage.inc\n"
	sed -e 's/\.rodata/.rom.data/g' -e 's/\^\.text/.section .rom.text/g' \
		-e 's/\^\.section \.text/.section .rom.text/g' $^ > $@.tmp
	mv $@.tmp $@
endif

romstage-srcs += $(objgenerated)/crt0.S

romstage-libs ?=

ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32),y)
romstage-oformat=elf32-i386
else
romstage-oformat=elf64-x86-64
endif

$(objcbfs)/romstage_null.debug: $$(romstage-objs) $(objgenerated)/romstage_null.ld $$(romstage-libs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_romstage) --gc-sections -nostdlib -nostartfiles -static -o $@ -L$(obj) $(COMPILER_RT_FLAGS_romstage) --whole-archive --start-group $(filter-out %.ld,$(romstage-objs)) $(romstage-libs) --no-whole-archive $(COMPILER_RT_romstage) --end-group -T $(objgenerated)/romstage_null.ld --oformat $(romstage-oformat)
	$(OBJCOPY_romstage) --only-section .illegal_globals $(@) $(objcbfs)/romstage_null.offenders && \
	$(NM_romstage) $(objcbfs)/romstage_null.offenders | grep -q ""; if [ $$? -eq 0 ]; then \
		echo "Forbidden global variables in romstage:"; \
		$(NM_romstage) $(objcbfs)/romstage_null.offenders; false; \
		else true; fi

$(objcbfs)/romstage.debug: $$(romstage-objs) $(objgenerated)/romstage.ld $$(romstage-libs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_romstage) --gc-sections -nostdlib -nostartfiles -static -o $@ -L$(obj) $(COMPILER_RT_FLAGS_romstage) --whole-archive --start-group $(filter-out %.ld,$(romstage-objs)) $(romstage-libs) --no-whole-archive $(COMPILER_RT_romstage) --end-group -T $(objgenerated)/romstage.ld --oformat $(romstage-oformat)

$(objgenerated)/romstage_null.ld: $$(filter %.ld,$$(romstage-objs))
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	rm -f $@
	printf "ROMSTAGE_BASE = 0x0;\n" > $@.tmp
	cat $^ >> $@.tmp
	mv $@.tmp $@

$(objgenerated)/romstage.ld: $(objgenerated)/romstage_null.ld $(objcbfs)/base_xip.txt
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	rm -f $@
	sed -e 's/^/ROMSTAGE_BASE = /g' -e 's/$$/;/g' $(objcbfs)/base_xip.txt > $@.tmp
	sed -e '/^ROMSTAGE_BASE/d' $(objgenerated)/romstage_null.ld >> $@.tmp
	mv $@.tmp $@

$(objcbfs)/base_xip.txt: $(obj)/coreboot.pre1 $(objcbfs)/romstage_null.bin
	rm -f $@
	$(CBFSTOOL) $(obj)/coreboot.pre1 locate -T -f $(objcbfs)/romstage_null.bin -n $(CONFIG_CBFS_PREFIX)/romstage -P $(CONFIG_XIP_ROM_SIZE) > $@.tmp \
	 || { echo "The romstage is larger than XIP size. Please expand the CONFIG_XIP_ROM_SIZE" ; exit 1; }
	mv $@.tmp $@

$(objgenerated)/crt0.S: $$(crt0s)
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	printf '$(foreach crt0,$(crt0s),#include "$(crt0)"\n)' > $@

# Compiling crt0 with -g seems to trigger https://sourceware.org/bugzilla/show_bug.cgi?id=6428
romstage-S-ccopts += -I. -g0

endif # CONFIG_ARCH_ROMSTAGE_X86_32 / CONFIG_ARCH_ROMSTAGE_X86_64

###############################################################################
# ramstage
###############################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32)$(CONFIG_ARCH_RAMSTAGE_X86_64),y)

ifeq ($(CONFIG_COMPILE_IN_DSDT),y)
ramstage-srcs += $(obj)/dsdt.aml

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32),y)
target-objcopy=-O elf32-i386 -B i386
endif
ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_64),y)
target-objcopy=-O elf64-x86_64 -B x86_64
endif

define ramstage-objs_aml_template
$(call src-to-obj,ramstage,$(1).aml): $(1).aml
	@printf "    OBJCOPY    $$(subst $$(obj)/,,$$(@))\n"
	cd $$(dir $$@) && $$(OBJCOPY_ramstage) -I binary $$(target-objcopy) $$(notdir $$<) $$(notdir $$@)
endef
endif

ramstage-srcs += $(wildcard src/mainboard/$(MAINBOARDDIR)/mainboard.c)
ifeq ($(CONFIG_GENERATE_MP_TABLE),y)
ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/mptable.c),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/mptable.c
endif
endif
ifeq ($(CONFIG_GENERATE_PIRQ_TABLE),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/irq_tables.c
endif
ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/reset.c),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/reset.c
endif
ifeq ($(CONFIG_HAVE_ACPI_TABLES),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/acpi_tables.c
$(eval $(call asl_template,dsdt))
ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/fadt.c),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/fadt.c
endif
endif # CONFIG_GENERATE_ACPI_TABLES
ifeq ($(CONFIG_HAVE_SMI_HANDLER),y)
ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/smihandler.c),)
smm-srcs += src/mainboard/$(MAINBOARDDIR)/smihandler.c
endif
endif
ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/get_bus_conf.c),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/get_bus_conf.c
endif

ramstage-libs ?=

ifeq ($(CONFIG_RELOCATABLE_RAMSTAGE),y)

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32),y)
$(eval $(call rmodule_link,$(objcbfs)/ramstage.debug, $(objgenerated)/ramstage.o, $(CONFIG_HEAP_SIZE),x86_32))
else
$(eval $(call rmodule_link,$(objcbfs)/ramstage.debug, $(objgenerated)/ramstage.o, $(CONFIG_HEAP_SIZE),x86_64))
endif

# The rmodule_link defintion creates an elf file with .rmod extension.
$(objcbfs)/ramstage.elf: $(objcbfs)/ramstage.debug.rmod
	cp $< $@

else

ramstage-srcs += $(src)/arch/x86/ramstage.ld

$(objcbfs)/ramstage.debug: $(objgenerated)/ramstage.o $(obj)/arch/x86/ramstage.ramstage.ld
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(LD_ramstage) $(CPPFLAGS) --gc-sections -o $@ -L$(obj) $< -T $(obj)/arch/x86/ramstage.ramstage.ld

endif

$(objgenerated)/ramstage.o: $$(ramstage-objs) $(COMPILER_RT_ramstage) $$(ramstage-libs)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32),y)
	$(LD_ramstage) -m elf_i386 -r -o $@ $(COMPILER_RT_FLAGS_ramstage) --whole-archive --start-group $(filter-out %.ld,$(ramstage-objs)) $(ramstage-libs) --no-whole-archive $(COMPILER_RT_ramstage) --end-group
else
	$(LD_ramstage) -m elf_x86_64 -r -o $@ $(COMPILER_RT_FLAGS_ramstage) --whole-archive --start-group $(filter-out %.ld,$(ramstage-objs)) $(ramstage-libs) --no-whole-archive $(COMPILER_RT_ramstage) --end-group
endif

endif # CONFIG_ARCH_RAMSTAGE_X86_32 / CONFIG_ARCH_RAMSTAGE_X86_64


ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32)$(CONFIG_ARCH_ROMSTAGE_X86_64),y)

romstage-y += cbmem.c
romstage-y += boot.c

romstage-y += cbfs_and_run.c
romstage-$(CONFIG_ARCH_RAMSTAGE_X86_32) += cpu_common.c
romstage-y += memset.c
romstage-y += memcpy.c
romstage-y += memmove.c
romstage-y += mmap_boot.c

romstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

endif # CONFIG_ARCH_ROMSTAGE_X86_32 / CONFIG_ARCH_ROMSTAGE_X86_64

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32)$(CONFIG_ARCH_RAMSTAGE_X86_64),y)

ramstage-y += boot.c
ramstage-y += gdt.c
ramstage-y += tables.c
ramstage-y += cbmem.c
ramstage-$(CONFIG_GENERATE_MP_TABLE) += mpspec.c
ramstage-$(CONFIG_GENERATE_PIRQ_TABLE) += pirq_routing.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-$(CONFIG_GENERATE_SMBIOS_TABLES) += smbios.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpigen.c
ramstage-$(CONFIG_HAVE_ACPI_RESUME) += wakeup.S

ramstage-y += c_start.S
ramstage-y += cpu.c
ramstage-y += cpu_common.c
ramstage-y += pci_ops_conf1.c
ramstage-$(CONFIG_MMCONF_SUPPORT) += pci_ops_mmconf.c
ramstage-y += exception.c
ramstage-$(CONFIG_IOAPIC) += ioapic.c
ramstage-y += memset.c
ramstage-y += memcpy.c
ramstage-y += memmove.c
ramstage-y += ebda.c
ramstage-y += mmap_boot.c
ramstage-$(CONFIG_COOP_MULTITASKING) += thread.c
ramstage-$(CONFIG_COOP_MULTITASKING) += thread_switch.S
ramstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

smm-y += memset.c
smm-y += memcpy.c
smm-y += memmove.c
smm-y += mmap_boot.c

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32),y)
rmodules_x86_32-y += memset.c
rmodules_x86_32-y += memcpy.c
rmodules_x86_32-y += memmove.c
else
rmodules_x86_64-y += memset.c
rmodules_x86_64-y += memcpy.c
rmodules_x86_64-y += memmove.c
endif

endif # CONFIG_ARCH_RAMSTAGE_X86_32 / CONFIG_ARCH_RAMSTAGE_X86_64
