/*
 * cbfs-mkstage
 *
 * Copyright (C) 2008 Jordan Crouse <jordan@cosmicpenguin.net>
 *               2009 coresystems GmbH
 *                 written by Patrick Georgi <patrick.georgi@coresystems.de>
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
#include <unistd.h>
#include "elf.h"
#include <fcntl.h>
#include <getopt.h>
#include <sys/stat.h>

#include "common.h"
#include "cbfs.h"

unsigned int idemp(unsigned int x)
{
	return x;
}

unsigned int swap32(unsigned int x)
{
	return ((x >> 24) | ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000) |
		(x << 24));
}

unsigned int (*elf32_to_native) (unsigned int) = idemp;

/* returns size of result, or -1 if error */
int parse_elf_to_stage(unsigned char *input, unsigned char **output,
		       comp_algo algo, uint32_t * location)
{
	Elf32_Phdr *phdr;
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *) input;
	char *header, *buffer;
	unsigned char *out;

	int headers;
	int i;
	struct cbfs_stage *stage;
	unsigned int data_start, data_end, mem_end;

	int elf_bigendian = 0;
	int host_bigendian = 0;

	comp_func_ptr compress = compression_function(algo);
	if (!compress)
		return -1;

	if (!iself(input)) {
		fprintf(stderr, "E:  The incoming file is not an ELF\n");
		return -1;
	}

	if (ehdr->e_ident[EI_DATA] == ELFDATA2MSB) {
		elf_bigendian = 1;
	}
	char test[4] = "1234";
	uint32_t inttest = *(uint32_t *) test;
	if (inttest == 0x31323334) {
		host_bigendian = 1;
	}
	if (elf_bigendian != host_bigendian) {
		elf32_to_native = swap32;
	}

	headers = ehdr->e_phnum;
	header = (char *)ehdr;

	phdr = (Elf32_Phdr *) & header[elf32_to_native(ehdr->e_phoff)];

	/* Now, regular headers - we only care about PT_LOAD headers,
	 * because thats what we're actually going to load
	 */

	data_start = 0xFFFFFFFF;
	data_end = 0;
	mem_end = 0;

	for (i = 0; i < headers; i++) {
		unsigned int start, mend, rend;

		if (elf32_to_native(phdr[i].p_type) != PT_LOAD)
			continue;

		/* Empty segments are never interesting */
		if (elf32_to_native(phdr[i].p_memsz) == 0)
			continue;

		/* BSS */

		start = elf32_to_native(phdr[i].p_paddr);

		mend = start + elf32_to_native(phdr[i].p_memsz);
		rend = start + elf32_to_native(phdr[i].p_filesz);

		if (start < data_start)
			data_start = start;

		if (rend > data_end)
			data_end = rend;

		if (mend > mem_end)
			mem_end = mend;
	}

	if (data_start < *location) {
		data_start = *location;
	}

	/* allocate an intermediate buffer for the data */
	buffer = calloc(data_end - data_start, 1);

	if (buffer == NULL) {
		fprintf(stderr, "E: Unable to allocate memory: %m\n");
		return -1;
	}

	/* Copy the file data into the buffer */

	for (i = 0; i < headers; i++) {
		unsigned int l_start, l_offset = 0;

		if (elf32_to_native(phdr[i].p_type) != PT_LOAD)
			continue;

		if (elf32_to_native(phdr[i].p_memsz) == 0)
			continue;

		l_start = elf32_to_native(phdr[i].p_paddr);
		if (l_start < *location) {
			l_offset = *location - l_start;
			l_start = *location;
		}

		memcpy(buffer + (l_start - data_start),
		       &header[elf32_to_native(phdr[i].p_offset)+l_offset],
		       elf32_to_native(phdr[i].p_filesz)-l_offset);
	}

	/* Now make the output buffer */
	out = calloc(sizeof(struct cbfs_stage) + data_end - data_start, 1);

	if (out == NULL) {
		fprintf(stderr, "E: Unable to allocate memory: %m\n");
		return -1;
	}

	stage = (struct cbfs_stage *)out;

	stage->load = data_start;
	stage->memlen = mem_end - data_start;
	stage->compression = algo;
	stage->entry = ehdr->e_entry;

	compress(buffer, data_end - data_start,
		 (char *)(out + sizeof(struct cbfs_stage)), (int *)&stage->len);

	*output = out;

	if (*location)
		*location -= sizeof(struct cbfs_stage);
	return sizeof(struct cbfs_stage) + stage->len;
}
