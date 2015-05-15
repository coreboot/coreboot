/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 The Chromium OS Authors. All rights reserved.
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
#include <string.h>

#ifdef LIBPAYLOAD
# define printk(x...)
# define init_default_cbfs_media libpayload_init_default_cbfs_media
  extern int libpayload_init_default_cbfs_media(struct cbfs_media *media);
#else
# include <console/console.h>
#endif

// Implementation of memory-mapped ROM media source on X86.

static int x86_rom_open(struct cbfs_media *media) {
	return 0;
}

static void *x86_rom_map(struct cbfs_media *media, size_t offset, size_t count) {
	void *ptr;
	const struct region_device *boot_dev;

	boot_dev = media->context;

	/* Extremely large offsets are considered relative to end of region. */
	if ((uint32_t)offset > (uint32_t)0xf0000000)
		offset += region_device_sz(boot_dev);

	ptr = rdev_mmap(boot_dev, offset, count);

	if (ptr == NULL)
		return (void *)-1;

	return ptr;
}

static void *x86_rom_unmap(struct cbfs_media *media, const void *address) {
	return NULL;
}

static size_t x86_rom_read(struct cbfs_media *media, void *dest, size_t offset,
			   size_t count) {
	void *ptr;

	ptr = x86_rom_map(media, offset, count);

	if (ptr == (void *)-1)
		return 0;

	memcpy(dest, ptr, count);
	x86_rom_unmap(media, ptr);
	return count;
}

static int x86_rom_close(struct cbfs_media *media) {
	return 0;
}

static int init_x86rom_cbfs_media(struct cbfs_media *media) {
	boot_device_init();

	media->context = (void *)boot_device_ro();

	if (media->context == NULL)
		return -1;

	media->open = x86_rom_open;
	media->close = x86_rom_close;
	media->map = x86_rom_map;
	media->unmap = x86_rom_unmap;
	media->read = x86_rom_read;
	return 0;
}

int init_default_cbfs_media(struct cbfs_media *media) {
	return init_x86rom_cbfs_media(media);
}
