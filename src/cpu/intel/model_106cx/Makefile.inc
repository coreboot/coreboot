ramstage-y += model_106cx_init.c
subdirs-y += ../../x86/name

cpu_incs += $(src)/cpu/intel/car/cache_as_ram_ht.inc
cpu_microcode-$(CONFIG_CPU_MICROCODE_CBFS_GENERATE) += microcode_blob.c
