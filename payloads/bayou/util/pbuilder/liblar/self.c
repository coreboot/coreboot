/*
 * This file is part of the bayou project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * Includes code from util/lar from coreboot-v3
 *
 * Copyright (C) 2006-2007 coresystems GmbH
 * Copyright (C) 2007 Patrick Georgi <patrick@georgi-clan.de>
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "elf.h"
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

typedef uint64_t u64;
typedef int64_t s64;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint8_t u8;

#include "self.h"

int elf_to_self(const char *filename, unsigned char **buffer,
		void (*compress) (char *, int, char *, int *))
{
	struct stat s;
	Elf32_Phdr *phdr;
	Elf32_Ehdr *ehdr;
	Elf32_Shdr *shdr;
	void *filep;
	char *header, *strtab;
	unsigned char *sptr;
	int headers, segments = 1, isize = 0, osize = 0, doffset = 0, fd, i;
	struct self_segment *segs;

	if (stat(filename, &s)) {
		printf("Unable to stat %s: %m\n", filename);
		return -1;
	}

	fd = open(filename, O_RDONLY);

	if (fd == -1) {
		printf("Unable to open %s: %m\n", filename);
		return -1;
	}

	/* Map the file so that we can easily parse it. */
	filep = (void *)
	    mmap(0, s.st_size, PROT_READ, MAP_SHARED, fd, 0);

	if (filep == MAP_FAILED) {
		close(fd);
		return -1;
	}

	ehdr = (Elf32_Ehdr *) filep;
	headers = ehdr->e_phnum;
	header = (char *)ehdr;

	phdr = (Elf32_Phdr *) & (header[ehdr->e_phoff]);
	shdr = (Elf32_Shdr *) & (header[ehdr->e_shoff]);

	strtab = &header[shdr[ehdr->e_shstrndx].sh_offset];

	/* Count number of headers - look for the .notes.pinfo section. */
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

	/*
	 * Now, regular headers - we only care about PT_LOAD headers,
	 * because thats what we're actually going to load.
	 */
	for (i = 0; i < headers; i++) {
		if (phdr[i].p_type != PT_LOAD)
			continue;

		/* Empty segments are never interesting. */
		if (phdr[i].p_memsz == 0)
			continue;

		isize += phdr[i].p_filesz;
		segments++;
	}

	/* Allocate a block of memory to store the SELF in. */
	sptr = calloc((segments * sizeof(struct self_segment)) + isize, 1);
	doffset = (segments * sizeof(struct self_segment));

	if (sptr == NULL)
		goto err;

	segs = (struct self_segment *)sptr;
	segments = 0;

	for (i = 0; i < ehdr->e_shnum; i++) {
		char *name;

		if (i == ehdr->e_shstrndx)
			continue;

		if (shdr[i].sh_size == 0)
			continue;

		name = (char *)(strtab + shdr[i].sh_name);

		if (!strcmp(name, ".note.pinfo")) {
			segs[segments].type = SELF_TYPE_PARAMS;
			segs[segments].load_addr = 0;
			segs[segments].len = (u32) shdr[i].sh_size;
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

		segs[segments].type = SELF_TYPE_DATA;
		segs[segments].load_addr = (u64) phdr[i].p_paddr;
		segs[segments].mem_len = (u32) phdr[i].p_memsz;
		segs[segments].offset = doffset;

		compress((char *)&header[phdr[i].p_offset],
			 phdr[i].p_filesz,
			 (char *)(sptr + doffset), (int *)&segs[segments].len);

		doffset += segs[segments].len;
		osize += segs[segments].len;

		segments++;
	}

	segs[segments].type = SELF_TYPE_ENTRY;
	segs[segments++].load_addr = (unsigned long long)ehdr->e_entry;

	*buffer = sptr;

	munmap(filep, s.st_size);
	close(fd);

	return (segments * sizeof(struct self_segment)) + osize;

err:
	munmap(filep, s.st_size);
	close(fd);

	return -1;
}

int iself(char *filename)
{
	Elf32_Ehdr ehdr;
	int fd = open(filename, O_RDONLY);
	int ret = 0;

	if (fd == -1)
		return 0;

	if (read(fd, &ehdr, sizeof(ehdr)) == sizeof(ehdr))
		ret = !memcmp(ehdr.e_ident, ELFMAG, 4);

	close(fd);

	return ret;
}
