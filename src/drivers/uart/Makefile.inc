ifeq ($(CONFIG_CONSOLE_SERIAL),y)
romstage-y += util.c
ramstage-y += util.c
bootblock-y += util.c
smm-y += util.c
endif

ifeq ($(CONFIG_CONSOLE_SERIAL8250),y)
romstage-y += uart8250io.c
ramstage-y += uart8250io.c
smm-y += uart8250io.c
endif

ifeq ($(CONFIG_CONSOLE_SERIAL8250MEM),y)
romstage-y += uart8250mem.c
ramstage-y += uart8250mem.c
smm-y += uart8250mem.c
endif

ifeq ($(CONFIG_CONSOLE_SERIAL_UART),y)

ifeq ($(CONFIG_DRIVERS_UART_PL011),y)
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += pl011.c
romstage-$(CONFIG_EARLY_CONSOLE) += pl011.c
ramstage-y += pl011.c
endif

endif
