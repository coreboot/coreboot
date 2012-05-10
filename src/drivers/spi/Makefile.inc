# SPI flash driver interface
ramstage-$(CONFIG_SPI_FLASH) += spi_flash.c

# drivers
ramstage-$(CONFIG_SPI_FLASH_EON) += eon.c
ramstage-$(CONFIG_SPI_FLASH_MACRONIX) += macronix.c
ramstage-$(CONFIG_SPI_FLASH_SPANSION) += spansion.c
ramstage-$(CONFIG_SPI_FLASH_SST) += sst.c
ramstage-$(CONFIG_SPI_FLASH_STMICRO) += stmicro.c
ramstage-$(CONFIG_SPI_FLASH_WINBOND) += winbond.c
ramstage-$(CONFIG_SPI_FRAM_RAMTRON) += ramtron.c

