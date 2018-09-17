/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <boardid.h>
#include <ec/google/chromeec/ec.h>

uint32_t board_id(void)
{
	MAYBE_STATIC uint32_t id = BOARD_ID_INIT;

	if (id == BOARD_ID_INIT) {
		if (google_chromeec_get_board_version(&id))
			id = BOARD_ID_UNKNOWN;
	}

	return id;
}
