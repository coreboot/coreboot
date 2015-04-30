ramstage-y += haswell_init.c
ramstage-y += tsc_freq.c
romstage-y += romstage.c
romstage-y += tsc_freq.c

ramstage-y += acpi.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smmrelocate.c
ramstage-$(CONFIG_MONOTONIC_TIMER_MSR) += monotonic_timer.c

cpu_microcode-$(CONFIG_CPU_MICROCODE_CBFS_GENERATE)  += microcode_blob.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += tsc_freq.c
smm-$(CONFIG_MONOTONIC_TIMER_MSR) += monotonic_timer.c

cpu_incs += $(src)/cpu/intel/haswell/cache_as_ram.inc

subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm
subdirs-y += ../microcode
subdirs-y += ../turbo
