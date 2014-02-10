ifeq ($(CONFIG_CONSOLE_SERIAL_UART),y)

ifeq ($(CONFIG_DRIVERS_UART_PL011),y)
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += pl011.c
romstage-$(CONFIG_EARLY_CONSOLE) += pl011.c
ramstage-y += pl011.c
endif

endif
