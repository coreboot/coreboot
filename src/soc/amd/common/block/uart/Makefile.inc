## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_UART),y)

# all-y can't be used, since verstage on PSP has a different implementation
bootblock-y += uart.c
verstage_x86-y += uart.c
romstage-y += uart.c
postcar-y += uart.c
ramstage-y += uart.c
smm-$(CONFIG_DEBUG_SMI) += uart.c

all-$(CONFIG_AMD_SOC_CONSOLE_UART) += uart_console.c

ifeq ($(CONFIG_DEBUG_SMI),y)
smm-$(CONFIG_AMD_SOC_CONSOLE_UART) += uart_console.c
endif

endif # CONFIG_SOC_AMD_COMMON_BLOCK_UART
