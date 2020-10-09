/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <boot_device.h>

static const struct mem_region_device boot_dev =
	MEM_REGION_DEV_RO_INIT(FLASH_BASE_ADDR, CONFIG_ROM_SIZE);

const struct region_device *boot_device_ro(void)
{
	return &boot_dev.rdev;
}
