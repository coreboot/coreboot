/*
 * cbfstool
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
#include <stdlib.h>
#include "cbfstool.h"

int namelen(const char *name)
{
	return ALIGN(strlen(name) + 1, 16);
}

/**
 * Given a name, return the header size for that name. 
 * @param name The name
 * @returns The header size given that name
 */
int headersize(const char *name)
{
	return sizeof(struct cbfs_file) + namelen(name);
}

/**
 * Given a name, set it into the header in a standard way
 * @param file the cbfs file
 * @param name The name
 */
void setname(struct cbfs_file *file, const char *name)
{
	memset(CBFS_NAME(file), 0, namelen(name));
	strcpy((char *)CBFS_NAME(file), name);
}

/**
 * Given a name, size, and type, set them into the header in a standard way. 
 * Special case of size of -1: set the size to all of ROM
 * @param rom The rom
 * @param c The cbfs file
 * @param name The name
 * @param size The size
 * @param type The type
 * @returns Always 0 for now
 */
int rom_set_header(struct rom *rom, struct cbfs_file *c, const char *name, int size, int type)
{
	unsigned int csize;
	csize = headersize(name);

	strcpy(c->magic, COMPONENT_MAGIC);

	/* special case -- if size is -1, means "as much as you can"
	 * it's usually only used in init. 
	 */
	if (size < 0)
		size = rom->fssize - csize;
	c->len = htonl(size);
	c->offset = htonl(csize);
	c->type = htonl(type);

	setname(c, name);
	return 0;
}

int nextfile(struct rom *rom, struct cbfs_file *c, int offset)
{
	return ALIGN(offset + ntohl(c->len),
					ntohl(rom->header->align));
}

/**
 * rom_alloc
 * Given a rom, walk the headers and find the first header of type 
 * CBFS_COMPONENT_NULL that is >= the desired size. 
 * If the CBFS_COMPONENT_NULL is 'align' bytes > size, 
 * create a new header of CBFS_COMPONENT_NULL following the file. 
 * The 'len' structure member of the desired file is initialized, but 
 * nothing else is. 
 * @param rom The rom
 * @param size the size of the file needed
 * @returns pointer to a cbfs_file struct. 
 */
struct cbfs_file * rom_alloc(struct rom *rom, const char *name, unsigned long size, int type)
{
	/* walk the rom and find an empty file with a base > base, and a large enough size */
	unsigned int offset = ntohl(rom->header->offset);
	int ret = -1;
	struct cbfs_file *c = NULL;
	unsigned long nextoffset, truncoffset;
	struct cbfs_file *newfile = NULL;

	while (offset < rom->fssize) {

		c = (struct cbfs_file *)ROM_PTR(rom, offset);

		if (!strcmp(c->magic, COMPONENT_MAGIC)) {
			if (c->type != CBFS_COMPONENT_NULL) {
				offset += ALIGN(ntohl(c->offset) + ntohl(c->len),
					ntohl(rom->header->align));
				continue;
			}
			/* Is this file big enough for our needs? */
			if (ntohl(c->len) >= size){
				ret = offset;
				break;
			}
			offset += ALIGN(ntohl(c->offset) + ntohl(c->len),
					ntohl(rom->header->align));
		} else {
			fprintf(stderr, "Corrupt rom -- found no header at %d\n", offset);
			exit(1);
		}
	}

	if (ret < 0)
		return NULL;

	/* figure out the real end of this file, and hence the size */
	/* compute where the next file is */
	nextoffset = ALIGN(ret + ntohl(c->len) + headersize(name),
				ntohl(rom->header->align));
	/* compute where the end of this new file might be */
	truncoffset = ALIGN(ret + size + headersize(name),
				ntohl(rom->header->align));
	/* If there is more than align bytes difference, create a new empty file */
	/* later, we can add code to merge all empty files. */
	if (nextoffset - truncoffset > ntohl(rom->header->align)) {
		unsigned int csize;
		csize = headersize("");
		newfile = (struct cbfs_file *)ROM_PTR(rom, truncoffset);
		rom_set_header(rom, newfile, "", 
			nextoffset - truncoffset - csize, CBFS_COMPONENT_NULL);
	} else truncoffset = nextoffset;

	c->len = htonl(size);

	strcpy(c->magic, COMPONENT_MAGIC);

	c->offset = htonl(headersize(name));

	c->type = htonl(type);

	setname(c, name);

	return ((struct cbfs_file *)ROM_PTR(rom, ret));
}

struct cbfs_file *rom_find(struct rom *rom, int offset)
{
	while (offset < rom->fssize) {
		struct cbfs_file *c =
		    (struct cbfs_file *)ROM_PTR(rom, offset);

		if (!strcmp(c->magic, COMPONENT_MAGIC))
			return c;

		offset += ntohl(rom->header->align);
	}

	return NULL;
}

struct cbfs_file *rom_find_first(struct rom *rom)
{
	return rom_find(rom, ntohl(rom->header->offset));
}

struct cbfs_file *rom_find_next(struct rom *rom, struct cbfs_file *prev)
{
	unsigned int offset = ROM_OFFSET(rom, prev);

	return rom_find(rom, offset +
			ALIGN(ntohl(prev->offset) + ntohl(prev->len),
			      ntohl(rom->header->align)));
}

struct cbfs_file *rom_find_by_name(struct rom *rom, const char *name)
{
	struct cbfs_file *c = rom_find_first(rom);

	while (c) {
		if (!strcmp((char *)CBFS_NAME(c), name))
			return c;

		c = rom_find_next(rom, c);
	}

	return NULL;
}

int rom_used_space(struct rom *rom)
{
	struct cbfs_file *c = rom_find_first(rom);
	unsigned int ret = 0;

	while (c) {
		int type;
		type = ntohl(c->type);
		if ((c->type == CBFS_COMPONENT_DELETED) ||
			(c->type == CBFS_COMPONENT_NULL))
			continue;
		ret += ROM_OFFSET(rom, c) + ntohl(c->offset) + ntohl(c->len);
		c = rom_find_next(rom, c);
	}

	return ret;
}

/** 
 * delete an item. This is a flash-friendly version -- it just blows the 
 * type to 0. Nothing else is changed. 
 * N.B. We no longer shuffle contents of ROM. That will come later. 
 * @param rom The rom
 * @param name Name of file to remove. 
 * @return -1 on error, 0 if a file was set to deleted. 
 */
int rom_remove(struct rom *rom, const char *name)
{
	struct cbfs_file *c = rom_find_by_name(rom, name);

	if (c == NULL) {
		ERROR("Component %s does not exist\n", name);
		return -1;
	}

	c->type = CBFS_COMPONENT_DELETED; 

	return 0;
}

int rom_extract(struct rom *rom, const char *name, void** buf, int *size )
{
	struct cbfs_file *c = rom_find_by_name(rom, name);

	if (c == NULL) {
		ERROR("Component %s does not exist\n", name);
		return -1;
	}

	*size = ntohl(c->len);
	*buf = ((unsigned char *)c) + headersize(name);
	return 0;
}

/**
 * Add a new file named 'name', of type 'type', size 'size'. Initialize that file
 * with the contents of 'buffer'. 
 * @param rom The rom
 * @param name file name
 * @param buffer file data
 * @param size Amount of data
 * @param type File type
 * @returns -1 on failure, 0 on success
 */
int rom_add(struct rom *rom, const char *name, void *buffer, int size, int type)
{
	struct cbfs_file *c;

	if (rom_find_by_name(rom, name)) {
		ERROR("Component %s already exists in this rom\n", name);
		return -1;
	}

	c = rom_alloc(rom, name, size, type);

	if (c == NULL) {
		ERROR("There is no more room in this ROM\n");
		return -1;
	}

	memcpy(((unsigned char *)c) + ntohl(c->offset), buffer, size);
	return 0;
}

