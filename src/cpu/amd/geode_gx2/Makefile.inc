subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm

driver-y += geode_gx2_init.c
ramstage-y += cpubug.c

cpu_incs += $(src)/cpu/amd/geode_gx2/cache_as_ram.inc
