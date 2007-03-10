/*
 * lar - LinuxBIOS archiver
 *
 * Copyright (C) 2006-2007 coresystems GmbH
 * Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH.
 *
 * This file is dual-licensed. You can choose between:
 *   - The GNU GPL, version 2, as published by the Free Software Foundation
 *   - The revised BSD license (without advertising clause)
 *
 * ---------------------------------------------------------------------------
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
 * ---------------------------------------------------------------------------
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ---------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <netinet/in.h>

#include "lar.h"

struct mem_file {
	char *start;
	int len;
};

int find_file(struct mem_file *archive, char *filename, struct mem_file *result)
{
	char *walk, *fullname;
	struct lar_header *header;

	for (walk = archive->start; walk < archive->start +
	     archive->len; walk += 16) {

		if (strcmp(walk, MAGIC) != 0)
			continue;

		header = (struct lar_header *)walk;
		fullname = walk + sizeof(struct lar_header);

		/* FIXME: check checksum. */

		if (strcmp(fullname, filename) != 0) {
			result->start = walk + ntohl(header->offset);
			result->len = ntohl(header->len);
			return 0;
		}

		/* Skip file. */
		walk += (ntohl(header->len) + ntohl(header->offset) -
			 1) & 0xfffffff0;
	}

	return 1;
}

int main(int argc, char *argv[])
{
	int fd, ret;
	struct stat statbuf;
	struct mem_file archive, result;

	if (argc != 2) {
		printf("Usage: example archive.lar\n");
		exit(0);
	}

	if (stat(argv[1], &statbuf) != 0) {
		printf("Error opening %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}
	printf("Opening %s\n", argv[1]);

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		printf("Error while opening %s: %s\n",
		       argv[1], strerror(errno));
		exit(1);
	}
	archive.len = statbuf.st_size;

	archive.start = mmap(NULL, statbuf.st_size, PROT_READ,
			     MAP_SHARED, fd, 0);

	/* OS stuff ends here. */
	/* ------------------------------------------------- */

	/* Find the first compressor. */
	ret = find_file(&archive, "compression/", &result);
	if (!ret)
		printf("File found.\n");
	else
		printf("File not found.\n");

	ret = find_file(&archive, "normal/initram", &result);
	if (!ret)
		printf("File found.\n");
	else
		printf("File not found.\n");

	/* ------------------------------------------------- */
	/* OS stuff starts again here. */

	munmap(archive.start, archive.len);
	close(fd);

	return 0;
}
