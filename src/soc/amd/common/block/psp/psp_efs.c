/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/psp_efs.h>
#include <boot_device.h>
#include <commonlib/region.h>
#include <device/mmio.h>
#include <types.h>

bool read_efs_spi_settings(uint8_t *mode, uint8_t *speed)
{
	bool ret = false;
	struct embedded_firmware *efs;

	efs = rdev_mmap(boot_device_ro(), EFS_OFFSET, sizeof(*efs));
	if (!efs)
		return false;

	if (efs->signature == EMBEDDED_FW_SIGNATURE) {
#ifndef SPI_MODE_FIELD
		printk(BIOS_ERR, "Unknown cpu in psp_efs.h\n");
		printk(BIOS_ERR, "SPI speed/mode not set.\n");
#else
		*mode = efs->SPI_MODE_FIELD;
		*speed = efs->SPI_SPEED_FIELD;
		ret = true;
#endif
	}
	rdev_munmap(boot_device_ro(), efs);
	return ret;
}
