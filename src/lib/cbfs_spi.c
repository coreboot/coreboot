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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

/*
 * This file provides a common CBFS wrapper for SPI storage. SPI driver
 * context is expanded with the buffer descriptor used to store data read from
 * SPI.
 */

#include <boot_device.h>
#include <cbfs.h>
#include <region.h>
#include <spi_flash.h>
#include <symbols.h>

static struct spi_flash *spi_flash_info;

static ssize_t spi_readat(const struct region_device *rd, void *b,
				size_t offset, size_t size)
{
	if (spi_flash_info->read(spi_flash_info, offset, size, b))
		return -1;
	return size;
}

static const struct region_device_ops spi_ops = {
	.mmap = mmap_helper_rdev_mmap,
	.munmap = mmap_helper_rdev_munmap,
	.readat = spi_readat,
};

static struct mmap_helper_region_device mdev =
	MMAP_HELPER_REGION_INIT(&spi_ops, 0, CONFIG_ROM_SIZE);

void boot_device_init(void)
{
	int bus = CONFIG_BOOT_MEDIA_SPI_BUS;
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

static int cbfs_media_open(struct cbfs_media *media)
{
	return 0;
}

static int cbfs_media_close(struct cbfs_media *media)
{
	return 0;
}

static size_t cbfs_media_read(struct cbfs_media *media,
			      void *dest, size_t offset,
			      size_t count)
{
	const struct region_device *boot_dev;

	boot_dev = media->context;

	if (rdev_readat(boot_dev, dest, offset, count) < 0)
		return 0;

	return count;
}

static void *cbfs_media_map(struct cbfs_media *media,
			    size_t offset, size_t count)
{
	const struct region_device *boot_dev;
	void *ptr;

	boot_dev = media->context;

	ptr = rdev_mmap(boot_dev, offset, count);

	if (ptr == NULL)
		return (void *)-1;

	return ptr;
}

static void *cbfs_media_unmap(struct cbfs_media *media,
			       const void *address)
{
	const struct region_device *boot_dev;

	boot_dev = media->context;

	rdev_munmap(boot_dev, (void *)address);

	return NULL;
}

int init_default_cbfs_media(struct cbfs_media *media)
{
	boot_device_init();

	media->context = (void *)boot_device_ro();

	if (media->context == NULL)
		return -1;

	media->open = cbfs_media_open;
	media->close = cbfs_media_close;
	media->read = cbfs_media_read;
	media->map = cbfs_media_map;
	media->unmap = cbfs_media_unmap;

	return 0;
}
