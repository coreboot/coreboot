ifeq ($(CONFIG_SOC_INTEL_CANNONLAKE),y)

subdirs-y += romstage
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/tsc

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/cpu.c
bootblock-y += bootblock/pch.c
bootblock-y += bootblock/report_platform.c
bootblock-y += gpio.c
bootblock-$(CONFIG_UART_DEBUG) += uart.c

romstage-y += cbmem.c
romstage-y += reset.c
romstage-$(CONFIG_UART_DEBUG) += uart.c

ramstage-y += cbmem.c
ramstage-$(CONFIG_PLATFORM_USES_FSP2_0) += reset.c
ramstage-$(CONFIG_UART_DEBUG) += uart.c

postcar-y += memmap.c
postcar-$(CONFIG_UART_DEBUG) += uart.c

CPPFLAGS_common += -I$(src)/soc/intel/cannonlake/include/fsp20
CPPFLAGS_common += -I$(src)/vendorcode/intel/fsp/fsp2_0/cannonlake

CPPFLAGS_common += -I$(src)/soc/intel/cannonlake
CPPFLAGS_common += -I$(src)/soc/intel/cannonlake/include

endif
