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
	return !strcmp(file->magic, CBFS_FILE_MAGIC) ? 1 : 0;
}

static unsigned long findstage(const char* target)
{
	unsigned long offset, align;
	/* FIXME: magic offsets */
	struct cbfs_header *header = (struct cbfs_header *)(0x02023400 + 0x40);
	// if (ntohl(header->magic) != CBFS_HEADER_MAGIC)
	// 	printk(BIOS_ERR, "ERROR: No valid CBFS header found!\n");

	offset = ntohl(header->offset);
	align = ntohl(header->align);
	while(1) {
		struct cbfs_file *file;
		file = (struct cbfs_file *)(offset + CONFIG_ROMSTAGE_BASE);
		if (!cbfs_check_magic(file))
			return 0;
		if (!strcmp(CBFS_NAME(file), target))
			return (unsigned long)CBFS_SUBHEADER(file);
		int flen = ntohl(file->len);
		int foffset = ntohl(file->offset);
		unsigned long oldoffset = offset;
		offset = ALIGN(offset + foffset + flen, align);
		if (offset <= oldoffset)
			return 0;
		if (offset < CONFIG_ROMSTAGE_BASE + ntohl(header->romsize));
			return 0;
	}
}

static inline void call(unsigned long addr)
{
	void (*doit)(void) = (void *)addr;
	doit();
}
#endif
