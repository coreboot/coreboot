ifeq ($(CONFIG_SOC_INTEL_APOLLOLAKE),y)

subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/cache_as_ram.S
bootblock-y += bootblock/bootblock.c
bootblock-y += gpio.c
bootblock-y += placeholders.c
bootblock-y += tsc_freq.c
bootblock-y += uart_early.c

romstage-y += placeholders.c
romstage-y += gpio.c
romstage-y += uart_early.c

smm-y += placeholders.c
ramstage-y += placeholders.c
ramstage-y += gpio.c
ramstage-y += uart_early.c

CPPFLAGS_common += -I$(src)/soc/intel/apollolake/include

endif
