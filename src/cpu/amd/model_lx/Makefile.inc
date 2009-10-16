subdirs-y += ../../x86/tsc
subdirs-y += ../../x86/fpu
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../../x86/smm

driver-y += model_lx_init.o
obj-y += cpubug.o
obj-y += vsmsetup.o
