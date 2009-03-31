/*
 * romtool
 *
 * Copyright (C) 2008 Jordan Crouse <jordan@cosmicpenguin.net>
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

#include <string.h>
#include "romtool.h"

struct romfs_file *rom_find(struct rom *rom, unsigned int offset)
{
	while (offset < rom->fssize) {
		struct romfs_file *c =
		    (struct romfs_file *)ROM_PTR(rom, offset);

		if (!strcmp(c->magic, COMPONENT_MAGIC))
			return c;

		offset += ntohl(rom->header->align);
	}

	return NULL;
}

struct romfs_file *rom_find_first(struct rom *rom)
{
	return rom_find(rom, ntohl(rom->header->offset));
}

struct romfs_file *rom_find_next(struct rom *rom, struct romfs_file *prev)
{
	unsigned int offset = ROM_OFFSET(rom, prev);

	return rom_find(rom, offset +
			ALIGN(ntohl(prev->offset) + ntohl(prev->len),
			      ntohl(rom->header->align)));
}

struct romfs_file *rom_find_empty(struct rom *rom)
{
	unsigned int offset = ntohl(rom->header->offset);
	unsigned int ret = ntohl(rom->header->offset);

	while (offset < rom->fssize) {

		struct romfs_file *c =
		    (struct romfs_file *)ROM_PTR(rom, offset);

		if (!strcmp(c->magic, COMPONENT_MAGIC)) {
			offset += ALIGN(ntohl(c->offset) + ntohl(c->len),
					ntohl(rom->header->align));

			ret = offset;
		} else
			offset += ntohl(rom->header->align);
	}

	return (ret < rom->fssize) ?
	    (struct romfs_file *)ROM_PTR(rom, ret) : NULL;
}

struct romfs_file *rom_find_by_name(struct rom *rom, const char *name)
{
	struct romfs_file *c = rom_find_first(rom);

	while (c) {
		if (!strcmp((char *)ROMFS_NAME(c), name))
			return c;

		c = rom_find_next(rom, c);
	}

	return NULL;
}

unsigned int rom_used_space(struct rom *rom)
{
	struct romfs_file *c = rom_find_first(rom);
	unsigned int ret = 0;

	while (c) {
		ret = ROM_OFFSET(rom, c) + ntohl(c->offset) + ntohl(c->len);
		c = rom_find_next(rom, c);
	}

	return ret;
}

int rom_remove(struct rom *rom, const char *name)
{
	struct romfs_file *c = rom_find_by_name(rom, name);
	struct romfs_file *n;
	int clear;

	if (c == NULL) {
		ERROR("Component %s does not exist\n", name);
		return -1;
	}

	/* Get the next component - and copy it into the current
	   space */

	n = rom_find_next(rom, c);

	memcpy(c, n, rom->fssize - ROM_OFFSET(rom, n));

	clear = ROM_OFFSET(rom, n) - ROM_OFFSET(rom, c);

	/* Zero the new space */
	memset(ROM_PTR(rom, rom->fssize - clear), 0, clear);
	return 0;
}

int rom_add(struct rom *rom, const char *name, void *buffer, int size, int type)
{
	struct romfs_file *c = rom_find_empty(rom);
	unsigned int offset;
	unsigned int csize;

	if (rom_find_by_name(rom, name)) {
		ERROR("Component %s already exists in this rom\n", name);
		return -1;
	}

	if (c == NULL) {
		ERROR("There is no more room in this ROM\n");
		return -1;
	}

	csize = sizeof(struct romfs_file) + ALIGN(strlen(name), 16) + size;

	offset = ROM_OFFSET(rom, c);

	if (offset + csize >= rom->fssize) {
		ERROR("There is not enough room in this ROM for this\n");
		ERROR("component. I need %d bytes, only have %d bytes avail\n",
		      csize, rom->fssize - offset);

		return -1;
	}

	strcpy(c->magic, COMPONENT_MAGIC);

	csize = sizeof(struct romfs_file) + ALIGN(strlen(name) + 1, 16);

	c->len = htonl(size);
	c->offset = htonl(csize);
	c->type = htonl(type);

	memset(ROMFS_NAME(c), 0, ALIGN(strlen(name) + 1, 16));
	strcpy((char *)ROMFS_NAME(c), name);

	memcpy(((unsigned char *)c) + csize, buffer, size);
	return 0;
}
