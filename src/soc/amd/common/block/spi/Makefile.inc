ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_SPI),y)

bootblock-y += fch_spi_ctrl.c
bootblock-y += fch_spi_flash.c
bootblock-y += fch_spi_special.c
bootblock-y += fch_spi_table.c
romstage-y += fch_spi_ctrl.c
romstage-y += fch_spi_flash.c
romstage-y += fch_spi_special.c
romstage-y += fch_spi_table.c
verstage-y += fch_spi_ctrl.c
verstage-y += fch_spi_flash.c
verstage-y += fch_spi_special.c
verstage-y += fch_spi_table.c
postcar-y += fch_spi_ctrl.c
postcar-y += fch_spi_flash.c
postcar-y += fch_spi_special.c
postcar-y += fch_spi_table.c
ramstage-y += fch_spi_ctrl.c
ramstage-y += fch_spi_flash.c
ramstage-y += fch_spi_special.c
ramstage-y += fch_spi_table.c
ifeq ($(CONFIG_SPI_FLASH_SMM),y)
smm-y += fch_spi_ctrl.c
smm-y += fch_spi_flash.c
smm-y += fch_spi_special.c
smm-y += fch_spi_table.c
endif

endif
