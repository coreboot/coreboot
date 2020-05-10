/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot_device.h>
#include <endian.h>

/* The ROM is memory mapped just below 4GiB. Form a pointer for the base. */
#define rom_base ((void *)(uintptr_t)(0x100000000ULL-CONFIG_ROM_SIZE))

static const struct mem_region_device boot_dev =
	MEM_REGION_DEV_RO_INIT(rom_base, CONFIG_ROM_SIZE);

const struct region_device *boot_device_ro(void)
{
	return &boot_dev.rdev;
}
