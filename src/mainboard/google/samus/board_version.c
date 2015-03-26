/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#include <ec/google/chromeec/ec.h>
#include "board_version.h"

const char *samus_board_version(void)
{
	switch (google_chromeec_get_board_version()) {
	case SAMUS_EC_BOARD_VERSION_EVT1:
		return "EVT1";
	case SAMUS_EC_BOARD_VERSION_EVT2:
		return "EVT2";
	case SAMUS_EC_BOARD_VERSION_EVT3:
		return "EVT3";
	case SAMUS_EC_BOARD_VERSION_EVT4:
		return "EVT4";
	default:
		return "Unknown";
	}
}
