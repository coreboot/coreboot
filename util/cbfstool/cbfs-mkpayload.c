/*
 * cbfs-mkpayload
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

#include "common.h"
#include "elf.h"

int parse_elf_to_payload(const struct buffer *input,
			 struct buffer *output, comp_algo algo) {
	Elf32_Phdr *phdr;
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)input->data;
	Elf32_Shdr *shdr;
	char *header;
	char *strtab;
	int headers;
	int segments = 1;
	int isize = 0, osize = 0;
	int doffset = 0;
	struct cbfs_payload_segment *segs;
	int i;

	if(!is_elf_object(input->data)){
		ERROR("The payload file is not in ELF format!\n");
		return -1;
	}

	comp_func_ptr compress = compression_function(algo);
	if (!compress)
		return -1;

	DEBUG("start: parse_elf_to_payload\n");
	headers = ehdr->e_phnum;
	header = (char *)ehdr;

	phdr = (Elf32_Phdr *) & (header[ehdr->e_phoff]);
	shdr = (Elf32_Shdr *) & (header[ehdr->e_shoff]);

	strtab = &header[shdr[ehdr->e_shstrndx].sh_offset];

	/* Count the number of headers - look for the .notes.pinfo
	 * section */

	for (i = 0; i < ehdr->e_shnum; i++) {
		char *name;

		if (i == ehdr->e_shstrndx)
			continue;

		if (shdr[i].sh_size == 0)
			continue;

		name = (char *)(strtab + shdr[i].sh_name);

		if (!strcmp(name, ".note.pinfo")) {
			segments++;
			isize += (unsigned int)shdr[i].sh_size;
		}
	}

	/* Now, regular headers - we only care about PT_LOAD headers,
	 * because thats what we're actually going to load
	 */

	for (i = 0; i < headers; i++) {
		if (phdr[i].p_type != PT_LOAD)
			continue;

		/* Empty segments are never interesting */
		if (phdr[i].p_memsz == 0)
			continue;

		isize += phdr[i].p_filesz;

		segments++;
	}

	/* Allocate a block of memory to store the data in */
	if (buffer_create(output, (segments * sizeof(*segs)) + isize,
			  input->name) != 0)
		return -1;
	memset(output->data, 0, output->size);

	doffset = (segments * sizeof(struct cbfs_payload_segment));

	segs = (struct cbfs_payload_segment *)output->data;
	segments = 0;

	for (i = 0; i < ehdr->e_shnum; i++) {
		char *name;

		if (i == ehdr->e_shstrndx)
			continue;

		if (shdr[i].sh_size == 0)
			continue;

		name = (char *)(strtab + shdr[i].sh_name);

		if (!strcmp(name, ".note.pinfo")) {
			segs[segments].type = PAYLOAD_SEGMENT_PARAMS;
			segs[segments].load_addr = 0;
			segs[segments].len = (unsigned int)shdr[i].sh_size;
			segs[segments].offset = doffset;

			memcpy((unsigned long *)(output->data + doffset),
			       &header[shdr[i].sh_offset], shdr[i].sh_size);

			doffset += segs[segments].len;
			osize += segs[segments].len;

			segments++;
		}
	}

	for (i = 0; i < headers; i++) {
		if (phdr[i].p_type != PT_LOAD)
			continue;

		if (phdr[i].p_memsz == 0)
			continue;

		if (phdr[i].p_filesz == 0) {
			segs[segments].type = PAYLOAD_SEGMENT_BSS;
			segs[segments].load_addr =
			    htonll(phdr[i].p_paddr);
			segs[segments].mem_len =
			    htonl(phdr[i].p_memsz);
			segs[segments].offset = htonl(doffset);

			segments++;
			continue;
		}

		if (phdr[i].p_flags & PF_X)
			segs[segments].type = PAYLOAD_SEGMENT_CODE;
		else
			segs[segments].type = PAYLOAD_SEGMENT_DATA;
		segs[segments].load_addr = htonll(phdr[i].p_paddr);
		segs[segments].mem_len = htonl(phdr[i].p_memsz);
		segs[segments].compression = htonl(algo);
		segs[segments].offset = htonl(doffset);

		int len;
		compress((char *)&header[phdr[i].p_offset],
			 phdr[i].p_filesz, output->data + doffset, &len);
		segs[segments].len = htonl(len);

		/* If the compressed section is larger, then use the
		   original stuff */

		if ((unsigned int)len > phdr[i].p_filesz) {
			segs[segments].compression = 0;
			segs[segments].len = htonl(phdr[i].p_filesz);

			memcpy(output->data + doffset,
			       &header[phdr[i].p_offset], phdr[i].p_filesz);
		}

		doffset += ntohl(segs[segments].len);
		osize += ntohl(segs[segments].len);

		segments++;
	}

	segs[segments].type = PAYLOAD_SEGMENT_ENTRY;
	segs[segments++].load_addr = htonll(ehdr->e_entry);

	output->size = (segments * sizeof(struct cbfs_payload_segment)) + osize;
	return 0;
}

int parse_flat_binary_to_payload(const struct buffer *input,
				 struct buffer *output,
				 uint32_t loadaddress,
				 uint32_t entrypoint,
				 comp_algo algo)
{
	comp_func_ptr compress;
	struct cbfs_payload_segment *segs;
	int doffset, len = 0;

	compress = compression_function(algo);
	if (!compress)
		return -1;

	DEBUG("start: parse_flat_binary_to_payload\n");
	if (buffer_create(output, (2 * sizeof(*segs) + input->size),
			  input->name) != 0)
		return -1;
	memset(output->data, 0, output->size);

	segs = (struct cbfs_payload_segment *)output->data;
	doffset = (2 * sizeof(*segs));

	/* Prepare code segment */
	segs[0].type = PAYLOAD_SEGMENT_CODE;
	segs[0].load_addr = htonll(loadaddress);
	segs[0].mem_len = htonl(input->size);
	segs[0].offset = htonl(doffset);

	/* TODO what if  compressed file size is bigger than original? */
	compress(input->data, input->size, output->data + doffset, &len);
	segs[0].compression = htonl(algo);
	segs[0].len = htonl(len);

	if ((unsigned int)len >= input->size) {
		segs[0].compression = 0;
		segs[0].len = htonl(input->size);
		memcpy(output->data + doffset, input->data, input->size);
	}

	/* prepare entry point segment */
	segs[1].type = PAYLOAD_SEGMENT_ENTRY;
	segs[1].load_addr = htonll(entrypoint);
	output->size = doffset + ntohl(segs[0].len);

	return 0;
}
