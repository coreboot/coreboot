subdirs-y += ../model_106cx
subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm
subdirs-y += ../microcode
subdirs-y += ../hyperthreading
subdirs-y += ../speedstep

ifneq ($(CONFIG_POSTCAR_STAGE),y)
cpu_incs-y += $(src)/cpu/intel/car/cache_as_ram_ht.inc
else
cpu_incs-y += $(src)/cpu/intel/car/p4-netburst/cache_as_ram.S
postcar-y += ../car/p4-netburst/exit_car.S
endif
romstage-y += ../car/romstage.c
