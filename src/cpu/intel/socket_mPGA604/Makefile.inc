subdirs-y += ../model_f2x
subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm
subdirs-y += ../microcode
subdirs-y += ../hyperthreading

cpu_incs-y += $(src)/cpu/intel/car/p4-netburst/cache_as_ram.S
postcar-y += ../car/p4-netburst/exit_car.S
romstage-y += ../car/romstage.c
