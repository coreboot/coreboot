# SPI flash driver interface

subdirs-y += tpm

ifeq ($(CONFIG_EM100PRO_SPI_CONSOLE),y)
ramstage-y += spiconsole.c
smm-y += spiconsole.c
endif

# add SPI drivers per stage
# $1 stage name
# $2 optional _EARLY suffix for CONFIG_BOOT_DEVICE_SPI_FLASH_RW_NOMMAP(_EARLY)
define add_spi_stage
$(1)-y += spi-generic.c
$(1)-y += bitbang.c
$(1)-$(CONFIG_COMMON_CBFS_SPI_WRAPPER) += cbfs_spi.c
$(1)-$(CONFIG_SPI_FLASH) += spi_flash.c
$(1)-$(CONFIG_SPI_SDCARD) += spi_sdcard.c
$(1)-$(CONFIG_BOOT_DEVICE_SPI_FLASH_RW_NOMMAP$(2)) += boot_device_rw_nommap.c
$(1)-$(CONFIG_CONSOLE_SPI_FLASH) += flashconsole.c
$(1)-$(CONFIG_SPI_FLASH_ADESTO) += adesto.c
$(1)-$(CONFIG_SPI_FLASH_AMIC) += amic.c
$(1)-$(CONFIG_SPI_FLASH_ATMEL) += atmel.c
$(1)-$(CONFIG_SPI_FLASH_EON) += eon.c
$(1)-$(CONFIG_SPI_FLASH_GIGADEVICE) += gigadevice.c
$(1)-$(CONFIG_SPI_FLASH_MACRONIX) += macronix.c
$(1)-$(CONFIG_SPI_FLASH_SPANSION) += spansion.c
$(1)-$(CONFIG_SPI_FLASH_SST) += sst.c
$(1)-$(CONFIG_SPI_FLASH_STMICRO) += stmicro.c
$(1)-$(CONFIG_SPI_FLASH_WINBOND) += winbond.c
$(1)-$(CONFIG_SPI_FRAM_RAMTRON) += ramtron.c
endef

$(eval $(call add_spi_stage,bootblock,_EARLY))
$(eval $(call add_spi_stage,romstage,_EARLY))
$(eval $(call add_spi_stage,verstage,_EARLY))
$(eval $(call add_spi_stage,postcar,_EARLY))
$(eval $(call add_spi_stage,ramstage))
ifeq ($(CONFIG_SPI_FLASH_SMM),y)
$(eval $(call add_spi_stage,smm))
endif
