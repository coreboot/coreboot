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
#include <cbfs.h>
#include <string.h>
#include <console/console.h>

/* Simple memory-mapped ROM emulation. */

static int emu_rom_open(struct cbfs_media *media) {
	return 0;
}

static void *emu_rom_map(struct cbfs_media *media, size_t offset, size_t count) {
        return (void*)(offset + CONFIG_BOOTBLOCK_BASE);
}

static void *emu_rom_unmap(struct cbfs_media *media, const void *address) {
	return NULL;
}

static size_t emu_rom_read(struct cbfs_media *media, void *dest, size_t offset,
			   size_t count) {
	void *ptr = emu_rom_map(media, offset, count);
	memcpy(dest, ptr, count);
	emu_rom_unmap(media, ptr);
	return count;
}

static int emu_rom_close(struct cbfs_media *media) {
	return 0;
}

int init_emu_rom_cbfs_media(struct cbfs_media *media);
int init_emu_rom_cbfs_media(struct cbfs_media *media) {
	media->open = emu_rom_open;
	media->close = emu_rom_close;
	media->map = emu_rom_map;
	media->unmap = emu_rom_unmap;
	media->read = emu_rom_read;
	return 0;
}

int init_default_cbfs_media(struct cbfs_media *media) {
	return init_emu_rom_cbfs_media(media);
}
