ramstage-y += model_10xxx_init.c
ramstage-y += processor_name.c

romstage-$(CONFIG_UPDATE_CPU_MICROCODE) += update_microcode.c

cpu_microcode-$(CONFIG_CPU_MICROCODE_CBFS_GENERATE) += microcode_blob.c
