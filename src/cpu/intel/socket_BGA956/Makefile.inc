ramstage-y += socket_BGA956.c
subdirs-y += ../model_1067x
subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm
subdirs-y += ../microcode
subdirs-y += ../hyperthreading
subdirs-y += ../speedstep

# Use Intel Core (not Core 2) code for CAR init, any CPU might be used.
cpu_incs += $(src)/cpu/intel/model_6ex/cache_as_ram.inc
