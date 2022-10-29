## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_LPC),y)

ramstage-y += lpc.c

bootblock-y += lpc_util.c
verstage_x86-y += lpc_util.c
romstage-y += lpc_util.c
postcar-y += lpc_util.c
ramstage-y += lpc_util.c
smm-y += lpc_util.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_LPC

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_LPC_SPI_DMA),y)
bootblock-y += spi_dma.c
verstage_x86-y += spi_dma.c
romstage-y += spi_dma.c
ramstage-y += spi_dma.c
smm-y += spi_dma.c
endif # CONFIG_SOC_AMD_COMMON_BLOCK_LPC_SPI_DMA

all-$(CONFIG_SOC_AMD_COMMON_BLOCK_USE_ESPI) += espi_util.c
