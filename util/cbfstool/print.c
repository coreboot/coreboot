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

#include <string.h>
#include "cbfstool.h"

void print_usage(void)
{
	printf("print\t\t\t\tShow the contents of the ROM\n");
}

int print_handler(struct rom *rom, int argc, char **argv)
{
	if ( argc > 0 ) {
		ERROR("print %s? print takes no arguments.\n", argv[0]);
		print_usage();
		return -1;
	}

	printf("%s: %d kB, bootblocksize %d, romsize %d, offset 0x%x\n", rom->name, rom->size / 1024, 
				ntohl(rom->header->bootblocksize), ntohl(rom->header->romsize), ntohl(rom->header->offset));
	printf("Alignment: %d bytes\n\n", ntohl(rom->header->align));

	struct cbfs_file *c = rom_find_first(rom);

	printf("%-30s Offset     %-12s Size\n", "Name", "Type");

	while (c) {
		char type[12];

		switch (htonl(c->type)) {
		case CBFS_COMPONENT_STAGE:
			strcpy(type, "stage");
			break;
		case CBFS_COMPONENT_PAYLOAD:
			strcpy(type, "payload");
			break;
		case CBFS_COMPONENT_OPTIONROM:
			strcpy(type, "optionrom");
			break;
		case CBFS_COMPONENT_NULL:
			strcpy(type, "free");
			break;
		default:
			sprintf(type, "0x%8.8x", htonl(c->type));
			break;
		}

		printf("%-30s 0x%-8x %-12s %d\n", CBFS_NAME(c),
		       ROM_OFFSET(rom, c), type, htonl(c->len));

		c = rom_find_next(rom, c);
	}

	return 0;
}
