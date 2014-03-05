/*
 * common utility functions for cbfstool
 *
 * Copyright (C) 2009 coresystems GmbH
 *                 written by Patrick Georgi <patrick.georgi@coresystems.de>
 * Copyright (C) 2012 Google, Inc.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include "common.h"
#include "cbfs.h"

/* Utilities */
int verbose = 0;

/* Small, OS/libc independent runtime check for endianess */
int is_big_endian(void)
{
	static const uint32_t inttest = 0x12345678;
	uint8_t inttest_lsb = *(uint8_t *)&inttest;
	if (inttest_lsb == 0x12) {
		return 1;
	}
	return 0;
}

/* Buffer and file I/O */

int buffer_create(struct buffer *buffer, size_t size, const char *name) {
	buffer->name = strdup(name);
	buffer->size = size;
	buffer->data = (char *)malloc(buffer->size);
	if (!buffer->data) {
		fprintf(stderr, "buffer_create: Insufficient memory (0x%zx).\n",
			size);
	}
	return (buffer->data == NULL);
}

int buffer_from_file(struct buffer *buffer, const char *filename) {
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		perror(filename);
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	buffer->size = ftell(fp);
	buffer->name = strdup(filename);
	rewind(fp);
	buffer->data = (char *)malloc(buffer->size);
	assert(buffer->data);
	if (fread(buffer->data, 1, buffer->size, fp) != buffer->size) {
		fprintf(stderr, "incomplete read: %s\n", filename);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}

int buffer_write_file(struct buffer *buffer, const char *filename) {
	FILE *fp = fopen(filename, "wb");
	if (!fp) {
		perror(filename);
		return -1;
	}
	assert(buffer && buffer->data);
	if (fwrite(buffer->data, 1, buffer->size, fp) != buffer->size) {
		fprintf(stderr, "incomplete write: %s\n", filename);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}

void buffer_delete(struct buffer *buffer) {
	assert(buffer);
	if (buffer->name) {
		free(buffer->name);
		buffer->name = NULL;
	}
	if (buffer->data) {
		free(buffer->data);
		buffer->data = NULL;
	}
	buffer->size = 0;
}

void cbfs_file_get_header(struct buffer *buf, struct cbfs_file *file)
{
	bgets(buf, &file->magic, sizeof(file->magic));
	file->len = xdr_be.get32(buf);
	file->type = xdr_be.get32(buf);
	file->checksum = xdr_be.get32(buf);
	file->offset = xdr_be.get32(buf);
}

static struct {
	uint32_t arch;
	const char *name;
} arch_names[] = {
	{ CBFS_ARCHITECTURE_ARMV7, "armv7" },
	{ CBFS_ARCHITECTURE_X86, "x86" },
	{ CBFS_ARCHITECTURE_UNKNOWN, "unknown" }
};

uint32_t string_to_arch(const char *arch_string)
{
	int i;
	uint32_t ret = CBFS_ARCHITECTURE_UNKNOWN;

	for (i = 0; i < ARRAY_SIZE(arch_names); i++) {
		if (!strcasecmp(arch_string, arch_names[i].name)) {
			ret = arch_names[i].arch;
			break;
		}
	}

	return ret;
}

static struct filetypes_t {
	uint32_t type;
	const char *name;
} filetypes[] = {
	{CBFS_COMPONENT_STAGE, "stage"},
	{CBFS_COMPONENT_PAYLOAD, "payload"},
	{CBFS_COMPONENT_OPTIONROM, "optionrom"},
	{CBFS_COMPONENT_BOOTSPLASH, "bootsplash"},
	{CBFS_COMPONENT_RAW, "raw"},
	{CBFS_COMPONENT_VSA, "vsa"},
	{CBFS_COMPONENT_MBI, "mbi"},
	{CBFS_COMPONENT_MICROCODE, "microcode"},
	{CBFS_COMPONENT_CMOS_DEFAULT, "cmos default"},
	{CBFS_COMPONENT_CMOS_LAYOUT, "cmos layout"},
	{CBFS_COMPONENT_DELETED, "deleted"},
	{CBFS_COMPONENT_NULL, "null"}
};

void print_supported_filetypes(void)
{
	int i, number = ARRAY_SIZE(filetypes);

	for (i=0; i<number; i++) {
		LOG(" %s%c", filetypes[i].name, (i==(number-1))?'\n':',');
		if ((i%8) == 7)
			LOG("\n");
	}
}

uint64_t intfiletype(const char *name)
{
	size_t i;
	for (i = 0; i < (sizeof(filetypes) / sizeof(struct filetypes_t)); i++)
		if (strcmp(filetypes[i].name, name) == 0)
			return filetypes[i].type;
	return -1;
}
