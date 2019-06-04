ramstage-y += haswell_init.c
ramstage-y += tsc_freq.c
romstage-y += romstage.c
romstage-y += tsc_freq.c
romstage-y += ../car/romstage.c

postcar-y += tsc_freq.c

ramstage-y += acpi.c
ramstage-$(CONFIG_CACHE_RELOCATED_RAMSTAGE_OUTSIDE_CBMEM) += stage_cache.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smmrelocate.c

romstage-$(CONFIG_CACHE_RELOCATED_RAMSTAGE_OUTSIDE_CBMEM) += stage_cache.c
postcar-$(CONFIG_CACHE_RELOCATED_RAMSTAGE_OUTSIDE_CBMEM) += stage_cache.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += tsc_freq.c

ifneq ($(CONFIG_TSC_MONOTONIC_TIMER),y)
bootblock-y += monotonic_timer.c
romstage-y += monotonic_timer.c
postcar-y += monotonic_timer.c
ramstage-y += monotonic_timer.c
smm-y += monotonic_timer.c
endif

bootblock-y += ../car/non-evict/cache_as_ram.S
bootblock-y += ../car/bootblock.c
bootblock-y += ../../x86/early_reset.S
bootblock-y += bootblock.c
bootblock-y += tsc_freq.c

postcar-y += ../car/non-evict/exit_car.S

verstage-y += tsc_freq.c

subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm
subdirs-y += ../microcode
subdirs-y += ../turbo
subdirs-y += ../common

cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_306cx/microcode.bin
cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_4065x/microcode.bin
