# SPI flash driver interface
ramstage-$(CONFIG_SPI_FLASH) += spi_flash.c

# drivers
ramstage-$(CONFIG_SPI_FLASH_EON) += eon.c
ramstage-$(CONFIG_SPI_FLASH_MACRONIX) += macronix.c
ramstage-$(CONFIG_SPI_FLASH_SPANSION) += spansion.c
ramstage-$(CONFIG_SPI_FLASH_SST) += sst.c
ramstage-$(CONFIG_SPI_FLASH_STMICRO) += stmicro.c
ramstage-$(CONFIG_SPI_FLASH_WINBOND) += winbond.c
ramstage-$(CONFIG_SPI_FLASH_GIGADEVICE) += gigadevice.c
ramstage-$(CONFIG_SPI_FRAM_RAMTRON) += ramtron.c

ifeq ($(CONFIG_SPI_FLASH_SMM),y)
# SPI flash driver interface
smm-$(CONFIG_SPI_FLASH) += spi_flash.c

# drivers
smm-$(CONFIG_SPI_FLASH_EON) += eon.c
smm-$(CONFIG_SPI_FLASH_MACRONIX) += macronix.c
smm-$(CONFIG_SPI_FLASH_SPANSION) += spansion.c
smm-$(CONFIG_SPI_FLASH_SST) += sst.c
smm-$(CONFIG_SPI_FLASH_STMICRO) += stmicro.c
smm-$(CONFIG_SPI_FLASH_WINBOND) += winbond.c
smm-$(CONFIG_SPI_FRAM_RAMTRON) += ramtron.c
smm-$(CONFIG_SPI_FLASH_GIGADEVICE) += gigadevice.c
endif