ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_UART),y)

bootblock-$(CONFIG_AMD_SOC_CONSOLE_UART) += uart_console.c

romstage-$(CONFIG_AMD_SOC_CONSOLE_UART) += uart_console.c

verstage_x86-$(CONFIG_AMD_SOC_CONSOLE_UART) += uart_console.c

ramstage-$(CONFIG_AMD_SOC_CONSOLE_UART) += uart_console.c

ifeq ($(CONFIG_DEBUG_SMI),y)
smm-$(CONFIG_AMD_SOC_CONSOLE_UART) += uart_console.c
endif

endif # CONFIG_SOC_AMD_COMMON_BLOCK_UART
