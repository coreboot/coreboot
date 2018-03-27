# SPI flash driver interface

subdirs-y += tpm

ifeq ($(CONFIG_SPI_CONSOLE),y)
ramstage-y += spiconsole.c
smm-$(CONFIG_DEBUG_SMI) += spiconsole.c
endif

ifeq ($(CONFIG_CONSOLE_SPI_FLASH),y)
bootblock-y += flashconsole.c
verstage-y += flashconsole.c
romstage-y += flashconsole.c
ramstage-y += flashconsole.c
postcar-y += flashconsole.c
smm-$(CONFIG_DEBUG_SMI) += flashconsole.c

endif

bootblock-y += spi-generic.c
bootblock-y += bitbang.c
bootblock-$(CONFIG_COMMON_CBFS_SPI_WRAPPER) += cbfs_spi.c
bootblock-$(CONFIG_SPI_FLASH) += spi_flash.c
bootblock-$(CONFIG_BOOT_DEVICE_SPI_FLASH_RW_NOMMAP_EARLY) += boot_device_rw_nommap.c
bootblock-$(CONFIG_SPI_FLASH_ADESTO) += adesto.c
bootblock-$(CONFIG_SPI_FLASH_AMIC) += amic.c
bootblock-$(CONFIG_SPI_FLASH_ATMEL) += atmel.c
bootblock-$(CONFIG_SPI_FLASH_EON) += eon.c
bootblock-$(CONFIG_SPI_FLASH_GIGADEVICE) += gigadevice.c
bootblock-$(CONFIG_SPI_FLASH_MACRONIX) += macronix.c
bootblock-$(CONFIG_SPI_FLASH_SPANSION) += spansion.c
bootblock-$(CONFIG_SPI_FLASH_SST) += sst.c
bootblock-$(CONFIG_SPI_FLASH_STMICRO) += stmicro.c
bootblock-$(CONFIG_SPI_FLASH_WINBOND) += winbond.c
bootblock-$(CONFIG_SPI_FRAM_RAMTRON) += ramtron.c

romstage-y += spi-generic.c
romstage-y += bitbang.c
romstage-$(CONFIG_COMMON_CBFS_SPI_WRAPPER) += cbfs_spi.c
romstage-$(CONFIG_SPI_FLASH) += spi_flash.c
romstage-$(CONFIG_BOOT_DEVICE_SPI_FLASH_RW_NOMMAP_EARLY) += boot_device_rw_nommap.c
romstage-$(CONFIG_SPI_FLASH_ADESTO) += adesto.c
romstage-$(CONFIG_SPI_FLASH_AMIC) += amic.c
romstage-$(CONFIG_SPI_FLASH_ATMEL) += atmel.c
romstage-$(CONFIG_SPI_FLASH_EON) += eon.c
romstage-$(CONFIG_SPI_FLASH_GIGADEVICE) += gigadevice.c
romstage-$(CONFIG_SPI_FLASH_MACRONIX) += macronix.c
romstage-$(CONFIG_SPI_FLASH_SPANSION) += spansion.c
romstage-$(CONFIG_SPI_FLASH_SST) += sst.c
romstage-$(CONFIG_SPI_FLASH_STMICRO) += stmicro.c
romstage-$(CONFIG_SPI_FLASH_WINBOND) += winbond.c
romstage-$(CONFIG_SPI_FRAM_RAMTRON) += ramtron.c

verstage-y += spi-generic.c
verstage-y += bitbang.c
verstage-$(CONFIG_COMMON_CBFS_SPI_WRAPPER) += cbfs_spi.c
verstage-$(CONFIG_SPI_FLASH) += spi_flash.c
verstage-$(CONFIG_BOOT_DEVICE_SPI_FLASH_RW_NOMMAP_EARLY) += boot_device_rw_nommap.c
verstage-$(CONFIG_SPI_FLASH_ADESTO) += adesto.c
verstage-$(CONFIG_SPI_FLASH_AMIC) += amic.c
verstage-$(CONFIG_SPI_FLASH_ATMEL) += atmel.c
verstage-$(CONFIG_SPI_FLASH_EON) += eon.c
verstage-$(CONFIG_SPI_FLASH_GIGADEVICE) += gigadevice.c
verstage-$(CONFIG_SPI_FLASH_MACRONIX) += macronix.c
verstage-$(CONFIG_SPI_FLASH_SPANSION) += spansion.c
verstage-$(CONFIG_SPI_FLASH_SST) += sst.c
verstage-$(CONFIG_SPI_FLASH_STMICRO) += stmicro.c
verstage-$(CONFIG_SPI_FLASH_WINBOND) += winbond.c
verstage-$(CONFIG_SPI_FRAM_RAMTRON) += ramtron.c

ramstage-y += spi-generic.c
ramstage-y += bitbang.c
ramstage-$(CONFIG_COMMON_CBFS_SPI_WRAPPER) += cbfs_spi.c
ramstage-$(CONFIG_SPI_FLASH) += spi_flash.c
ramstage-$(CONFIG_BOOT_DEVICE_SPI_FLASH_RW_NOMMAP) += boot_device_rw_nommap.c
ramstage-$(CONFIG_SPI_FLASH_ADESTO) += adesto.c
ramstage-$(CONFIG_SPI_FLASH_AMIC) += amic.c
ramstage-$(CONFIG_SPI_FLASH_ATMEL) += atmel.c
ramstage-$(CONFIG_SPI_FLASH_EON) += eon.c
ramstage-$(CONFIG_SPI_FLASH_GIGADEVICE) += gigadevice.c
ramstage-$(CONFIG_SPI_FLASH_MACRONIX) += macronix.c
ramstage-$(CONFIG_SPI_FLASH_SPANSION) += spansion.c
ramstage-$(CONFIG_SPI_FLASH_SST) += sst.c
ramstage-$(CONFIG_SPI_FLASH_STMICRO) += stmicro.c
ramstage-$(CONFIG_SPI_FLASH_WINBOND) += winbond.c
ramstage-$(CONFIG_SPI_FRAM_RAMTRON) += ramtron.c

ifeq ($(CONFIG_SPI_FLASH_SMM),y)
smm-y += spi-generic.c
smm-y += bitbang.c
# SPI flash driver interface
smm-$(CONFIG_SPI_FLASH) += spi_flash.c
smm-$(CONFIG_BOOT_DEVICE_SPI_FLASH_RW_NOMMAP) += boot_device_rw_nommap.c

# drivers
smm-$(CONFIG_SPI_FLASH_ADESTO) += adesto.c
smm-$(CONFIG_SPI_FLASH_AMIC) += amic.c
smm-$(CONFIG_SPI_FLASH_ATMEL) += atmel.c
smm-$(CONFIG_SPI_FLASH_EON) += eon.c
smm-$(CONFIG_SPI_FLASH_MACRONIX) += macronix.c
smm-$(CONFIG_SPI_FLASH_GIGADEVICE) += gigadevice.c
smm-$(CONFIG_SPI_FLASH_SPANSION) += spansion.c
smm-$(CONFIG_SPI_FLASH_SST) += sst.c
smm-$(CONFIG_SPI_FLASH_STMICRO) += stmicro.c
smm-$(CONFIG_SPI_FLASH_WINBOND) += winbond.c
smm-$(CONFIG_SPI_FRAM_RAMTRON) += ramtron.c
endif

postcar-y += spi-generic.c
postcar-y += bitbang.c
postcar-$(CONFIG_BOOT_DEVICE_SPI_FLASH_RW_NOMMAP_EARLY) += boot_device_rw_nommap.c
postcar-$(CONFIG_SPI_FLASH) += spi_flash.c
