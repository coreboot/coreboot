/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2006-2007 coresystems GmbH
 * Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <arch/types.h>
#include <string.h>
#include <lar.h>
#include <console/console.h>

#ifndef CONFIG_BIG_ENDIAN
#define ntohl(x) ( ((x&0xff)<<24) | ((x&0xff00)<<8) | \
		((x&0xff0000) >> 8) | ((x&0xff000000) >> 24) )
#else
#define ntohl(x) (x)
#endif

int find_file(struct mem_file *archive, char *filename, struct mem_file *result)
{
	char *walk, *fullname;
	struct lar_header *header;

	printk(BIOS_INFO, "LAR: Attempting to open '%s'.\n", filename);
	printk(BIOS_SPEW, "LAR: Start 0x%x len 0x%x\n", archive->start,
	       archive->len);

	for (walk = archive->start;
	     walk < (char *)archive->start + archive->len - 1; walk += 16) {
		if (strcmp(walk, MAGIC) != 0)
			continue;

		header = (struct lar_header *)walk;
		fullname = walk + sizeof(struct lar_header);

		printk(BIOS_SPEW, "LAR: current filename is %s\n", fullname);
		// FIXME: check checksum

		if (strcmp(fullname, filename) == 0) {
			result->start = walk + ntohl(header->offset);
			result->len = ntohl(header->len);
			return 0;
		}
		// skip file
		walk += (ntohl(header->len) + ntohl(header->offset) -
			1) & 0xfffffff0;
	}
	return 1;
}

int copy_file(struct mem_file *archive, char *filename, void *where)
{
	int ret;
	struct mem_file result;

	ret = find_file(archive, filename, &result);
	if (ret) {
		printk(BIOS_INFO, "LAR: copy_file: No such file '%s'\n",
		       filename);
		return 1;
	}

	memcpy(where, result.start, result.len);

	return 0;
}

int run_file(struct mem_file *archive, char *filename, void *where)
{
	int (*v) (void);

	if (copy_file(archive, filename, where)) {
		printk(BIOS_INFO, "LAR: Run file %s failed: No such file.\n",
		       filename);
		return 1;
	}

	v = where;
	return v();
}
