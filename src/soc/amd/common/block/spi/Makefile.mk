## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_SPI),y)

all_x86-$(CONFIG_SOC_AMD_COMMON_BLOCK_SPI_MMAP) += mmap_boot.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_SPI_MMAP) += mmap_boot.c

all-$(CONFIG_SOC_AMD_COMMON_BLOCK_SPI_MMAP_USE_ROM3) += mmap_boot_rom3.c
smm-$(CONFIG_SOC_AMD_COMMON_BLOCK_SPI_MMAP_USE_ROM3) += mmap_boot_rom3.c

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

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_SPI_BACKUP_SPI_FLASH),y)
all_x86-y += backup_boot_device_rw_nommap.c
smm-$(CONFIG_SPI_FLASH_SMM) += backup_boot_device_rw_nommap.c
endif # CONFIG_SOC_AMD_COMMON_BLOCK_SPI_BACKUP_SPI_FLASH

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_PSP_ROM_ARMOR3),y)
all_x86-y += rom_armor_boot_device_rw_nommap.c
smm-y += rom_armor_boot_device_rw_nommap.c
endif # CONFIG_SOC_AMD_COMMON_BLOCK_PSP_ROM_ARMOR3

endif # CONFIG_SOC_AMD_COMMON_BLOCK_SPI

ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_SPI_HID2) += hid2.c
