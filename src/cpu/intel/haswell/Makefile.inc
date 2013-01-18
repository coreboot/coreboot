ramstage-y += haswell_init.c
subdirs-y += ../../x86/name
romstage-y += romstage.c

ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += acpi.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smmrelocate.c

cpu_microcode-$(CONFIG_CPU_MICROCODE_CBFS_GENERATE)  += microcode_blob.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c

cpu_incs += $(src)/cpu/intel/haswell/cache_as_ram.inc
