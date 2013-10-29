ramstage-y += model_206ax_init.c
subdirs-y += ../../x86/name

ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += acpi.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c

cpu_microcode-$(CONFIG_CPU_MICROCODE_CBFS_GENERATE)  += microcode_blob.c

cpu_incs += $(src)/cpu/intel/fsp_model_206ax/cache_as_ram.inc

CC := $(CC) -I$(CONFIG_MICROCODE_INCLUDE_PATH)
