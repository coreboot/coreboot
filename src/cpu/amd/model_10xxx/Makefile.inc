driver-y += model_10xxx_init.c
ramstage-$(CONFIG_UPDATE_CPU_MICROCODE) += update_microcode.c
ramstage-y += apic_timer.c
ramstage-y += processor_name.c
