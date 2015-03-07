/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <stdlib.h>
#include <string.h>

#include <boardid.h>
#include <cbfs_core.h>
#include <console/console.h>

#include "mainboard/google/urara/urara_boardid.h"

/* Name of the CBFS file were the board ID string is read from. */
#define CBFS_BOARD_ID_FILE_NAME "board_id"

const struct bid_map {
	const char *board_name;
	uint8_t   board_id;
} board_id_map[] = {
	{"urara", URARA_BOARD_ID_BUB},
	{"buranku", URARA_BOARD_ID_BURANKU},
	{"derwent", URARA_BOARD_ID_DERWENT},
	{"jaguar", URARA_BOARD_ID_JAGUAR},
	{"kennet", URARA_BOARD_ID_KENNET},
	{"space", URARA_BOARD_ID_SPACE},
};

static int cached_board_id = -1;

static uint8_t retrieve_board_id(void)
{
	struct cbfs_file *board_id_file;
	const char *board_id_file_name = CBFS_BOARD_ID_FILE_NAME;
	char *file_contents;
	int i;
	unsigned length;

	board_id_file = cbfs_get_file(CBFS_DEFAULT_MEDIA, board_id_file_name);
	if (!board_id_file) {
		printk(BIOS_WARNING,
		       "board_id: failed to locate file '%s'\n",
		       board_id_file_name);
		return 0;
	}

	length = be32_to_cpu(board_id_file->len);

	file_contents = cbfs_get_file_content(CBFS_DEFAULT_MEDIA,
					      board_id_file_name,
					      CBFS_TYPE_RAW, NULL);

	if (!file_contents) {
		printk(BIOS_WARNING, "board_id: failed to read file '%s'\n",
		       board_id_file_name);
		return 0;
	}

	for (i = 0; i < ARRAY_SIZE(board_id_map); i++) {
		const struct bid_map *entry = board_id_map + i;

		if ((strlen(entry->board_name) == length) &&
		    !strncmp(entry->board_name, file_contents, length)) {
			printk(BIOS_INFO, "board_id: name '%s', ID %d\n",
			       entry->board_name, entry->board_id);
			return entry->board_id;
		}
	}

	printk(BIOS_WARNING, "board_id: no match for board name '%.*s'\n",
	       length, file_contents);
	printk(BIOS_WARNING, "board_id: will use default board ID 0\n");

	return 0;
}

uint8_t board_id(void)
{
	if (cached_board_id == -1)
		cached_board_id = retrieve_board_id();

	return cached_board_id;
}

