ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_SPI),y)

bootblock-y += fch_spi_ctrl.c
romstage-y += fch_spi_ctrl.c
verstage-y += fch_spi_ctrl.c
postcar-y += fch_spi_ctrl.c
ramstage-y += fch_spi_ctrl.c
ifeq ($(CONFIG_SPI_FLASH_SMM),y)
smm-y += fch_spi_ctrl.c
endif

endif
