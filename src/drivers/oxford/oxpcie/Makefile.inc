ifeq ($(CONFIG_CONSOLE_SERIAL),y)
ramstage-y += oxpcie_early.c oxpcie.c
romstage-y += oxpcie_early.c
endif
