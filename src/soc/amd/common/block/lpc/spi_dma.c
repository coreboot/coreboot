/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot_device.h>
#include <commonlib/helpers.h>
#include <commonlib/region.h>
#include <spi_flash.h>
#include <string.h>
#include <types.h>

/* The ROM is memory mapped just below 4GiB. Form a pointer for the base. */
#define rom_base ((void *)(uintptr_t)(0x100000000ULL - CONFIG_ROM_SIZE))

static void *spi_dma_mmap(const struct region_device *rd, size_t offset, size_t size __unused)
{
	const struct mem_region_device *mdev;

	mdev = container_of(rd, __typeof__(*mdev), rdev);

	return &mdev->base[offset];
}

static int spi_dma_munmap(const struct region_device *rd __unused, void *mapping __unused)
{
	return 0;
}

static ssize_t spi_dma_readat_mmap(const struct region_device *rd, void *b, size_t offset,
				   size_t size)
{
	const struct mem_region_device *mdev;

	mdev = container_of(rd, __typeof__(*mdev), rdev);

	memcpy(b, &mdev->base[offset], size);

	return size;
}

const struct region_device_ops spi_dma_rdev_ro_ops = {
	.mmap = spi_dma_mmap,
	.munmap = spi_dma_munmap,
	.readat = spi_dma_readat_mmap,
};

static const struct mem_region_device boot_dev = {
	.base = rom_base,
	.rdev = REGION_DEV_INIT(&spi_dma_rdev_ro_ops, 0, CONFIG_ROM_SIZE),
};

const struct region_device *boot_device_ro(void)
{
	return &boot_dev.rdev;
}

uint32_t spi_flash_get_mmap_windows(struct flash_mmap_window *table)
{
	table->flash_base = 0;
	table->host_base = (uint32_t)(uintptr_t)rom_base;
	table->size = CONFIG_ROM_SIZE;

	return 1;
}
