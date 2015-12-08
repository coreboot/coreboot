/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#include <boot_device.h>
#include <cbfs.h>
#include <endian.h>
#include <stdlib.h>

/* The ROM is memory mapped just below 4GiB. Form a pointer for the base. */
#define rom_base ((void *)(uintptr_t)(0x100000000ULL-CONFIG_ROM_SIZE))

static const struct mem_region_device boot_dev =
	MEM_REGION_DEV_INIT(rom_base, CONFIG_ROM_SIZE);

const struct region_device *boot_device_ro(void)
{
	return &boot_dev.rdev;
}

static int cbfs_master_header_props(struct cbfs_props *props)
{
	struct cbfs_header header;
	int32_t offset;
	const struct region_device *bdev;

	bdev = boot_device_ro();

	rdev_readat(bdev, &offset, CONFIG_ROM_SIZE - sizeof(offset),
			sizeof(offset));

	/* The offset is relative to the end of the media. */
	offset += CONFIG_ROM_SIZE;

	rdev_readat(bdev, &header , offset, sizeof(header));

	header.magic = ntohl(header.magic);
	header.romsize = ntohl(header.romsize);
	header.bootblocksize = ntohl(header.bootblocksize);
	header.offset = ntohl(header.offset);

	if (header.magic != CBFS_HEADER_MAGIC)
		return -1;

	props->offset = header.offset;
	if (CONFIG_ROM_SIZE != header.romsize)
		props->size = CONFIG_ROM_SIZE;
	else
		props->size = header.romsize;
	props->size -= props->offset;
	props->size -= header.bootblocksize;
	props->size = ALIGN_DOWN(props->size, 64);

	return 0;
}

const struct cbfs_locator cbfs_master_header_locator = {
	.name = "Master Header Locator",
	.locate = cbfs_master_header_props,
};
