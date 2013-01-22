/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
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

#ifndef __INCLUDE_ARCH_CBFS__
#define __INCLUDE_ARCH_CBFS__

#include <string.h>
#include <types.h>
#include <cbfs_core.h>
#include <arch/byteorder.h>

#if 0
static int cbfs_check_magic(struct cbfs_file *file)
{
	return strcmp(file->magic, CBFS_FILE_MAGIC) ? 0 : 1;
}

static unsigned long loadstage(const char* target)
{
	unsigned long offset, align;
	struct cbfs_header *header = (struct cbfs_header *)(CONFIG_BOOTBLOCK_BASE + 0x40);

	offset = ntohl(header->offset);
	align = ntohl(header->align);
	printk(BIOS_INFO, "cbfs header (0x%p)\n", header);
	printk(BIOS_INFO, "\tmagic: 0x%08x\n", ntohl(header->magic));
	printk(BIOS_INFO, "\tversion: 0x%08x\n", ntohl(header->version));
	printk(BIOS_INFO, "\tromsize: 0x%08x\n", ntohl(header->romsize));
	printk(BIOS_INFO, "\tbootblocksize: 0x%08x\n", ntohl(header->bootblocksize));
	printk(BIOS_INFO, "\talign: 0x%08x\n", ntohl(header->align));
	printk(BIOS_INFO, "\toffset: 0x%08x\n", ntohl(header->offset));
	while(1) {
		struct cbfs_file *file;
		struct cbfs_stage *stage;
		/* FIXME: SPI image hack */
		file = (struct cbfs_file *)(offset + CONFIG_SPI_IMAGE_HACK);
		if (!cbfs_check_magic(file)) {
			printk(BIOS_INFO, "magic is wrong, file: %p\n", file);
			return 0;
		}
		if (!strcmp(CBFS_NAME(file), target)) {
			uint32_t  load, entry;
			printk(BIOS_INFO, "CBFS name matched, offset: %p\n", file);
			printk(BIOS_INFO, "\tmagic: %02x%02x%02x%02x%02x%02x%02x%02x\n",
				file->magic[0], file->magic[1], file->magic[2], file->magic[3],
				file->magic[4], file->magic[5], file->magic[6], file->magic[7]);
			printk(BIOS_INFO, "\tlen: 0x%08x\n", ntohl(file->len));
			printk(BIOS_INFO, "\ttype: 0x%08x\n", ntohl(file->type));
			printk(BIOS_INFO, "\tchecksum: 0x%08x\n", ntohl(file->checksum));
			printk(BIOS_INFO, "\toffset: 0x%08x\n", ntohl(file->offset));
			/* exploit the fact that this is all word-aligned. */
			stage = CBFS_SUBHEADER(file);
			load = stage->load;
			entry = stage->entry;
			int i;
			u32 *to = (void *)load;
			u32 *from = (void *)((u8 *)stage+sizeof(*stage));
			/* we could do memmove/memset here. But the math gets messy. 
			 * far easier just to do what we want.
			 */
			 printk(BIOS_INFO, "entry: 0x%08x, load: 0x%08x, "
			 	"len: 0x%08x, memlen: 0x%08x\n", entry,
				 load, stage->len, stage->memlen);
			for(i = 0; i < stage->len; i += 4)
				*to++ = *from++;
			for(; i < stage->memlen; i += 4)
				*to++ = 0;
			return entry;
		}
		int flen = ntohl(file->len);
		int foffset = ntohl(file->offset);
		unsigned long oldoffset = offset;
		offset = ALIGN(offset + foffset + flen, align);
		printk(BIOS_INFO, "offset: 0x%08lx\n", offset);
		if (offset <= oldoffset)
			return 0;
		if (offset > CONFIG_ROMSTAGE_SIZE)
			return 0;
	}
}
#endif // 0
#endif // __INCLUDE_ARCH_CBFS__
