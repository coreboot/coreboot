# no conditionals here. If you include this file from a socket, then you get all the binaries.
ramstage-y += model_fxx_init.c
ramstage-y += model_fxx_update_microcode.c
ramstage-y += processor_name.c
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += powernow_acpi.c
