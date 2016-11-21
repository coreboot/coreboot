/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * This file provides a common CBFS wrapper for SPI storage. SPI driver
 * context is expanded with the buffer descriptor used to store data read from
 * SPI.
 */

#include <boot_device.h>
#include <console/console.h>
#include <spi_flash.h>
#include <symbols.h>
#include <cbmem.h>
#include <timer.h>

static struct spi_flash *spi_flash_info;

/*
 * Set this to 1 to debug SPI speed, 0 to disable it
 * The format is:
 *
 * read SPI 62854 7db7: 10416 us, 3089 KB/s, 24.712 Mbps
 *
 * The important number is the last one. It should roughly match your SPI
 * clock. If it doesn't, your driver might need a little tuning.
 */
#define SPI_SPEED_DEBUG		0

static ssize_t spi_readat(const struct region_device *rd, void *b,
				size_t offset, size_t size)
{
	struct stopwatch sw;
	bool show = SPI_SPEED_DEBUG && size >= 4 * KiB;

	if (show)
		stopwatch_init(&sw);
	if (spi_flash_read(spi_flash_info, offset, size, b))
		return -1;
	if (show) {
		long usecs;

		usecs = stopwatch_duration_usecs(&sw);
		u64 speed;	/* KiB/s */
		int bps;	/* Bits per second */

		speed = (u64)size * 1000 / usecs;
		bps = speed * 8;

		printk(BIOS_DEBUG, "read SPI %#zx %#zx: %ld us, %lld KB/s, %d.%03d Mbps\n",
		       offset, size, usecs, speed, bps / 1000, bps % 1000);
	}
	return size;
}

static ssize_t spi_writeat(const struct region_device *rd, const void *b,
				size_t offset, size_t size)
{
	if (spi_flash_write(spi_flash_info, offset, size, b))
		return -1;
	return size;
}

static ssize_t spi_eraseat(const struct region_device *rd,
				size_t offset, size_t size)
{
	if (spi_flash_erase(spi_flash_info, offset, size))
		return -1;
	return size;
}

/* Provide all operations on the same device. */
static const struct region_device_ops spi_ops = {
	.mmap = mmap_helper_rdev_mmap,
	.munmap = mmap_helper_rdev_munmap,
	.readat = spi_readat,
	.writeat = spi_writeat,
	.eraseat = spi_eraseat,
};

static struct mmap_helper_region_device mdev =
	MMAP_HELPER_REGION_INIT(&spi_ops, 0, CONFIG_ROM_SIZE);

static void switch_to_postram_cache(int unused)
{
	/*
	 * Call boot_device_init() to ensure spi_flash is initialized before
	 * backing mdev with postram cache. This prevents the mdev backing from
	 * being overwritten if spi_flash was not accessed before dram was up.
	 */
	boot_device_init();
	if (_preram_cbfs_cache != _postram_cbfs_cache)
		mmap_helper_device_init(&mdev, _postram_cbfs_cache,
					_postram_cbfs_cache_size);
}
ROMSTAGE_CBMEM_INIT_HOOK(switch_to_postram_cache);

void boot_device_init(void)
{
	int bus = CONFIG_BOOT_DEVICE_SPI_FLASH_BUS;
	int cs = 0;

	if (spi_flash_info != NULL)
		return;

	spi_flash_info = spi_flash_probe(bus, cs);

	mmap_helper_device_init(&mdev, _cbfs_cache, _cbfs_cache_size);
}

/* Return the CBFS boot device. */
const struct region_device *boot_device_ro(void)
{
	if (spi_flash_info == NULL)
		return NULL;

	return &mdev.rdev;
}

/* The read-only and read-write implementations are symmetric. */
const struct region_device *boot_device_rw(void)
{
	return boot_device_ro();
}
