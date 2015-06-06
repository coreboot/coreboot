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
 * Foundation, Inc.
 */

#include <stdlib.h>
#include <string.h>

#include <boardid.h>
#include <cbfs.h>
#include <console/console.h>

#include "mainboard/google/urara/urara_boardid.h"

/* Name of the CBFS file were the board ID string is read from. */
#define CBFS_BOARD_ID_FILE_NAME "board_id"

const struct bid_map {
	const char *board_name;
	uint8_t   board_id;
	struct board_hw hardware;
} board_id_map[] = {
	{"urara", URARA_BOARD_ID_BUB, {0} },
	{"buranku", URARA_BOARD_ID_BURANKU, {3} },
	{"derwent", URARA_BOARD_ID_DERWENT, {3} },
	{"jaguar", URARA_BOARD_ID_JAGUAR, {3} },
	{"kennet", URARA_BOARD_ID_KENNET, {3} },
	{"space", URARA_BOARD_ID_SPACE, {3} },
};

static int cached_board_id = -1;

static uint8_t retrieve_board_id(void)
{
	const char *board_id_file_name = CBFS_BOARD_ID_FILE_NAME;
	char *file_contents;
	int i;
	size_t length;

	file_contents = cbfs_boot_map_with_leak(board_id_file_name,
						CBFS_TYPE_RAW, &length);

	if (!file_contents) {
		printk(BIOS_WARNING,
		       "board_id: failed to locate file '%s'\n",
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

const struct board_hw *board_get_hw(void)
{
	int i;
	uint8_t bid = board_id();

	for (i = 0; i < ARRAY_SIZE(board_id_map); i++) {
		if (bid == board_id_map[i].board_id)
			return &(board_id_map[i].hardware);
	}

	return 0;
}

uint8_t board_id(void)
{
	if (cached_board_id == -1)
		cached_board_id = retrieve_board_id();

	return cached_board_id;
}
