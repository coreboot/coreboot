/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <boot_device.h>
#include <cbfs.h>
#include <string.h>
#include <symbols.h>
#include <console/console.h>

/* Maps directly to qemu memory mapped space of 0x10000 up to rom size. */
static const struct mem_region_device gboot_dev =
	MEM_REGION_DEV_INIT((void *)0x10000, CONFIG_ROM_SIZE);

const struct region_device *boot_device_ro(void)
{
	return &gboot_dev.rdev;
}

static int emu_rom_open(struct cbfs_media *media)
{
	return 0;
}

static void *emu_rom_map(struct cbfs_media *media, size_t offset, size_t count)
{
	const struct region_device *boot_dev;
	void *ptr;

	boot_dev = media->context;

	ptr = rdev_mmap(boot_dev, offset, count);

	if (ptr == NULL)
		return (void *)-1;

	return ptr;
}

static void *emu_rom_unmap(struct cbfs_media *media, const void *address)
{
	const struct region_device *boot_dev;

	boot_dev = media->context;

	rdev_munmap(boot_dev, (void *)address);

	return NULL;
}

static size_t emu_rom_read(struct cbfs_media *media, void *dest, size_t offset,
			   size_t count)
{
	const struct region_device *boot_dev;

	boot_dev = media->context;

	if (rdev_readat(boot_dev, dest, offset, count) < 0)
		return 0;

	return count;
}

static int emu_rom_close(struct cbfs_media *media)
{
	return 0;
}

static int init_emu_rom_cbfs_media(struct cbfs_media *media)
{
	boot_device_init();

	media->context = (void *)boot_device_ro();
	media->open = emu_rom_open;
	media->close = emu_rom_close;
	media->map = emu_rom_map;
	media->unmap = emu_rom_unmap;
	media->read = emu_rom_read;
	return 0;
}

int init_default_cbfs_media(struct cbfs_media *media)
{
	return init_emu_rom_cbfs_media(media);
}
