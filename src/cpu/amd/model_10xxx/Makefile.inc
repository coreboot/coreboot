romstage-y += ../../x86/mtrr/earlymtrr.c
ramstage-y += model_10xxx_init.c
ramstage-y += processor_name.c

romstage-y += update_microcode.c
romstage-y += ram_calc.c
ramstage-y += ram_calc.c
ramstage-y += monotonic_timer.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += powernow_acpi.c

cpu_microcode-$(CONFIG_CPU_MICROCODE_CBFS_GENERATE) += microcode_blob.c
