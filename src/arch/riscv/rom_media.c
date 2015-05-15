/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */
#include <boot_device.h>
#include <cbfs.h>
#include <console/console.h>
#include <string.h>

/* This assumes that the CBFS resides at 0x0, which is true for the default
 * configuration. */
static const struct mem_region_device gboot_dev =
	MEM_REGION_DEV_INIT(NULL, CONFIG_ROM_SIZE);

const struct region_device *boot_device_ro(void)
{
	return &gboot_dev.rdev;
}

static int rom_media_open(struct cbfs_media *media) {
	return 0;
}

static void *rom_media_map(struct cbfs_media *media, size_t offset, size_t count) {
	const struct region_device *boot_dev;
	void *ptr;

	printk(BIOS_INFO, "%s: media %p, offset %lx, size %ld.\n", __func__, media, offset, count);
	boot_dev = media->context;

	ptr = rdev_mmap(boot_dev, offset, count);

	if (ptr == NULL)
		return (void *)-1;

	return ptr;
}

static void *rom_media_unmap(struct cbfs_media *media, const void *address) {
	const struct region_device *boot_dev;

	boot_dev = media->context;

	rdev_munmap(boot_dev, (void *)address);

	return NULL;
}

static size_t rom_media_read(struct cbfs_media *media, void *dest, size_t offset,
			   size_t count) {
	const struct region_device *boot_dev;

	boot_dev = media->context;

	if (rdev_readat(boot_dev, dest, offset, count) < 0)
		return 0;

	return count;
}

static int rom_media_close(struct cbfs_media *media) {
	return 0;
}

static int init_rom_media_cbfs(struct cbfs_media *media) {
	boot_device_init();
	media->context = (void *)boot_device_ro();
	media->open = rom_media_open;
	media->close = rom_media_close;
	media->map = rom_media_map;
	media->unmap = rom_media_unmap;
	media->read = rom_media_read;
	return 0;
}

int init_default_cbfs_media(struct cbfs_media *media) {
	return init_rom_media_cbfs(media);
}
