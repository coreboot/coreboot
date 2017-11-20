ramstage-y += model_206ax_init.c
subdirs-y += ../../x86/name
subdirs-y += ../smm/gen1
subdirs-y += ../common

ramstage-y += acpi.c

ramstage-y += common.c
romstage-y += common.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += common.c

ramstage-y += tsc_freq.c
romstage-y += tsc_freq.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += tsc_freq.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c

romstage-$(CONFIG_CACHE_RELOCATED_RAMSTAGE_OUTSIDE_CBMEM) += stage_cache.c
postcar-$(CONFIG_CACHE_RELOCATED_RAMSTAGE_OUTSIDE_CBMEM) += stage_cache.c
ramstage-$(CONFIG_CACHE_RELOCATED_RAMSTAGE_OUTSIDE_CBMEM) += stage_cache.c

cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_206ax/microcode.bin
cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_306ax/microcode.bin

cpu_incs-y += $(src)/cpu/intel/car/non-evict/cache_as_ram.S
postcar-y += ../car/non-evict/exit_car.S

romstage-y += ../car/romstage.c
