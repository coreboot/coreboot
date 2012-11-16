subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/mtrr
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm
subdirs-y += ../../intel/microcode

ramstage-y += c7_init.c

cpu_incs += $(src)/cpu/via/car/cache_as_ram.inc
