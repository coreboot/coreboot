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
		if (CONFIG(SOC_AMD_STONEYRIDGE)) {
			*mode = efs->spi_readmode_f15_mod_60_6f;
			*speed = efs->fast_speed_new_f15_mod_60_6f;
			ret = true;
		} else if (CONFIG(SOC_AMD_PICASSO)) {
			*mode = efs->spi_readmode_f17_mod_00_2f;
			*speed = efs->spi_fastspeed_f17_mod_00_2f;
			ret = true;
		} else if (CONFIG(SOC_AMD_CEZANNE) || CONFIG(SOC_AMD_MENDOCINO)) {
			*mode = efs->spi_readmode_f17_mod_30_3f;
			*speed = efs->spi_fastspeed_f17_mod_30_3f;
			ret = true;
		} else {
			printk(BIOS_ERR, "Unknown cpu in psp_efs.h\n");
			printk(BIOS_ERR, "SPI speed/mode not set.\n");
		}
	}
	rdev_munmap(boot_device_ro(), efs);
	return ret;
}
