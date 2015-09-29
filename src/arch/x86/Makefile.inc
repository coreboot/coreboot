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
# Use a '-a 64' option to cbfstool locate to provide a minimum alignment
# requirement for the overall romstage. While the first object within
# romstage could have a 4 byte minimum alignment that doesn't mean the linker
# won't decide the entire section should be aligned to a larger value. In the
# future cbfstool should add XIP files proper and honor the alignment
# requirements of the program segment.
#
# Make sure that segment for .car.data is ignored while adding romstage.
CBFSTOOL_PRE_OPTS  = -a 64 --xip -S ".car.data" -P $(CONFIG_XIP_ROM_SIZE)
endif

## Calculate the base address of CBFS for later comparisons
CBFS_BASE_ADDRESS=$(call int-add, $(call int-subtract, 0xffffffff $(CONFIG_CBFS_SIZE)) 1)

###############################################################################
# bootblock
###############################################################################

ifeq ($(CONFIG_ARCH_BOOTBLOCK_X86_32)$(CONFIG_ARCH_BOOTBLOCK_X86_64),y)

# x86-specific linker flags
ifeq ($(CONFIG_ARCH_BOOTBLOCK_X86_32),y)
LDFLAGS_bootblock += -m elf_i386 --oformat elf32-i386
else
LDFLAGS_bootblock += -m elf_x86_64 --oformat elf64-x86-64
endif

# Add the assembly file that pulls in the rest of the dependencies in
# the right order. Make sure the auto generated bootblock.inc is a proper
# dependency. Make the same true for the linker sript.
bootblock-y += id.S
bootblock-y += bootblock.S
bootblock-y += walkcbfs.S
$(obj)/arch/x86/bootblock.bootblock.o: $(objgenerated)/bootblock.inc

bootblock-y += bootblock.ld
$(obj)/arch/x86/bootblock.bootblock.ld: $(objgenerated)/bootblock.ld

bootblock_romccflags := -mcpu=i386 -O2 -D__PRE_RAM__ -D__BOOTBLOCK__
ifeq ($(CONFIG_SSE),y)
bootblock_romccflags := -mcpu=k7 -msse -O2 -D__PRE_RAM__ -D__BOOTBLOCK__
endif

# This is a hack in case there are no per chipset linker files.
$(objgenerated)/empty:
	touch $@

$(objgenerated)/bootblock.ld: $$(filter-out $(obj)/arch/x86/bootblock.bootblock.ld, $$(filter %.ld,$$(bootblock-objs))) $(objgenerated)/empty
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	cat $^ >> $@.tmp
	mv $@.tmp $@

$(objgenerated)/bootblock.inc: $(src)/arch/x86/$(subst ",,$(CONFIG_BOOTBLOCK_SOURCE)) $(objutil)/romcc/romcc $(OPTION_TABLE_H) $(KCONFIG_AUTOHEADER)
	@printf "    ROMCC      $(subst $(obj)/,,$(@))\n"
	$(CC_bootblock) $(CPPFLAGS_bootblock) -MM -MT$(objgenerated)/bootblock.inc \
		$< > $(objgenerated)/bootblock.inc.d
	$(ROMCC) -c -S $(bootblock_romccflags) -I. $(CPPFLAGS_bootblock) $< -o $@

# $(obj)/arch/x86/bootblock.bootblock.ld is part of $(bootblock-objs)
$(objcbfs)/bootblock.debug: $$(bootblock-objs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_bootblock) $(LDFLAGS_bootblock) -o $@ -L$(obj) \
		$(filter-out %.ld,$(bootblock-objs)) \
		-T $(obj)/arch/x86/bootblock.bootblock.ld


endif # CONFIG_ARCH_BOOTBLOCK_X86_32 / CONFIG_ARCH_BOOTBLOCK_X86_64

###############################################################################
# common support for early assembly includes
###############################################################################

# Chipset specific assembly stubs in the romstage program flow. Certain
# boards have more than one assembly stub so collect those and put them
# into a single generated file.
crt0s = $(cpu_incs-y)

$(objgenerated)/assembly.inc: $$(crt0s)
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	printf '$(foreach crt0,$(crt0s),#include "$(crt0)"\n)' > $@

define early_x86_stage
# $1 stage name
# $2 oformat
$(1)-y += memlayout.ld
# Add the assembly file that pulls in the rest of the dependencies in
# the right order. Make sure the auto generated assembly.inc is a proper
# dependency.
$(1)-y += assembly_entry.S
$$(obj)/arch/x86/assembly_entry.$(1).o: $(objgenerated)/assembly.inc

# The '.' include path is needed for the generated assembly.inc file.
$(1)-S-ccopts += -I.

$$(objcbfs)/$(1).debug: $$$$($(1)-libs) $$$$($(1)-objs)
	@printf "    LINK       $$(subst $$(obj)/,,$$(@))\n"
	$$(LD_$(1)) $$(LDFLAGS_$(1)) -o $$@ -L$$(obj) $$(COMPILER_RT_FLAGS_$(1)) --whole-archive --start-group $$(filter-out %.ld,$$($(1)-objs)) $$($(1)-libs) --no-whole-archive $$(COMPILER_RT_$(1)) --end-group -T $$(obj)/arch/x86/memlayout.$(1).ld --oformat $(2)
	LANG=C LC_ALL= $$(OBJCOPY_$(1)) --only-section .illegal_globals $$(@) $$(objcbfs)/$(1)_null.offenders 2>&1 | \
	grep -v "Empty loadable segment detected" && \
	$$(NM_$(1)) $$(objcbfs)/$(1)_null.offenders | grep -q ""; if [ $$$$? -eq 0 ]; then \
		echo "Forbidden global variables in "$(1)":"; \
		$$(NM_$(1)) $$(objcbfs)/$(1)_null.offenders; false; \
		else true; fi
endef

###############################################################################
# verstage
###############################################################################

ifeq ($(CONFIG_ARCH_VERSTAGE_X86_32)$(CONFIG_ARCH_VERSTAGE_X86_64),y)

verstage-y += boot.c

verstage-$(CONFIG_ARCH_RAMSTAGE_X86_32) += cpu_common.c
verstage-y += memset.c
verstage-y += memcpy.c
verstage-y += memmove.c
verstage-y += mmap_boot.c

verstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

verstage-libs += $(objgenerated)/libverstage.a

ifeq ($(CONFIG_ARCH_VERSTAGE_X86_32),y)
$(eval $(call early_x86_stage,verstage,elf32-i386))
else
$(eval $(call early_x86_stage,verstage,elf64-x86-64))
endif

# Verstage on x86 expected to be xip.
CBFSTOOL_VERSTAGE_OPTS  = -a 64 --xip -S ".car.data"

endif # CONFIG_ARCH_VERSTAGE_X86_32 / CONFIG_ARCH_VERSTAGE_X86_64

###############################################################################
# romstage
###############################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32)$(CONFIG_ARCH_ROMSTAGE_X86_64),y)

ifneq ($(CONFIG_ROMCC),y)

romstage-srcs += $(src)/mainboard/$(MAINBOARDDIR)/romstage.c

else # CONFIG_ROMCC == y

# This order matters. The mainboards requiring ROMCC need their mainboard
# code to follow the prior crt0s files for program flow control. The
# romstage.inc from the MAINBOARDDIR is implicitly main() for romstage
# because of the instruction sequen fall-through.
crt0s += $(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc
crt0s += $(src)/arch/x86/crt0_romcc_epilogue.inc

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

endif

romstage-libs ?=

ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32),y)
$(eval $(call early_x86_stage,romstage,elf32-i386))
else
$(eval $(call early_x86_stage,romstage,elf64-x86-64))
endif

# Compiling crt0 with -g seems to trigger https://sourceware.org/bugzilla/show_bug.cgi?id=6428
romstage-S-ccopts += -g0

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
ifeq ($(CONFIG_COMPILE_IN_DSDT),y)
$(eval $(call asl_template,dsdt,n))
else
$(eval $(call asl_template,dsdt))
endif
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

# The rmodule_link defintion creates an elf file with .rmod extension.
$(objcbfs)/ramstage.elf: $(objcbfs)/ramstage.debug.rmod
	cp $< $@

endif

ramstage-y += memlayout.ld

$(objcbfs)/ramstage.debug: $(objgenerated)/ramstage.o $(obj)/arch/x86/memlayout.ramstage.ld
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(LD_ramstage) $(CPPFLAGS) $(LDFLAGS_ramstage) -o $@ -L$(obj) $< -T $(obj)/arch/x86/memlayout.ramstage.ld

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
