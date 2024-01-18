## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_INTEL_COMMON_BLOCK_SPI),y)
bootblock-y += spi.c

verstage-y += spi.c

romstage-y += spi.c

ramstage-y += spi.c

postcar-y += spi.c

smm-$(CONFIG_SPI_FLASH_SMM) += spi.c
endif
