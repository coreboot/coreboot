# no conditionals here. If you include this file from a socket, then you get all the binaries.
driver-y += model_fxx_init.c
ramstage-y += apic_timer.c
ramstage-y += model_fxx_update_microcode.c
ramstage-y += processor_name.c
ramstage-y += powernow_acpi.c
