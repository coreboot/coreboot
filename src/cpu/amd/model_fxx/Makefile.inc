# no conditionals here. If you include this file from a socket, then you get all the binaries. 
driver-y += model_fxx_init.o
obj-y += apic_timer.o
obj-y += model_fxx_update_microcode.o
obj-y += processor_name.o
obj-y += powernow_acpi.o
