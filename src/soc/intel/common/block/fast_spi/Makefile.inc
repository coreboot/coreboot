bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi.c
bootblock-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi_flash.c

verstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi.c
verstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi_flash.c

romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi.c
romstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi_flash.c

ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi.c
ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi_flash.c

postcar-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi.c
postcar-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi_flash.c

ifeq ($(CONFIG_SPI_FLASH_SMM),y)
smm-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi.c
smm-$(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI) += fast_spi_flash.c
endif

CPPFLAGS_common += -I$(src)/soc/intel/common/block/fast_spi
