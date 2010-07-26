subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm

driver-y += model_gx2_init.o
obj-y += cpubug.o

cpu_incs += $(src)/cpu/amd/model_gx2/cache_as_ram.inc
