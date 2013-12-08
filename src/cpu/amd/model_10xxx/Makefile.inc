ramstage-y += model_10xxx_init.c
ramstage-y += processor_name.c

romstage-$(CONFIG_SUPPORT_CPU_UCODE_IN_CBFS) += update_microcode.c

cpu_microcode-$(CONFIG_CPU_MICROCODE_CBFS_GENERATE) += microcode_blob.c
