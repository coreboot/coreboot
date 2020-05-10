/* SPDX-License-Identifier: GPL-2.0-only */
#include <boot_device.h>

/*
 * 0x80000000 is this start of RAM. We currently need to load coreboot.rom into
 * RAM on SPIKE, because SPIKE doesn't support loading custom code into the
 * boot ROM.
 */
static const struct mem_region_device boot_dev =
	MEM_REGION_DEV_RO_INIT(0x80000000, CONFIG_ROM_SIZE);

const struct region_device *boot_device_ro(void)
{
	return &boot_dev.rdev;
}
