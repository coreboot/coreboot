ifeq ($(CONFIG_DRIVERS_UART),y)

romstage-y += util.c
postcar-y += util.c
ramstage-y += util.c
bootblock-y += util.c
verstage-y += util.c
smm-$(CONFIG_DEBUG_SMI) += util.c

# Add the driver, only one can be enabled. The driver files may
# be located in the soc/ or cpu/ directories instead of here.

ifeq ($(CONFIG_DRIVERS_UART_8250IO),y)
bootblock-y += uart8250io.c
verstage-y += uart8250io.c
romstage-y += uart8250io.c
postcar-y += uart8250io.c
ramstage-y += uart8250io.c
smm-$(CONFIG_DEBUG_SMI) += uart8250io.c
endif

ifeq ($(CONFIG_DRIVERS_UART_8250MEM),y)
bootblock-y += uart8250mem.c
verstage-y += uart8250mem.c
romstage-y += uart8250mem.c
postcar-y += uart8250mem.c
ramstage-y += uart8250mem.c
smm-$(CONFIG_DEBUG_SMI) += uart8250mem.c
endif

ifeq ($(CONFIG_DRIVERS_UART_OXPCIE),y)
ramstage-y += oxpcie_early.c oxpcie.c
postcar-y += oxpcie_early.c
romstage-y += oxpcie_early.c
endif

ifeq ($(CONFIG_DRIVERS_UART_PL011),y)
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += pl011.c
romstage-y += pl011.c
postcar-y += pl011.c
ramstage-y += pl011.c
verstage-y += pl011.c
endif

ifeq ($(CONFIG_DRIVERS_UART_SIFIVE),y)
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += sifive.c
romstage-y += sifive.c
postcar-y += sifive.c
ramstage-y += sifive.c
endif

endif
