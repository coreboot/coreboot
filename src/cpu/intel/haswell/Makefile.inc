driver-y += haswell_init.c
subdirs-y += ../../x86/name

ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += acpi.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c

cpu_incs += $(src)/cpu/intel/haswell/cache_as_ram.inc
