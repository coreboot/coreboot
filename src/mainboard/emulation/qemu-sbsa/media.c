/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boot_device.h>

/* Maps directly to NOR flash up to ROM size. */
static const struct mem_region_device boot_dev =
	MEM_REGION_DEV_RO_INIT((void *)0x10000000, CONFIG_ROM_SIZE);

const struct region_device *boot_device_ro(void)
{
	return &boot_dev.rdev;
}
