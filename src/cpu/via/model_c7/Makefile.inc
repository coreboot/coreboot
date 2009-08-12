ifeq ($(CONFIG_CPU_VIA_C7),y)
	subdirs-y += ../../x86/tsc
	subdirs-y += ../../x86/mtrr
	subdirs-y += ../../x86/fpu
	subdirs-y += ../../x86/mmx
	subdirs-y += ../../x86/sse
	subdirs-y += ../../x86/lapic
	subdirs-y += ../../x86/cache
	subdirs-y += ../../x86/smm
	subdirs-y += ../../intel/microcode
endif

obj-y += model_c7_init.o
