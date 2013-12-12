romstage-y += ../../x86/mtrr/earlymtrr.c
ramstage-y += model_10xxx_init.c
ramstage-y += processor_name.c

romstage-$(CONFIG_UPDATE_CPU_MICROCODE) += update_microcode.c
