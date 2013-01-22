/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <stdint.h>
#include <string.h>
#include <cbfs.h>

// Implementation of memory-mapped ROM media source on X86.

static int x86_rom_open(struct cbfs_media *media) {
	// TODO cache rom size by searching for cbfs_header->romsize.
	return 0;
}

static void *x86_rom_map(struct cbfs_media *media, size_t offset, size_t count) {
	void *ptr = (void*)((uint32_t)0x0 - CONFIG_ROM_SIZE + offset);
	return ptr;
}

static void *x86_rom_unmap(struct cbfs_media *media, const void *address) {
	return NULL;
}

static size_t x86_rom_read(struct cbfs_media *media, void *dest, size_t offset,
			   size_t count) {
	void *ptr = x86_rom_map(media, offset, count);
	memcpy(dest, ptr, count);
	x86_rom_unmap(media, ptr);
	return count;
}

static int x86_rom_close(struct cbfs_media *media) {
	return 0;
}

int init_default_cbfs_media(struct cbfs_media *media) {
	media->context = (void*)CONFIG_ROM_SIZE;
	media->open = x86_rom_open;
	media->close = x86_rom_close;
	media->map = x86_rom_map;
	media->unmap = x86_rom_unmap;
	media->read = x86_rom_read;
	return 0;
}
