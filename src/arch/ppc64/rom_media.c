/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot_device.h>

/* This assumes that the CBFS resides at 0x0, which is true for the default
 * configuration. */
static const struct mem_region_device boot_dev =
	MEM_REGION_DEV_RO_INIT(NULL, CONFIG_ROM_SIZE);

const struct region_device *boot_device_ro(void)
{
	return &boot_dev.rdev;
}
