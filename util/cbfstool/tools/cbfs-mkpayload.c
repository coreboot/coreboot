/*
 * cbfs-mkpayload
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "elf.h"
#include <fcntl.h>
#include <getopt.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include "common.h"
#include "../cbfs.h"

int parse_elf(unsigned char *input, unsigned char **output, int algo,
	      void (*compress) (char *, int, char *, int *))
{
	Elf32_Phdr *phdr;
	Elf32_Ehdr *ehdr;
	Elf32_Shdr *shdr;
	char *header;
	char *strtab;
	unsigned char *sptr;
	int headers;
	int segments = 1;
	int isize = 0, osize = 0;
	int doffset = 0;
	struct cbfs_payload_segment *segs;
	int i;

	ehdr = (Elf32_Ehdr *) input;
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

		if (!strcmp(name, ".note.pinfo"))
			segments++;
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

	sptr =
	    calloc((segments * sizeof(struct cbfs_payload_segment)) + isize,
		   1);
	doffset = (segments * sizeof(struct cbfs_payload_segment));

	if (sptr == NULL)
		goto err;

	segs = (struct cbfs_payload_segment *)sptr;
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

			memcpy((unsigned long *)(sptr + doffset),
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
			    (unsigned long long)htonl(phdr[i].p_paddr);
			segs[segments].mem_len = (unsigned int)htonl(phdr[i].p_memsz);
			segs[segments].offset = htonl(doffset);

			segments++;
			continue;
		}

		segs[segments].type = PAYLOAD_SEGMENT_DATA;
		segs[segments].load_addr = (unsigned int)htonl(phdr[i].p_paddr);
		segs[segments].mem_len = (unsigned int)htonl(phdr[i].p_memsz);
		segs[segments].compression = htonl(algo);
		segs[segments].offset = htonl(doffset);

		int len;
		compress((char *)&header[phdr[i].p_offset],
			 phdr[i].p_filesz,
			 (char *)(sptr + doffset), &len);
		segs[segments].len = htonl(len);

		/* If the compressed section is larger, then use the
		   original stuff */

		if ((unsigned int)len > phdr[i].p_filesz) {
			segs[segments].compression = 0;
			segs[segments].len = htonl(phdr[i].p_filesz);

			memcpy((char *)(sptr + doffset),
			       &header[phdr[i].p_offset], phdr[i].p_filesz);
		}

		doffset += ntohl(segs[segments].len);
		osize += ntohl(segs[segments].len);

		segments++;
	}

	segs[segments].type = PAYLOAD_SEGMENT_ENTRY;
	segs[segments++].load_addr = (unsigned long long)htonl(ehdr->e_entry);

	*output = sptr;

	return (segments * sizeof(struct cbfs_payload_segment)) + osize;

err:
	return -1;
}

int main(int argc, char **argv)
{
	void (*compress) (char *, int, char *, int *);
	int algo = CBFS_COMPRESS_NONE;

	char *output = NULL;
	char *input = NULL;

	unsigned char *buffer, *obuffer;
	int size, osize;

	while (1) {
		int option_index;
		static struct option longopt[] = {
			{"output", 1, 0, 'o'},
			{"lzma", 0, 0, 'l'},
			{"nocompress", 0, 0, 'n'},
		};

		signed char ch = getopt_long(argc, argv, "o:ln",
					     longopt, &option_index);

		if (ch == -1)
			break;

		switch (ch) {
		case 'o':
			output = optarg;
			break;
		case 'l':
			algo = CBFS_COMPRESS_LZMA;
			break;
		case 'n':
			algo = CBFS_COMPRESS_NONE;
			break;
		default:
			//usage();
			return -1;
		}
	}

	if (optind < argc)
		input = argv[optind];

	if (input == NULL || !strcmp(input, "-"))
		buffer = file_read_to_buffer(STDIN_FILENO, &size);
	else {
		printf("Reading from %s\n", input);
		buffer = file_read(input, &size);
	}

	if (!iself(buffer)) {
		fprintf(stderr, "E:  This does not appear to be an ELF file\n");
		return -1;
	}

	switch (algo) {
	case CBFS_COMPRESS_NONE:
		compress = none_compress;
		break;
	case CBFS_COMPRESS_LZMA:
		compress = lzma_compress;
		break;
	default:
		fprintf(stderr, "E: Unknown compression algorithm %d!\n", algo);
		return -1;
	}

	osize = parse_elf(buffer, &obuffer, algo, compress);

	if (osize == -1) {
		fprintf(stderr, "E:  Error while converting the payload\n");
		return -1;
	}

	if (output == NULL || !strcmp(output, "-"))
		file_write_from_buffer(STDOUT_FILENO, obuffer, osize);
	else
		file_write(output, obuffer, osize);

	return 0;
}
