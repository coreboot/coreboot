/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Jordan Crouse <jordan@cosmicpenguin.net>
 * Copyright (C) 2011 secunet Security Networks AG
 *     (Written by Patrick Georgi <patrick.georgi@secunet.com>)
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

#ifdef __MINGW32__
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#ifndef __MINGW32__
#include <sys/mman.h>
#endif
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include "cbfs.h"
#include "common.h"

#define ALIGN(x,a)              __ALIGN_MASK(x,(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))

static void *cbfs_mapped;
static void *cbfs_offset;
static void* virt_to_phys(u32 virt)
{
	return cbfs_offset + virt;
}

#ifdef DEBUG
#define debug(x...) printf(x)
#else
#define debug(x...) while(0) {}
#endif

static int cbfs_check_magic(struct cbfs_file *file)
{
	return !strcmp(file->magic, CBFS_FILE_MAGIC) ? 1 : 0;
}

static struct cbfs_header *cbfs_master_header(void)
{
	struct cbfs_header *header;

	void *ptr = virt_to_phys(*((u32*)virt_to_phys(CBFS_HEADPTR_ADDR)));
	debug("Check CBFS header at %p\n", ptr);
	header = (struct cbfs_header *) ptr;

	debug("magic is %08x\n", ntohl(header->magic));
	if (ntohl(header->magic) != CBFS_HEADER_MAGIC) {
		printf("ERROR: No valid CBFS header found!\n");
		return NULL;
	}

	debug("Found CBFS header at %p\n", ptr);
	return header;
}

struct cbfs_file *cbfs_find(const char *name)
{
	struct cbfs_header *header = cbfs_master_header();
	void *offset;

	if (header == NULL)
		return NULL;
	offset = virt_to_phys(0 - ntohl(header->romsize) + ntohl(header->offset));

	int align= ntohl(header->align);

	while(1) {
		struct cbfs_file *file = (struct cbfs_file *) offset;
		if (!cbfs_check_magic(file)) return NULL;
		debug("Check %s\n", CBFS_NAME(file));
		if (!strcmp(CBFS_NAME(file), name))
			return file;

		int flen = ntohl(file->len);
		int foffset = ntohl(file->offset);
		debug("CBFS: follow chain: %p + %x + %x + align -> ", offset, foffset, flen);

		void *oldoffset = offset;
		offset = (void*)ALIGN((uintptr_t)(offset + foffset + flen), align);
		debug("%p\n", (void *)offset);
		if (offset <= oldoffset) return NULL;

		if (offset < virt_to_phys(0xFFFFFFFF - ntohl(header->romsize)))
			return NULL;
	}
}

void *cbfs_find_file(const char *name, unsigned int type, unsigned int *len)
{
	struct cbfs_file *file = cbfs_find(name);

	if (file == NULL) {
		printf("CBFS:  Could not find file %s\n",
		       name);
		return NULL;
	}

	if (ntohl(file->type) != type) {
		printf("CBFS:  File %s is of type %x instead of"
		       "type %x\n", name, file->type, type);

		return NULL;
	}
	if (len != NULL) *len = file->len;

	return (void *) CBFS_SUBHEADER(file);
}

void open_cbfs(const char *filename)
{
	struct stat cbfs_stat;
	int cbfs_fd;

	cbfs_fd = open(filename, O_RDWR);
	if (cbfs_fd == -1) {
		printf("Couldn't open '%s'\n", filename);
		exit(-1);
	}
	if (fstat(cbfs_fd, &cbfs_stat) == -1) {
		printf("Couldn't stat '%s'\n", filename);
		exit(-1);
	}
	cbfs_mapped = mmap(NULL, cbfs_stat.st_size, PROT_READ | PROT_WRITE,
			MAP_SHARED, cbfs_fd, 0);
	if (cbfs_mapped == MAP_FAILED) {
		printf("Couldn't map '%s'\n", filename);
		exit(-1);
	}
	cbfs_offset = cbfs_mapped-(0xffffffff-cbfs_stat.st_size+1);
}

