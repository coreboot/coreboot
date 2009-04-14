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

#include <stdio.h>
#include "elf.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "common.h"

#define BLOCKSIZE (1024 * 16)

void file_write_from_buffer(int fd, unsigned char *buffer, int size)
{
	unsigned char *ptr = buffer;

	while (size) {
		int ret = write(fd, ptr, size);

		if (ret == -1)
			break;

		size -= ret;
		ptr += ret;
	}
}

unsigned char *file_read_to_buffer(int fd, int *fsize)
{
	unsigned char *buffer = malloc(BLOCKSIZE);
	unsigned char *ptr = buffer;

	int bsize = BLOCKSIZE;
	int remain = BLOCKSIZE;
	int size = 0;
	int ret;

	if (buffer == NULL)
		return NULL;

	while (1) {
		ret = read(fd, ptr, remain);

		if (ret <= 0)
			break;

		remain -= ret;
		ptr += ret;
		size += ret;

		/* Allocate more memory */

		if (remain == 0) {
			buffer = realloc(buffer, bsize + BLOCKSIZE);

			if (buffer == NULL)
				return NULL;

			ptr = buffer + size;

			bsize += BLOCKSIZE;
			remain = BLOCKSIZE;
		}
	}

	if (ret == 0) {
		*fsize = size;
		return buffer;
	}

	*fsize = 0;
	free(buffer);
	return NULL;
}

unsigned char *file_read(const char *filename, int *fsize)
{
	int fd = open(filename, O_RDONLY);
	unsigned char *buffer;

	if (fd == -1)
		return NULL;

	buffer = file_read_to_buffer(fd, fsize);
	close(fd);
	return buffer;
}

void file_write(const char *filename, unsigned char *buffer, int size)
{
	int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC,
		      S_IRUSR | S_IWUSR);

	if (fd == -1) {
		fprintf(stderr, "E: Could not create %s: %m\n", filename);
		return;
	}

	file_write_from_buffer(fd, buffer, size);
	close(fd);
}

int iself(unsigned char *input)
{
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *) input;
	return !memcmp(ehdr->e_ident, ELFMAG, 4);
}
