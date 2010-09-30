# no conditionals here. If you include this file from a socket, then you get all the binaries.
driver-y += model_10xxx_init.c
ramstage-y += update_microcode.c
ramstage-y += apic_timer.c
ramstage-y += processor_name.c
