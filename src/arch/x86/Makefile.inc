##
## SPDX-License-Identifier: GPL-2.0-only
## This file is part of the coreboot project.
##

ifeq ($(CONFIG_POSTCAR_STAGE),y)
$(eval $(call init_standard_toolchain,postcar))
endif

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

stripped_second_vbios_id = $(call strip_quotes,$(CONFIG_VGA_BIOS_SECOND_ID))
cbfs-files-$(CONFIG_VGA_BIOS_SECOND) += pci$(stripped_second_vbios_id).rom
pci$(stripped_second_vbios_id).rom-file := $(call strip_quotes,$(CONFIG_VGA_BIOS_SECOND_FILE))
pci$(stripped_second_vbios_id).rom-type := optionrom

stripped_vgabios_dgpu_id = $(call strip_quotes,$(CONFIG_VGA_BIOS_DGPU_ID))
cbfs-files-$(CONFIG_VGA_BIOS_DGPU) += pci$(stripped_vgabios_dgpu_id).rom
pci$(stripped_vgabios_dgpu_id).rom-file := $(call strip_quotes,$(CONFIG_VGA_BIOS_DGPU_FILE))
pci$(stripped_vgabios_dgpu_id).rom-type := optionrom

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
# all (bootblock,verstage,romstage,postcar,ramstage)
###############################################################################

ifeq ($(CONFIG_ARCH_X86),y)

all-$(CONFIG_HAVE_CF9_RESET) += cf9_reset.c
all-y += boot.c
all-y += memcpy.c
all-y += memset.c
all-y += cpu_common.c
all-y += post.c

endif

###############################################################################
# bootblock
###############################################################################

ifeq ($(CONFIG_ARCH_BOOTBLOCK_X86_32)$(CONFIG_ARCH_BOOTBLOCK_X86_64),y)

bootblock-$(CONFIG_IDT_IN_EVERY_STAGE) += exception.c
bootblock-$(CONFIG_IDT_IN_EVERY_STAGE) += idt.S
bootblock-$(CONFIG_COLLECT_TIMESTAMPS_TSC) += timestamp.c
bootblock-$(CONFIG_X86_TOP4G_BOOTMEDIA_MAP) += mmap_boot.c
bootblock-$(CONFIG_BOOTBLOCK_NORMAL) += bootblock_normal.c
bootblock-y += id.S
$(call src-to-obj,bootblock,$(dir)/id.S): $(obj)/build.h

bootblock-y += bootblock_crt0.S

ifeq ($(CONFIG_ARCH_BOOTBLOCK_X86_32),y)
$(eval $(call early_x86_stage,bootblock,elf32-i386))
else
$(eval $(call early_x86_stage,bootblock,elf64-x86-64))
endif

bootblock-$(CONFIG_MICROCODE_UPDATE_PRE_RAM) += walkcbfs.S

endif # CONFIG_ARCH_BOOTBLOCK_X86_32 / CONFIG_ARCH_BOOTBLOCK_X86_64

###############################################################################
# verstage
###############################################################################

ifeq ($(CONFIG_ARCH_VERSTAGE_X86_32)$(CONFIG_ARCH_VERSTAGE_X86_64),y)

verstage-$(CONFIG_VBOOT_SEPARATE_VERSTAGE) += gdt_init.S
verstage-$(CONFIG_IDT_IN_EVERY_STAGE) += exception.c
verstage-$(CONFIG_IDT_IN_EVERY_STAGE) += idt.S

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
# gdt_init.S is included by entry32.inc when romstage is the first C
# environment.
romstage-y += gdt_init.S
romstage-y += cbmem.c
romstage-$(CONFIG_IDT_IN_EVERY_STAGE) += exception.c
romstage-$(CONFIG_IDT_IN_EVERY_STAGE) += idt.S
romstage-y += memmove.c
romstage-$(CONFIG_X86_TOP4G_BOOTMEDIA_MAP) += mmap_boot.c
romstage-y += postcar_loader.c
romstage-$(CONFIG_COLLECT_TIMESTAMPS_TSC) += timestamp.c

romstage-srcs += $(wildcard $(src)/mainboard/$(MAINBOARDDIR)/romstage.c)
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
postcar-y += gdt_init.S
postcar-$(CONFIG_IDT_IN_EVERY_STAGE) += exception.c
postcar-$(CONFIG_IDT_IN_EVERY_STAGE) += idt.S
postcar-y += exit_car.S
postcar-y += memmove.c
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
ramstage-y += c_start.S
ramstage-y += cpu.c
ramstage-y += ebda.c
ramstage-y += exception.c
ramstage-y += idt.S
ramstage-y += gdt.c
ramstage-$(CONFIG_IOAPIC) += ioapic.c
ramstage-y += memlayout.ld
ramstage-y += memmove.c
ramstage-$(CONFIG_X86_TOP4G_BOOTMEDIA_MAP) += mmap_boot.c
ramstage-$(CONFIG_GENERATE_MP_TABLE) += mpspec.c
ramstage-$(CONFIG_GENERATE_PIRQ_TABLE) += pirq_routing.c
ramstage-y += rdrand.c
ramstage-$(CONFIG_GENERATE_SMBIOS_TABLES) += smbios.c
ramstage-y += tables.c
ramstage-$(CONFIG_COOP_MULTITASKING) += thread.c
ramstage-$(CONFIG_COOP_MULTITASKING) += thread_switch.S
ramstage-$(CONFIG_COLLECT_TIMESTAMPS_TSC) += timestamp.c
ramstage-$(CONFIG_HAVE_ACPI_RESUME) += wakeup.S

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
ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/acpi_tables.c),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/acpi_tables.c
endif
$(eval $(call asl_template,dsdt))
ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/fadt.c),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/fadt.c
endif
endif # CONFIG_GENERATE_ACPI_TABLES

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
ifeq ($(CONFIG_ARCH_RAMSTAGE_X86_32),y)
	$(LD_ramstage) -m elf_i386 -r -o $@ $(COMPILER_RT_FLAGS_ramstage) --whole-archive --start-group $(filter-out %.ld,$(ramstage-objs)) $(ramstage-libs) --no-whole-archive $(COMPILER_RT_ramstage) --end-group
else
	$(LD_ramstage) -m elf_x86_64 -r -o $@ $(COMPILER_RT_FLAGS_ramstage) --whole-archive --start-group $(filter-out %.ld,$(ramstage-objs)) $(ramstage-libs) --no-whole-archive $(COMPILER_RT_ramstage) --end-group
endif

endif # CONFIG_ARCH_RAMSTAGE_X86_32 / CONFIG_ARCH_RAMSTAGE_X86_64

smm-$(CONFIG_IDT_IN_EVERY_STAGE) += exception.c
smm-$(CONFIG_IDT_IN_EVERY_STAGE) += idt.S
smm-y += memcpy.c
smm-y += memmove.c
smm-y += memset.c
smm-$(CONFIG_X86_TOP4G_BOOTMEDIA_MAP) += mmap_boot.c

ifneq ($(wildcard src/mainboard/$(MAINBOARDDIR)/smihandler.c),)
smm-srcs += src/mainboard/$(MAINBOARDDIR)/smihandler.c
endif
