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

static unsigned long findstage(const char* target)
{
	unsigned long offset, align;
	/* FIXME: magic offsets */
	struct cbfs_header *header = (struct cbfs_header *)(0x02062040);
	// if (ntohl(header->magic) != CBFS_HEADER_MAGIC)
	// 	printk(BIOS_ERR, "ERROR: No valid CBFS header found!\n");

	offset = ntohl(header->offset);
	align = ntohl(header->align);
	printk(BIOS_INFO, "cbfs header (%p)\n", header);
	printk(BIOS_INFO, "\tmagic: 0x%08x\n", ntohl(header->magic));
	printk(BIOS_INFO, "\tversion: 0x%08x\n", ntohl(header->version));
	printk(BIOS_INFO, "\tromsize: 0x%08x\n", ntohl(header->romsize));
	printk(BIOS_INFO, "\tbootblocksize: 0x%08x\n", ntohl(header->bootblocksize));
	printk(BIOS_INFO, "\talign: 0x%08x\n", ntohl(header->align));
	printk(BIOS_INFO, "\toffset: 0x%08x\n", ntohl(header->offset));
	while(1) {
		struct cbfs_file *file;
		file = (struct cbfs_file *)(offset + 0x02060000);
		if (!cbfs_check_magic(file)) {
			printk(BIOS_INFO, "magic is wrong, file: %p\n", file);
			return 0;
		}
		if (!strcmp(CBFS_NAME(file), target)) {
			printk(BIOS_INFO, "CBFS name matched, offset: %p\n", file);
			printk(BIOS_INFO, "\tmagic: %02x%02x%02x%02x%02x%02x%02x%02x\n",
				file->magic[0], file->magic[1], file->magic[2], file->magic[3],
				file->magic[4], file->magic[5], file->magic[6], file->magic[7]);
			printk(BIOS_INFO, "\tlen: 0x%08x\n", ntohl(file->len));
			printk(BIOS_INFO, "\ttype: 0x%08x\n", ntohl(file->type));
			printk(BIOS_INFO, "\tchecksum: 0x%08x\n", ntohl(file->checksum));
			printk(BIOS_INFO, "\toffset: 0x%08x\n", ntohl(file->offset));
			return (unsigned long)CBFS_SUBHEADER(file);
		}
		int flen = ntohl(file->len);
		int foffset = ntohl(file->offset);
		unsigned long oldoffset = offset;
		offset = ALIGN(offset + foffset + flen, align);
		printk(BIOS_INFO, "offset: 0x%08lx\n", offset);
		if (offset <= oldoffset)
			return 0;
		if (offset > (128 * 1024))
			return 0;
	}
}

static inline void call(unsigned long addr)
{
	__attribute__((noreturn)) void (*doit)(void) = (void *)addr;
	printk(BIOS_INFO, "addr: %08lx, doit: %p\n", addr, doit);
#if 0
	/* FIXME: dumping SRAM content for sanity checking */
//	int i;
	for (i = 0; i < 0x10000; i++) {
		if (i % 16 == 0)
			printk(BIOS_INFO, "\n0x%08lx: ", addr + i);
		else
			printk(BIOS_INFO, " ");
		printk(BIOS_INFO, "%02x", *(uint8_t *)(addr + i));
	}
#endif
	/* FIXME: do we need to change to/from arm/thumb? */
	doit();
}
#endif
