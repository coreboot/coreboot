/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
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
