## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_NONCAR),y)

bootblock-y += bootblock.c
bootblock-y += cpu.c
bootblock-y += early_cache.c
bootblock-y += pre_c.S
bootblock-y += write_resume_eip.c
bootblock-$(CONFIG_TPM_MEASURED_BOOT) += bootblock_measure.c
romstage-y += memmap.c
romstage-y += romstage.c
ramstage-y += cpu.c
romstage-y += cpu.c
ramstage-y += memmap.c
ramstage-y += mpinit.c

# See memlayout_x86.ld for details:
# AMDCOMPRESS only supports one PT_LOAD segment.
# 1. Copy .data section to .datacopy section
#    Assembly will copy .datacopy to .data at runtime
# 2. Mark .data and .bss as NOLOAD
#    Drops the second PT_LOAD segment from ELF
$(objcbfs)/bootblock_fixed_data.elf: $(objcbfs)/bootblock.elf
	@printf "    OBJCOPY    $(notdir $(@))\n"
	$(OBJCOPY_bootblock) --dump-section .data=$(objcbfs)/data.section $<
	cp $< $@
	$(OBJCOPY_bootblock) --set-section-flags .datacopy=load \
		--set-section-flags .bss=noload --set-section-flags .data=noload $@
	$(OBJCOPY_bootblock) --update-section .datacopy=$(objcbfs)/data.section $@

endif # CONFIG_SOC_AMD_COMMON_BLOCK_NONCAR
