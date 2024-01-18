## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_SPI),y)

bootblock-y += fch_spi_ctrl.c
romstage-y += fch_spi_ctrl.c
verstage-y += fch_spi_ctrl.c
postcar-y += fch_spi_ctrl.c
ramstage-y += fch_spi_ctrl.c
smm-$(CONFIG_SPI_FLASH_SMM) += fch_spi_ctrl.c

bootblock-y += fch_spi.c
romstage-y += fch_spi.c
postcar-y += fch_spi.c
ramstage-y += fch_spi.c
verstage-y += fch_spi.c
smm-$(CONFIG_SPI_FLASH_SMM) += fch_spi.c

bootblock-y += fch_spi_util.c
romstage-y += fch_spi_util.c
postcar-y += fch_spi_util.c
ramstage-y += fch_spi_util.c
verstage-y += fch_spi_util.c
smm-$(CONFIG_SPI_FLASH_SMM) += fch_spi_util.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_SPI
