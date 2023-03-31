## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_LPC),y)

ramstage-y += lpc.c

all_x86-y += lpc_util.c
smm-y += lpc_util.c

endif # CONFIG_SOC_AMD_COMMON_BLOCK_LPC

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_LPC_SPI_DMA),y)
all_x86-y += spi_dma.c
smm-y += spi_dma.c
endif # CONFIG_SOC_AMD_COMMON_BLOCK_LPC_SPI_DMA

all-$(CONFIG_SOC_AMD_COMMON_BLOCK_USE_ESPI) += espi_util.c
