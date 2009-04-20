/*
 * cbfstool
 *
 * Copyright (C) 2009 Myles Watson <mylesgw@gmail.com>
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
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "cbfstool.h"

static int extract_blob(struct rom *rom, const char *filename, const char *name)
{
	void *buf;
	int fd, ret;
	unsigned long size;

	ret = rom_extract(rom, name, &buf, &size);

	if (ret == -1) 
		return -1;

	fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

	if (fd == -1) {
		ERROR("Could not open %s: %m\n", filename);
		return -1;
	}

	if (write(fd, buf, size) != size) {
		ERROR("Couldn't write %ld bytes!\n", size);
		ret = -1;
	}

	close(fd);

	return ret;
}

void extract_usage(void)
{
	printf("extract [FILE] [NAME] \textract a component\n");
}

int extract_handler(struct rom *rom, int argc, char **argv)
{
	if (argc < 2) {
		extract_usage();
		return -1;
	}

	if (!rom_exists(rom)) {
		ERROR("Can't find the ROM!\n");
		return -1;
	}

	return extract_blob(rom, argv[0], argv[1]);
}

