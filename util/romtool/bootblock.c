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

void bootblock_usage(void)
{
	printf("bootblock [FILE]\t\tAdd a bootblock to the ROM\n");
}

int bootblock_handler(struct rom *rom, int argc, char **argv)
{
	if (argc < 1) {
		bootblock_usage();
		return -1;
	}

	return add_bootblock(rom, argv[0]);
}
