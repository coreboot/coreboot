ramstage-y += socket_LGA771.c
subdirs-y += ../model_6ex
subdirs-y += ../model_6fx
subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm
subdirs-y += ../microcode
subdirs-y += ../hyperthreading

cpu_incs += $(src)/cpu/intel/model_6ex/cache_as_ram.inc
