subdirs-y += ../model_106cx
subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm
subdirs-y += ../microcode
subdirs-y += ../hyperthreading
subdirs-y += ../speedstep

cpu_incs-y += $(src)/cpu/intel/car/cache_as_ram_ht.inc
romstage-y += ../car/romstage.c
