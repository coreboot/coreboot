/*
 * This file is part of the coreboot project.
 *
 * Copyright(C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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
 * Foundation, Inc.
 */

#include <boardid.h>
#include <stdlib.h>
#include "ec/google/chromeec/ec.h"

uint8_t board_id(void)
{
	MAYBE_STATIC int id = -1;

#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
	if (id < 0)
		id = google_chromeec_get_board_version();
#endif
	return id;
}

