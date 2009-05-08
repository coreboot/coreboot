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

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#include "cbfstool.h"

void resize_usage(void)
{
	printf("resize [SIZE] [ERASEBLOCK]\tResize the ROM\n");
}

int resize_handler(struct rom *rom, int argc, char **argv)
{
	unsigned int align, offset;
	int size;
	char null = '\0';
	int bootblocksize = ntohl(rom->header->bootblocksize);

	if (argc < 1) {
		resize_usage();
		return -1;
	}

	if (rom->fd <= 0) {
		ERROR("ROM file %s does not exist\n", rom->name);
		return -1;
	}

	align = ntohl(rom->header->align);
	size = get_size(argv[0]);

	if (argc >= 2)
		align = strtoul(argv[1], NULL, 0);

	if (size == rom->size && align == ntohl(rom->header->align)) {
		ERROR("New parameters are the same as the old\n");
		return 0;
	}

	if (size < bootblocksize) {
		ERROR("The new size is smaller then the bootblock size\n");
		return -1;
	}

	/* if the current ROM is too big for the new size, then complain */

	if (rom_used_space(rom) > size -  bootblocksize) {
		ERROR("The new size is too small for the current ROM\n");
		return -1;
	}

	/* Grow the rom if we need to */

	if (size > rom->size) {
		munmap(rom->ptr, rom->size);

		lseek(rom->fd, size - 1, SEEK_SET);
		write(rom->fd, &null, 1);

		rom->ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED,
				rom->fd, 0);

		if (rom->ptr == MAP_FAILED) {
			ERROR("Unable to grow the ROM\n");
			return -1;
		}
	}

	/* We only have to rewrite the entries if the alignment changed */

	if (align != ntohl(rom->header->align)) {
		struct cbfs_file *c;

		/* The first entry doesn't have to move */

		c = rom_find(rom, rom->header->offset);
		offset = rom->header->offset;

		while (c) {
			struct cbfs_file *n = rom_find_next(rom, c);
			unsigned int next;

			if (n == NULL)
				break;

			/* Calculate a new location for the entry */
			next =
			    ROM_OFFSET(rom,
				       c) + ALIGN(ntohl(c->offset) +
						  ntohl(c->len), align);

			/* Copy the next entry there */
			memmove(ROM_PTR(rom, next), n,
				ntohl(n->offset) + ntohl(n->len));

			c = (struct cbfs_file *)ROM_PTR(rom, next);

			/* If the previous header wasn't overwritten by the change,
			   corrupt the header so we don't accidently find it */

			if (ROM_OFFSET(rom, n) >
			    next + ntohl(c->len) + ntohl(c->offset))
				memset(n->magic, 0, sizeof(n->magic));
		}
	}

	/* Copy the bootblock */

	memmove(rom->ptr + size -  bootblocksize,
		rom->ptr + rom->size -  bootblocksize,  bootblocksize);

	/* Recacluate the location of the header */

	offset = ROM_READL(rom, size - 12);

	rom->header = (struct cbfs_header *)
	    ROM_PTR(rom, size - (0xFFFFFFFF - offset) - 1);

	/* Put the new values in the header */
	rom->header->romsize = htonl(size);
	rom->header->align = htonl(align);

	/* Truncate the file if we have to */

	if (size < rom->size) {
		munmap(rom->ptr, rom->size);

		rom->ptr = NULL;

		if (ftruncate(rom->fd, size)) {
			ERROR("Unable to truncate the ROM\n");
			return -1;
		}

		rom->ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED,
				rom->fd, 0);

		if (rom->ptr == MAP_FAILED) {
			ERROR("Unable to truncate the ROM\n");
			return -1;
		}
	}

	rom->size = size;
	rom->fssize = size -  bootblocksize;

	return 0;
}
