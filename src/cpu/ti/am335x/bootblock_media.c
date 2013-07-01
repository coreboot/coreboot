/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <cbfs.h>
#include <console/console.h>
#include <string.h>

static int dummy_open(struct cbfs_media *media)
{
	return 0;
}

static int dummy_close(struct cbfs_media *media)
{
	return 0;
}

static void * on_chip_memory_map(struct cbfs_media *media, size_t offset,
				 size_t count)
{
	return (void *)((uintptr_t)CONFIG_BOOTBLOCK_BASE + offset);
}

static void * dummy_unmap(struct cbfs_media *media, const void *address)
{
	return NULL;
}

static size_t on_chip_memory_read(struct cbfs_media *media, void *dest,
				  size_t offset, size_t count)
{
	void *ptr = media->map(media, offset, count);
	memcpy(dest, ptr, count);
	media->unmap(media, ptr);
	return count;
}

int init_default_cbfs_media(struct cbfs_media *media)
{
	struct cbfs_header *header =
		(struct cbfs_header *)((uintptr_t)CONFIG_BOOTBLOCK_BASE +
				       CONFIG_CBFS_HEADER_ROM_OFFSET);

	if (CBFS_HEADER_MAGIC != ntohl(header->magic)) {
		printk(BIOS_ERR, "Invalid CBFS master header at %p\n", header);
		return -1;
	}

	media->open = dummy_open;
	media->close = dummy_close;
	media->map = on_chip_memory_map;
	media->unmap = dummy_unmap;
	media->read = on_chip_memory_read;

	return 0;
}
