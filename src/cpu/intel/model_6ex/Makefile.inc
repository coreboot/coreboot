ramstage-y += model_6ex_init.c
subdirs-y += ../../x86/name

cpu_microcode-$(CONFIG_CPU_MICROCODE_CBFS_GENERATE) += microcode_blob.c
