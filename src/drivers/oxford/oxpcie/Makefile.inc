ramstage-$(CONFIG_DRIVERS_OXFORD_OXPCIE) += oxpcie.c

ifeq ($(CONFIG_CONSOLE_SERIAL8250MEM),y)
romstage-$(CONFIG_DRIVERS_OXFORD_OXPCIE) += oxpcie_early.c
endif
