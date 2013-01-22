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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */
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
	// Some address (ex, pointer to master header) may be given in memory
	// mapped location. To workaround that, we handle >0xf0000000 as real
	// memory pointer.

	if ((uint32_t)offset > (uint32_t)0xf0000000)
		ptr = (void*)offset;
	else
		ptr = (void*)(0 - (uint32_t)media->context + offset);
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

int init_x86rom_cbfs_media(struct cbfs_media *media);
int init_x86rom_cbfs_media(struct cbfs_media *media) {
	// On X86, we always keep a reference of pointer to CBFS header in
	// 0xfffffffc, and the pointer is still a memory-mapped address.
	// Since the CBFS core always use ROM offset, we need to figure out
	// header->romsize even before media is initialized.
	struct cbfs_header *header = (struct cbfs_header*)
			*(uint32_t*)(0xfffffffc);
	if (CBFS_HEADER_MAGIC != ntohl(header->magic)) {
#if defined(CONFIG_ROM_SIZE)
		printk(BIOS_ERR, "Invalid CBFS master header at %p\n", header);
		media->context = (void*)CONFIG_ROM_SIZE;
#else
		return -1;
#endif
	} else {
		uint32_t romsize = ntohl(header->romsize);
		media->context = (void*)romsize;
#if defined(CONFIG_ROM_SIZE)
		if (CONFIG_ROM_SIZE != romsize)
			printk(BIOS_INFO, "Warning: rom size unmatch (%d/%d)\n",
			       CONFIG_ROM_SIZE, romsize);
#endif
	}
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
