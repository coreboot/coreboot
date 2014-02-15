ifeq ($(CONFIG_CONSOLE_SERIAL8250MEM),y)
ramstage-y += oxpcie_early.c oxpcie.c
romstage-y += oxpcie_early.c
endif
