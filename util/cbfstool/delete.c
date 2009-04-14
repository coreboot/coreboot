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
#include <stdlib.h>
#include <unistd.h>
#include "cbfstool.h"

void delete_usage(void)
{
	printf("delete [NAME] ...\t\tDelete a component\n");
}

int delete_handler(struct rom *rom, int argc, char **argv)
{
	int i;
	int ret = 0;

	if (argc < 1) {
		delete_usage();
		return -1;
	}

	for (i = 0; i < argc; i++)
		ret |= rom_remove(rom, argv[i]);

	return ret;
}
