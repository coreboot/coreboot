/*
 * romtool
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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "romtool.h"

void create_usage(void)
{
	printf("create SIZE BOOTBLOCKSIZE [ALIGN] [BOOTBLOCK]\tCreate a ROM file\n");
}

int create_handler(struct rom *rom, int argc, char **argv)
{
	int align = 16, size;
	char *bootblock = NULL;
	int bootblocksize;

	if (argc < 2) {
		create_usage();
		return -1;
	}

	size = get_size(argv[0]);

	bootblocksize = get_size(argv[1]);

	if (argc == 3) {
		bootblock = argv[2];
	} else if (argc >= 4) {
		align = strtoul(argv[2], NULL, 0);
		bootblock = argv[3];
	}

	if (size < bootblocksize) {
		ERROR("Cannot create a rom %d smaller then bootblock size %d\n", size, bootblocksize);
		return -1;
	}

	if (align == 0) {
		ERROR("Cannot create with an align size of 0\n");
		return -1;
	}

	if (create_rom(rom, rom->name, size, bootblocksize, align))
		return -1;

	if (bootblock != NULL)
		return add_bootblock(rom, bootblock);

	return 0;
}
