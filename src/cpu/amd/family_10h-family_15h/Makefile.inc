romstage-y += ../../x86/mtrr/earlymtrr.c

romstage-y += init_cpus.c

ramstage-y += model_10xxx_init.c
ramstage-y += processor_name.c

romstage-y += update_microcode.c
romstage-y += tsc_freq.c
ramstage-y += tsc_freq.c
romstage-y += ram_calc.c
ramstage-y += ram_calc.c
ramstage-y += monotonic_timer.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += powernow_acpi.c

# Microcode for Family 10h, 11h, 12h, and 14h
cbfs-files-$(CONFIG_CPU_MICROCODE_MULTIPLE_FILES) += microcode_amd.bin
microcode_amd.bin-file := 3rdparty/blobs/cpu/amd/family_10h-family_14h/microcode_amd.bin
microcode_amd.bin-type := microcode

# Microcode for Family 15h
cbfs-files-$(CONFIG_CPU_MICROCODE_MULTIPLE_FILES) += microcode_amd_fam15h.bin
microcode_amd_fam15h.bin-file := 3rdparty/blobs/cpu/amd/family_15h/microcode_amd_fam15h.bin
microcode_amd_fam15h.bin-type := microcode
