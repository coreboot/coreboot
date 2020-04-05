/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <ec/google/chromeec/ec.h>
#include <variant/board_version.h>

const char *samus_board_version(void)
{
	uint32_t board_version = SAMUS_EC_BOARD_VERSION_UNKNOWN;

	google_chromeec_get_board_version(&board_version);
	switch (board_version) {
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
