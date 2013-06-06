ramstage-y += model_2065x_init.c
subdirs-y += ../../x86/name
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../intel/turbo
subdirs-y += ../../intel/microcode
subdirs-y += ../../x86/smm

ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += acpi.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c

cpu_microcode-$(CONFIG_CPU_MICROCODE_CBFS_GENERATE)  += microcode_blob.c

cpu_incs += $(src)/cpu/intel/model_2065x/cache_as_ram.inc
