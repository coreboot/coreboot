subdirs-y += ../model_6bx
subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm
subdirs-y += ../microcode

cpu_incs-y += $(src)/cpu/intel/car/cache_as_ram.inc
