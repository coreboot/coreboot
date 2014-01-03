ramstage-y += model_2065x_init.c
subdirs-y += ../../x86/name
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/tsc
subdirs-y += ../../intel/turbo
subdirs-y += ../../intel/microcode
subdirs-y += ../../x86/smm


ramstage-y += tsc_freq.c
romstage-y += tsc_freq.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += tsc_freq.c

ramstage-y += acpi.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c

cpu_microcode-$(CONFIG_CPU_MICROCODE_CBFS_GENERATE)  += microcode_blob.c

cpu_incs += $(src)/cpu/intel/model_2065x/cache_as_ram.inc
