/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_BACKUP_BOOT_DEVICE_H
#define AMD_BLOCK_BACKUP_BOOT_DEVICE_H

#include <commonlib/region.h>
#include <spi_flash.h>

enum boot_device {
	FLASH_PRIMARY, // primary SPI flash
	FLASH_BACKUP,  // backup  SPI flash
};

/* Retrieve the SPI CS index of the backup boot device. */
int backup_boot_device_spi_cs(void);

const struct region_device *backup_boot_device_rw(void);
const struct spi_flash *backup_boot_device_spi_flash(void);
int backup_boot_device_rw_subregion(const struct region *sub,
				    struct region_device *subrd);
int backup_boot_device_sync_subregion(const struct region *sub,
				      const bool direction_prim_to_sec);

#endif
