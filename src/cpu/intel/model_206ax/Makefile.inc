ramstage-y += model_206ax_init.c
subdirs-y += ../../x86/name
subdirs-y += ../smm/gen1

ramstage-y += acpi.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += finalize.c

cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_206ax/microcode.bin
cpu_microcode_bins += 3rdparty/blobs/cpu/intel/model_306ax/microcode.bin

cpu_incs-y += $(src)/cpu/intel/model_206ax/cache_as_ram.inc
