ifeq ($(CONFIG_SOC_INTEL_CANNONLAKE),y)

subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/tsc

bootblock-$(CONFIG_DRIVERS_UART_8250MEM) += uart.c
bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/cpu.c
bootblock-y += bootblock/pch.c
bootblock-y += bootblock/report_platform.c
bootblock-y += gpio.c

romstage-y += cbmem.c
romstage-y += reset.c
romstage-$(CONFIG_DRIVERS_UART_8250MEM) += uart.c

ramstage-y += cbmem.c
ramstage-$(CONFIG_PLATFORM_USES_FSP2_0) += reset.c
ramstage-$(CONFIG_DRIVERS_UART_8250MEM) += uart.c

CPPFLAGS_common += -I$(src)/soc/intel/cannonlake/include/fsp20
CPPFLAGS_common += -I$(src)/vendorcode/intel/fsp/fsp2_0/cannonlake

CPPFLAGS_common += -I$(src)/soc/intel/cannonlake
CPPFLAGS_common += -I$(src)/soc/intel/cannonlake/include

endif
