ramstage-y += haswell_init.c
ramstage-y += tsc_freq.c
romstage-y += romstage.c
romstage-y += tsc_freq.c

ramstage-y += acpi.c
ramstage-$(CONFIG_CACHE_RELOCATED_RAMSTAGE_OUTSIDE_CBMEM) += stage_cache.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smmrelocate.c
ramstage-y += monotonic_timer.c

romstage-$(CONFIG_CACHE_RELOCATED_RAMSTAGE_OUTSIDE_CBMEM) += stage_cache.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += tsc_freq.c
smm-y += monotonic_timer.c

cpu_incs-y += $(src)/cpu/intel/haswell/cache_as_ram.inc

subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm
subdirs-y += ../microcode
subdirs-y += ../turbo

cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_306cx/microcode.bin
cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_4065x/microcode.bin
