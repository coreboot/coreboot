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

################################################################################
# i386 specific tools
NVRAMTOOL:=$(objutil)/nvramtool/nvramtool

OPTION_TABLE_H:=
ifeq ($(CONFIG_HAVE_OPTION_TABLE),y)

cbfs-files-y += cmos_layout.bin
cmos_layout.bin-file = $(obj)/cmos_layout.bin
cmos_layout.bin-type = cmos_layout

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


define early_x86_assembly_entry_rule
# $1 stage name
# Add the assembly file that pulls in the rest of the dependencies in
# the right order. Make sure the auto generated assembly.inc is a proper
# dependency.
$(1)-y += assembly_entry.S
$(call src-to-obj,$(1),$(dir)/assembly_entry.S): $(objgenerated)/assembly.inc
endef

define early_x86_stage
# $1 stage name
# $2 oformat
$(1)-y += memlayout.ld

# The '.' include path is needed for the generated assembly.inc file.
$(1)-S-ccopts += -I.

$$(objcbfs)/$(1).debug: $$$$($(1)-libs) $$$$($(1)-objs)
	@printf "    LINK       $$(subst $$(obj)/,,$$(@))\n"
	$$(LD_$(1)) $$(LDFLAGS_$(1)) -o $$@ -L$$(obj) $$(COMPILER_RT_FLAGS_$(1)) --whole-archive --start-group $$(filter-out %.ld,$$($(1)-objs)) $$($(1)-libs) --no-whole-archive $$(COMPILER_RT_$(1)) --end-group -T $(call src-to-obj,$(1),$(dir)/memlayout.ld) --oformat $(2)
	-LANG=C LC_ALL= $$(OBJCOPY_$(1)) --only-section .illegal_globals $$(@) $$(objcbfs)/$(1)_null.offenders >/dev/null 2>&1
	if [ -z "$$$$($$(NM_$(1)) $$(objcbfs)/$(1)_null.offenders 2>&1 | grep 'no symbols')" ];then \
		echo "Forbidden global variables in $(1):"; \
		$$(NM_$(1)) $$(objcbfs)/$(1)_null.offenders; false; \
	fi
endef

###############################################################################
# bootblock
###############################################################################

ifeq ($(CONFIG_ARCH_BOOTBLOCK_X86_32)$(CONFIG_ARCH_BOOTBLOCK_X86_64),y)

bootblock-y += boot.c
bootblock-y += cpu_common.c
bootblock-$(CONFIG_IDT_IN_EVERY_STAGE) += exception.c
bootblock-$(CONFIG_IDT_IN_EVERY_STAGE) += idt.S
bootblock-y += memcpy.c
bootblock-y += memset.c
bootblock-$(CONFIG_COLLECT_TIMESTAMPS_TSC) += timestamp.c
bootblock-$(CONFIG_X86_TOP4G_BOOTMEDIA_MAP) += mmap_boot.c

bootblock-y += id.S
$(call src-to-obj,bootblock,$(dir)/id.S): $(obj)/build.h

ifeq ($(CONFIG_C_ENVIRONMENT_BOOTBLOCK),y)

bootblock-y += bootblock_crt0.S

ifeq ($(CONFIG_ARCH_BOOTBLOCK_X86_32),y)
$(eval $(call early_x86_stage,bootblock,elf32-i386))
else
$(eval $(call early_x86_stage,bootblock,elf64-x86-64))
endif

else # !C_ENVIRONMENT_BOOTBLOCK

# x86-specific linker flags
ifeq ($(CONFIG_ARCH_BOOTBLOCK_X86_32),y)
LDFLAGS_bootblock += -m elf_i386 --oformat elf32-i386
else
LDFLAGS_bootblock += -m elf_x86_64 --oformat elf64-x86-64
endif

# Add the assembly file that pulls in the rest of the dependencies in
# the right order. Make sure the auto generated bootblock.inc is a proper
# dependency. Make the same true for the linker sript.
bootblock-y += bootblock_romcc.S
bootblock-y += walkcbfs.S
$(call src-to-obj,bootblock,$(dir)/bootblock_romcc.S): $(objgenerated)/bootblock.inc

bootblock-y += bootblock.ld
$(call src-to-obj,bootblock,$(dir)/bootblock.ld): $(objgenerated)/bootblock.ld

bootblock_romccflags := -mcpu=i386 -O2 -D__PRE_RAM__ -D__BOOTBLOCK__
ifeq ($(CONFIG_SSE),y)
bootblock_romccflags := -mcpu=k7 -msse -O2 -D__PRE_RAM__ -D__BOOTBLOCK__
endif

# This is a hack in case there are no per chipset linker files.
$(objgenerated)/empty: build-dirs
	touch $@

$(objgenerated)/bootblock.ld: $$(filter-out $(call src-to-obj,bootblock,src/arch/x86/bootblock.ld), $$(filter %.ld,$$(bootblock-objs))) $(objgenerated)/empty
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	cat $^ >> $@.tmp
	mv $@.tmp $@

-include $(objgenerated)/bootblock.inc.d
$(objgenerated)/bootblock.inc: $(src)/arch/x86/$(subst ",,$(CONFIG_BOOTBLOCK_SOURCE)) $(objutil)/romcc/romcc $(OPTION_TABLE_H) $(KCONFIG_AUTOHEADER)
# The open quote in the subst messes with syntax highlighting. Fix it - ")
	@printf "    ROMCC      $(subst $(obj)/,,$(@))\n"
	$(CC_bootblock) $(CPPFLAGS_bootblock) -MM -MT$(objgenerated)/bootblock.inc \
		$< > $(objgenerated)/bootblock.inc.d
	$(ROMCC) -c -S $(bootblock_romccflags) -I. $(CPPFLAGS_bootblock) $< -o $@

# bootblock.ld is part of $(bootblock-objs)
$(objcbfs)/bootblock.debug: $$(bootblock-objs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_bootblock) $(LDFLAGS_bootblock) -o $@ -L$(obj) \
		$(filter-out %.ld,$(bootblock-objs)) \
		-T $(call src-to-obj,bootblock,src/arch/x86/bootblock.ld)

endif # C_ENVIRONMENT_BOOTBLOCK


endif # CONFIG_ARCH_BOOTBLOCK_X86_32 / CONFIG_ARCH_BOOTBLOCK_X86_64

###############################################################################
# verstage
###############################################################################

ifeq ($(CONFIG_ARCH_VERSTAGE_X86_32)$(CONFIG_ARCH_VERSTAGE_X86_64),y)

verstage-y += boot.c
verstage-$(CONFIG_VBOOT_SEPARATE_VERSTAGE) += gdt_init.S
verstage-$(CONFIG_IDT_IN_EVERY_STAGE) += exception.c
verstage-$(CONFIG_IDT_IN_EVERY_STAGE) += idt.S

verstage-$(CONFIG_ARCH_RAMSTAGE_X86_32) += cpu_common.c
verstage-y += memset.c
verstage-y += memcpy.c
verstage-y += memmove.c
verstage-$(CONFIG_X86_TOP4G_BOOTMEDIA_MAP) += mmap_boot.c
# If verstage is a separate stage it means there's no need
# for a chipset-specific car_stage_entry() so use the generic one
# which just calls verstage().
verstage-$(CONFIG_VBOOT_SEPARATE_VERSTAGE) += verstage.c

verstage-$(CONFIG_COLLECT_TIMESTAMPS_TSC) += timestamp.c

verstage-libs ?=

$(eval $(call early_x86_assembly_entry_rule,verstage))

ifeq ($(CONFIG_ARCH_VERSTAGE_X86_32),y)
$(eval $(call early_x86_stage,verstage,elf32-i386))
else
$(eval $(call early_x86_stage,verstage,elf64-x86-64))
endif

endif # CONFIG_ARCH_VERSTAGE_X86_32 / CONFIG_ARCH_VERSTAGE_X86_64

###############################################################################
# romstage
###############################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32)$(CONFIG_ARCH_ROMSTAGE_X86_64),y)

romstage-$(CONFIG_HAVE_ACPI_RESUME) += acpi_s3.c
romstage-y += boot.c
# gdt_init.S is included by entry32.inc when romstage is the first C
# environment.
romstage-$(CONFIG_C_ENVIRONMENT_BOOTBLOCK) += gdt_init.S
romstage-y += cbmem.c
romstage-y += cbfs_and_run.c
romstage-$(CONFIG_ARCH_RAMSTAGE_X86_32) += cpu_common.c
romstage-$(CONFIG_EARLY_EBDA_INIT) += ebda.c
romstage-$(CONFIG_IDT_IN_EVERY_STAGE) += exception.c
romstage-$(CONFIG_IDT_IN_EVERY_STAGE) += idt.S
romstage-y += memcpy.c
romstage-y += memmove.c
romstage-y += memset.c
romstage-$(CONFIG_X86_TOP4G_BOOTMEDIA_MAP) += mmap_boot.c
romstage-y += postcar_loader.c
romstage-$(CONFIG_COLLECT_TIMESTAMPS_TSC) += timestamp.c

romstage-srcs += $(src)/mainboard/$(MAINBOARDDIR)/romstage.c
romstage-libs ?=

$(eval $(call early_x86_assembly_entry_rule,romstage))

ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32),y)
$(eval $(call early_x86_stage,romstage,elf32-i386))
else
$(eval $(call early_x86_stage,romstage,elf64-x86-64))
endif

# Compiling crt0 with -g seems to trigger https://sourceware.org/bugzilla/show_bug.cgi?id=6428
romstage-S-ccopts += -g0

endif # CONFIG_ARCH_ROMSTAGE_X86_32 / CONFIG_ARCH_ROMSTAGE_X86_64

###############################################################################
# postcar
###############################################################################

$(eval $(call create_class_compiler,postcar,x86_32))
postcar-generic-ccopts += -D__POSTCAR__

postcar-$(CONFIG_HAVE_ACPI_RESUME) += acpi_s3.c
postcar-y += boot.c
postcar-y += gdt_init.S
postcar-y += cbfs_and_run.c
postcar-y += cbmem.c
postcar-y += cpu_common.c
postcar-$(CONFIG_EARLY_EBDA_INIT) += ebda.c
postcar-$(CONFIG_IDT_IN_EVERY_STAGE) += exception.c
postcar-$(CONFIG_IDT_IN_EVERY_STAGE) += idt.S
postcar-y += exit_car.S
postcar-y += memcpy.c
postcar-y += memmove.c
postcar-y += memset.c
postcar-y += memlayout.ld
postcar-$(CONFIG_X86_TOP4G_BOOTMEDIA_MAP) += mmap_boot.c
postcar-y += postcar.c
postcar-$(CONFIG_COLLECT_TIMESTAMPS_TSC) += timestamp.c

LDFLAGS_postcar += -Map $(objcbfs)/postcar.map

$(objcbfs)/postcar.debug: $$(postcar-objs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_postcar) $(LDFLAGS_postcar) -o $@ -L$(obj) $(COMPILER_RT_FLAGS_postcar) --whole-archive --start-group $(filter-out %.ld,$^) --no-whole-archive $(COMPILER_RT_postcar) --end-group -T $(call src-to-obj,postcar,src/arch/x86/memlayout.ld)

$(objcbfs)/postcar.elf: $(objcbfs)/postcar.debug.rmod
	cp $< $@

# Add postcar to CBFS
cbfs-files-$(CONFIG_POSTCAR_STAGE)  += $(CONFIG_CBFS_PREFIX)/postcar
$(CONFIG_CBFS_PREFIX)/postcar-file := $(objcbfs)/postcar.elf
$(CONFIG_CBFS_PREFIX)/postcar-type := stage
$(CONFIG_CBFS_PREFIX)/postcar-compression := none

###############################################################################
# ramstage
###############################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32)$(CONFIG_ARCH_RAMSTAGE_X86_64),y)

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpigen.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpigen_dsm.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi_device.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi_pld.c
ramstage-$(CONFIG_HAVE_ACPI_RESUME) += acpi_s3.c
ramstage-$(CONFIG_ACPI_BERT) += acpi_bert_storage.c
ramstage-y += boot.c
ramstage-y += c_start.S
ramstage-y += cbmem.c
ramstage-y += cpu.c
ramstage-y += cpu_common.c
ramstage-y += ebda.c
ramstage-y += exception.c
ramstage-y += idt.S
ramstage-y += gdt.c
ramstage-$(CONFIG_IOAPIC) += ioapic.c
ramstage-y += memcpy.c
ramstage-y += memlayout.ld
ramstage-y += memmove.c
ramstage-y += memset.c
ramstage-$(CONFIG_X86_TOP4G_BOOTMEDIA_MAP) += mmap_boot.c
ramstage-$(CONFIG_GENERATE_MP_TABLE) += mpspec.c
ramstage-y += pci_ops_conf1.c
ramstage-$(CONFIG_NO_MMCONF_SUPPORT) += pci_ops.c
ramstage-$(CONFIG_GENERATE_PIRQ_TABLE) += pirq_routing.c
ramstage-y += rdrand.c
ramstage-$(CONFIG_GENERATE_SMBIOS_TABLES) += smbios.c
ramstage-y += tables.c
ramstage-$(CONFIG_COOP_MULTITASKING) += thread.c
ramstage-$(CONFIG_COOP_MULTITASKING) += thread_switch.S
ramstage-$(CONFIG_COLLECT_TIMESTAMPS_TSC) += timestamp.c
ramstage-$(CONFIG_HAVE_ACPI_RESUME) += wakeup.S

smm-$(CONFIG_IDT_IN_EVERY_STAGE) += exception.c
smm-$(CONFIG_IDT_IN_EVERY_STAGE) += idt.S
smm-y += memcpy.c
smm-y += memmove.c
smm-y += memset.c
smm-$(CONFIG_X86_TOP4G_BOOTMEDIA_MAP) += mmap_boot.c

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32),y)
rmodules_x86_32-y += memcpy.c
rmodules_x86_32-y += memmove.c
rmodules_x86_32-y += memset.c
else
rmodules_x86_64-y += memcpy.c
rmodules_x86_64-y += memmove.c
rmodules_x86_64-y += memset.c
endif

ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32),y)
target-objcopy=-O elf32-i386 -B i386
endif
ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_64),y)
target-objcopy=-O elf64-x86-64 -B i386:x86-64
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

# The rmodule_link definition creates an elf file with .rmod extension.
$(objcbfs)/ramstage.elf: $(objcbfs)/ramstage.debug.rmod
	cp $< $@

endif

$(objcbfs)/ramstage.debug: $(objgenerated)/ramstage.o $(call src-to-obj,ramstage,src/arch/x86/memlayout.ld)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(LD_ramstage) $(CPPFLAGS) $(LDFLAGS_ramstage) -o $@ -L$(obj) $< -T $(call src-to-obj,ramstage,src/arch/x86/memlayout.ld)

$(objgenerated)/ramstage.o: $$(ramstage-objs) $(COMPILER_RT_ramstage) $$(ramstage-libs)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32),y)
	$(LD_ramstage) -m elf_i386 -r -o $@ $(COMPILER_RT_FLAGS_ramstage) --whole-archive --start-group $(filter-out %.ld,$(ramstage-objs)) $(ramstage-libs) --no-whole-archive $(COMPILER_RT_ramstage) --end-group
else
	$(LD_ramstage) -m elf_x86_64 -r -o $@ $(COMPILER_RT_FLAGS_ramstage) --whole-archive --start-group $(filter-out %.ld,$(ramstage-objs)) $(ramstage-libs) --no-whole-archive $(COMPILER_RT_ramstage) --end-group
endif

endif # CONFIG_ARCH_RAMSTAGE_X86_32 / CONFIG_ARCH_RAMSTAGE_X86_64
