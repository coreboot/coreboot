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
#include <arch/cbfs.h>


static int cbfs_check_magic(struct cbfs_file *file)
{
	return strcmp(file->magic, CBFS_FILE_MAGIC) ? 0 : 1;
}

/* start and stop the stream each time. It's the simplest thing to do */
static unsigned long loadstage(const char* target)
{
	unsigned long offset, align;
	struct stream stream;
	struct cbfs_header *header = (struct cbfs_header *)(CONFIG_BOOTBLOCK_BASE + 0x40);
	stream_start(&stream, 0x12d30000, 4*1048576);

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
		struct cbfs_file f, *file = &f;
		struct cbfs_stage s, *stage = &s;
		int amount;
		/* FIXME: SPI image hack */
		amount = stream_read(&stream, (void *)file, sizeof(file), offset);
		if (amount < sizeof(*file)){
			printk(BIOS_INFO, "Stream read of file header only returned %d bytes, wanted %d\n",
			       amount, sizeof(*file));
			goto fail;
		}

		if (!cbfs_check_magic(file)) {
			printk(BIOS_INFO, "magic is wrong, file: %p\n", file);
			goto fail;
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
			offset += file->offset;
			amount = stream_read(&stream, (void *)stage, 
					     sizeof(*stage), offset);
			if (amount < sizeof(*stage)){
				printk(BIOS_ERR, "Read stage: got %d, wanted %d\n", 
				       amount, sizeof(stage));
				goto fail;
			}
			load = stage->load;
			entry = stage->entry;
			int i;
			offset += sizeof(stage);
			amount = stream_read(&stream, (void *)load, stage->len, offset);
			if (amount < stage->len){
				printk(BIOS_ERR, "Stage read: got %d, wanted %d\n", 
				       amount, stage->len);
				       
				goto fail;
			}
			u32 *zero = (u32 *)(load + stage->len);
			for(i = stage->len; i < stage->memlen; i += 4)
				*zero++ = 0;
			stream_fini(&stream);
			return entry;
		}
		int flen = ntohl(file->len);
		int foffset = ntohl(file->offset);
		unsigned long oldoffset = offset;
		offset = ALIGN(offset + foffset + flen, align);
		printk(BIOS_INFO, "offset: 0x%08lx\n", offset);
		if (offset <= oldoffset)
			goto fail;
		if (offset > CONFIG_ROMSTAGE_SIZE)
			goto fail;
	}

 fail:
	stream_fini(&stream);
	return 0;
}
#endif
