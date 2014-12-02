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

static int rom_media_open(struct cbfs_media *media) {
	return 0;
}

static void *rom_media_map(struct cbfs_media *media, size_t offset, size_t count) {
	void *ptr;
	printk(BIOS_INFO, "%s: media %p, offset %lx, size %ld.\n", __func__, media, offset, count);

	ptr = (void*)offset;
	return ptr;
}

static void *rom_media_unmap(struct cbfs_media *media, const void *address) {
	return NULL;
}

static size_t rom_media_read(struct cbfs_media *media, void *dest, size_t offset,
			   size_t count) {
	void *ptr = rom_media_map(media, offset, count);
	memcpy(dest, ptr, count);
	rom_media_unmap(media, ptr);
	return count;
}

static int rom_media_close(struct cbfs_media *media) {
	return 0;
}

static int init_rom_media_cbfs(struct cbfs_media *media) {
	//extern unsigned long _cbfs_master_header;
	// On X86, we always keep a reference of pointer to CBFS header in
	// 0xfffffffc, and the pointer is still a memory-mapped address.
	// Since the CBFS core always use ROM offset, we need to figure out
	// header->romsize even before media is initialized.
	struct cbfs_header *header = (struct cbfs_header*) CONFIG_CBFS_HEADER_ROM_OFFSET; //&_cbfs_master_header;
	if (CBFS_HEADER_MAGIC != ntohl(header->magic)) {
		printk(BIOS_ERR, "Invalid CBFS master header at %p\n", header);
		printk(BIOS_ERR, "Expected %08lx and got %08lx\n", (unsigned long) CBFS_HEADER_MAGIC, (unsigned long) ntohl(header->magic));
		return -1;
	} else {
		uint32_t romsize = ntohl(header->romsize);
		media->context = (void*)(uintptr_t)romsize;
#if defined(CONFIG_ROM_SIZE)
		if (CONFIG_ROM_SIZE != romsize)
			printk(BIOS_INFO, "Warning: rom size unmatch (%d/%d)\n",
			       CONFIG_ROM_SIZE, romsize);
#endif
	}
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
