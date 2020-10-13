/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_SAMSUNG_COMMON_INCLUDE_SOC_ALTERNATE_CBFS_H
#define SOC_SAMSUNG_COMMON_INCLUDE_SOC_ALTERNATE_CBFS_H

/* These are pointers to function pointers. Double indirection! */
static void **const irom_sdmmc_read_blocks_ptr = (void **)0x02020030;
static void **const irom_msh_read_from_fifo_emmc_ptr = (void **)0x02020044;
static void **const irom_msh_end_boot_op_emmc_ptr = (void **)0x02020048;
static void **const irom_spi_sf_read_ptr = (void **)0x02020058;
static void **const irom_load_image_from_usb_ptr = (void **)0x02020070;

#define SECONDARY_BASE_BOOT_USB 0xfeed0002
static u32 *const iram_secondary_base = (u32 *)0x02020018;

/* Values pulled from U-Boot, I think the manual is wrong here (for SPI) */
#define OM_STAT_SDMMC 0x4
#define OM_STAT_EMMC  0x8
#define OM_STAT_SPI  0x14
#define OM_STAT_MASK 0x7f

#endif /* SOC_SAMSUNG_COMMON_INCLUDE_SOC_ALTERNATE_CBFS_H */
