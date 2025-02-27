/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot_device.h>
#include <endian.h>
#include <spi_flash.h>

#if CONFIG_ROM_SIZE >= (16 * MiB)
#define ROM_SIZE (16 * MiB)
#else
#define ROM_SIZE CONFIG_ROM_SIZE
#endif

/* The ROM is memory mapped just below 4GiB. Form a pointer for the base. */
#define rom_base ((void *)(uintptr_t)(0x100000000ULL-ROM_SIZE))

static const struct mem_region_device boot_dev =
	MEM_REGION_DEV_RO_INIT(rom_base, ROM_SIZE);

const struct region_device *boot_device_ro(void)
{
	return &boot_dev.rdev;
}

uint32_t spi_flash_get_mmap_windows(struct flash_mmap_window *table)
{
	table->flash_base = 0;
	table->host_base = (uint32_t)(uintptr_t)rom_base;
	table->size = ROM_SIZE;

	return 1;
}
