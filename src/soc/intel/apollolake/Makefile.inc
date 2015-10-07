ifeq ($(CONFIG_SOC_INTEL_APOLLOLAKE),y)

subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/cache_as_ram.S
bootblock-y += placeholders.c

romstage-y += placeholders.c
smm-y += placeholders.c
ramstage-y += placeholders.c

CPPFLAGS_common += -I$(src)/soc/intel/apollolake/include

endif
