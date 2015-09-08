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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <boot_device.h>
#include <cbfs.h>
#include <console/console.h>
#include <endian.h>
#include <commonlib/region.h>

/* This function is marked as weak to allow a particular platform to
 * override the logic. This implementation should work for most devices. */
int __attribute__((weak)) cbfs_boot_region_properties(struct cbfs_props *props)
{
	struct cbfs_header header;
	const struct region_device *bdev;
	int32_t rel_offset;
	size_t offset;

	bdev = boot_device_ro();

	if (bdev == NULL)
		return -1;

	/* Find location of header using signed 32-bit offset from
	 * end of CBFS region. */
	offset = CONFIG_CBFS_SIZE - sizeof(int32_t);
	if (rdev_readat(bdev, &rel_offset, offset, sizeof(int32_t)) < 0)
		return -1;

	offset = CONFIG_CBFS_SIZE + rel_offset;
	if (rdev_readat(bdev, &header, offset, sizeof(header)) < 0)
		return -1;

	header.magic = ntohl(header.magic);
	header.romsize = ntohl(header.romsize);
	header.offset = ntohl(header.offset);

	if (header.magic != CBFS_HEADER_MAGIC)
		return -1;

	props->offset = header.offset;
	props->size = header.romsize;
	props->size -= props->offset;

	printk(BIOS_SPEW, "CBFS @ %zx size %zx\n", props->offset, props->size);

	return 0;
}
